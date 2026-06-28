/* Copyright 2026 K5-SIG contributors
 *
 * Licensed under the Apache License, Version 2.0.
 */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "app/chFrScanner.h"
#include "app/sigint.h"
#include "driver/eeprom.h"
#ifdef ENABLE_UART
	#include "driver/uart.h"
#endif
#include "frequencies.h"
#include "misc.h"
#include "radio.h"
#include "settings.h"

#define SIGINT_BANK_ALL_MASK 0x003Fu
#define SIGINT_BANK_COUNT    6u

typedef struct {
	const char *Name;
	uint32_t Lower;
	uint32_t Upper;
	STEP_Setting_t Step;
	ModulationMode_t Modulation;
	uint8_t Bandwidth;
} SIGINT_ScanPreset_t;

typedef struct {
	const char *Name;
	uint8_t FirstChannel;
	uint8_t LastChannel;
} SIGINT_MemoryBank_t;

static SIGINT_Record_t gLastRecord;
static SIGINT_EventType_t gLastEventType;
static bool gHasLastRecord;
static const SIGINT_ScanPreset_t *gActiveScanPreset;
static uint16_t gEnabledBankMask;
static char gLastBankMessage[16];

static const SIGINT_ScanPreset_t gScanPresets[] = {
	{ "AIR AM",   11800000, 13697500, STEP_25kHz,   MODULATION_AM, BANDWIDTH_WIDE   },
	{ "PMR446",   44600000, 44620000, STEP_12_5kHz, MODULATION_FM, BANDWIDTH_NARROW },
	{ "MARINE",   15600000, 16202500, STEP_25kHz,   MODULATION_FM, BANDWIDTH_NARROW },
	{ "HAM 2M",   14400000, 14600000, STEP_12_5kHz, MODULATION_FM, BANDWIDTH_NARROW },
	{ "HAM 70",   43000000, 44000000, STEP_12_5kHz, MODULATION_FM, BANDWIDTH_NARROW },
	{ "VHF HI",   13700000, 17400000, STEP_12_5kHz, MODULATION_FM, BANDWIDTH_NARROW },
	{ "UHF",      40000000, 47000000, STEP_12_5kHz, MODULATION_FM, BANDWIDTH_NARROW },
};

static const char gBankOffMsg[] = " OFF";
static const char gBankOnMsg[] = " ON";

static const SIGINT_MemoryBank_t gMemoryBanks[SIGINT_BANK_COUNT] = {
	{ "BANK 1", 0,   37  },
	{ "BANK 2", 38,  94  },
	{ "BANK 3", 95,  151 },
	{ "BANK 4", 152, 171 },
	{ "BANK 5", 172, 187 },
	{ "BANK 6", 188, 199 },
};

static const SIGINT_ScanPreset_t *SIGINT_FindScanPreset(uint32_t Frequency)
{
	for (unsigned int i = 0; i < ARRAY_SIZE(gScanPresets); i++) {
		if (Frequency >= gScanPresets[i].Lower && Frequency <= gScanPresets[i].Upper)
			return &gScanPresets[i];
	}

	return &gScanPresets[0];
}

static void SIGINT_UpdateBankMessageFromMask(void)
{
	if (gEnabledBankMask == SIGINT_BANK_ALL_MASK)
		strcpy(gLastBankMessage, "BANK ALL");
	else if (gEnabledBankMask == 0)
		strcpy(gLastBankMessage, "BANK NONE");
	else
		strcpy(gLastBankMessage, "BANK CUSTOM");
}

static void SIGINT_SaveBankMask(void)
{
	const uint8_t mask = gEnabledBankMask & SIGINT_BANK_ALL_MASK;
	EEPROM_WriteBuffer(0x0F1F, &mask);
}

static char SIGINT_HexNibble(uint8_t Value)
{
	Value &= 0x0F;
	return Value < 10 ? (char)('0' + Value) : (char)('A' + Value - 10);
}

static void SIGINT_SendBankEvent(void)
{
#ifdef ENABLE_UART
	char line[] = "K5SIG,EVT=BANK,MASK=0x00XX\r\n";
	const uint8_t mask = gEnabledBankMask & SIGINT_BANK_ALL_MASK;
	line[24] = SIGINT_HexNibble(mask >> 4);
	line[25] = SIGINT_HexNibble(mask);
	UART_Send((uint8_t *)line, sizeof(line) - 1);
#endif
}

void SIGINT_Init(void)
{
	memset(&gLastRecord, 0, sizeof(gLastRecord));
	gLastEventType = SIGINT_EVENT_HIT;
	gHasLastRecord = false;
	gActiveScanPreset = NULL;
	gEnabledBankMask = SIGINT_BANK_ALL_MASK;
	SIGINT_UpdateBankMessageFromMask();
}

bool SIGINT_ApplyScanPresetForCurrentVfo(void)
{
#ifndef ENABLE_SCAN_RANGES
	return false;
#else
	if (gRxVfo == NULL || IS_MR_CHANNEL(gRxVfo->CHANNEL_SAVE))
		return false;

	const SIGINT_ScanPreset_t *preset = SIGINT_FindScanPreset(gRxVfo->pRX->Frequency);
	const uint16_t step = gStepFrequencyTable[preset->Step];

	gRxVfo->STEP_SETTING = preset->Step;
	gRxVfo->StepFrequency = step;
	gRxVfo->Modulation = preset->Modulation;
	gRxVfo->CHANNEL_BANDWIDTH = preset->Bandwidth;
	gRxVfo->freq_config_RX.Frequency = preset->Lower;
	gRxVfo->freq_config_TX.Frequency = preset->Lower;
	gRxVfo->TX_OFFSET_FREQUENCY_DIRECTION = TX_OFFSET_FREQUENCY_DIRECTION_OFF;
	gRxVfo->TX_OFFSET_FREQUENCY = 0;

	gScanRangeStart = preset->Lower;
	gScanRangeStop = preset->Upper;
	gActiveScanPreset = preset;

	RADIO_ApplyOffset(gRxVfo);
	RADIO_ConfigureSquelchAndOutputPower(gRxVfo);
	RADIO_SetupRegisters(true);
	gUpdateDisplay = true;
	gUpdateStatus = true;

	return true;
#endif
}

const char *SIGINT_GetActiveScanPresetName(void)
{
	return gActiveScanPreset != NULL ? gActiveScanPreset->Name : NULL;
}

void SIGINT_ClearActiveScanPreset(void)
{
	gActiveScanPreset = NULL;
}

bool SIGINT_IsChannelInEnabledBank(uint8_t Channel)
{
	bool mapped = false;

	for (unsigned int i = 0; i < SIGINT_BANK_COUNT; i++) {
		const SIGINT_MemoryBank_t *bank = &gMemoryBanks[i];
		if (Channel >= bank->FirstChannel && Channel <= bank->LastChannel) {
			mapped = true;
			if (gEnabledBankMask & (1u << i))
				return true;
		}
	}

	return !mapped;
}

bool SIGINT_ToggleBankByKey(uint8_t Key)
{
	if (Key == 0) {
		gEnabledBankMask = (gEnabledBankMask == SIGINT_BANK_ALL_MASK) ? 0 : SIGINT_BANK_ALL_MASK;
		SIGINT_UpdateBankMessageFromMask();
		SIGINT_SaveBankMask();
		SIGINT_SendBankEvent();
		return true;
	}

	if (Key < 1 || Key > SIGINT_BANK_COUNT)
		return false;

	const uint16_t bit = 1u << (Key - 1u);
	gEnabledBankMask ^= bit;

	const SIGINT_MemoryBank_t *bank = &gMemoryBanks[Key - 1u];
	strcpy(gLastBankMessage, bank->Name);
	strcat(gLastBankMessage, (gEnabledBankMask & bit) ? gBankOnMsg : gBankOffMsg);
	SIGINT_SaveBankMask();
	SIGINT_SendBankEvent();

	return true;
}

bool SIGINT_FindFirstEnabledBankChannel(uint8_t *pChannel)
{
	if (pChannel == NULL || gEnabledBankMask == 0)
		return false;

	for (unsigned int i = 0; i <= MR_CHANNEL_LAST; i++) {
		if (SIGINT_IsChannelInEnabledBank(i) && RADIO_CheckValidChannel(i, false, 0)) {
			*pChannel = i;
			return true;
		}
	}

	return false;
}

void SIGINT_SetEnabledBankMask(uint8_t Mask)
{
	if (Mask == 0xFF || (Mask & ~SIGINT_BANK_ALL_MASK))
		gEnabledBankMask = SIGINT_BANK_ALL_MASK;
	else
		gEnabledBankMask = Mask & SIGINT_BANK_ALL_MASK;

	SIGINT_UpdateBankMessageFromMask();
}

uint8_t SIGINT_GetEnabledBankMask(void)
{
	return gEnabledBankMask & SIGINT_BANK_ALL_MASK;
}

const char *SIGINT_GetLastBankMessage(void)
{
	return gLastBankMessage;
}

void SIGINT_RecordEvent(SIGINT_EventType_t Type, const SIGINT_Record_t *pRecord)
{
	if (pRecord == NULL)
		return;

	gLastRecord = *pRecord;
	gLastEventType = Type;
	gHasLastRecord = true;
}

const SIGINT_Record_t *SIGINT_GetLastRecord(void)
{
	return gHasLastRecord ? &gLastRecord : NULL;
}

SIGINT_EventType_t SIGINT_GetLastEventType(void)
{
	return gLastEventType;
}
