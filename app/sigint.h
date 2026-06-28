/* Copyright 2026 K5-SIG contributors
 *
 * Licensed under the Apache License, Version 2.0.
 */

#ifndef APP_SIGINT_H
#define APP_SIGINT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	SIGINT_MODE_UNKNOWN = 0,
	SIGINT_MODE_FM,
	SIGINT_MODE_NFM,
	SIGINT_MODE_AM,
	SIGINT_MODE_CW,
	SIGINT_MODE_RAW,
} SIGINT_Mode_t;

typedef enum {
	SIGINT_EVENT_HIT = 0,
	SIGINT_EVENT_BOOKMARK,
	SIGINT_EVENT_CW_TX,
	SIGINT_EVENT_APRS_TX,
} SIGINT_EventType_t;

typedef struct {
	uint32_t FrequencyHz;
	int16_t  RssiDbm;
	int16_t  PeakRssiDbm;
	uint32_t Time10ms;
	SIGINT_Mode_t Mode;
} SIGINT_Record_t;

void SIGINT_Init(void);
bool SIGINT_ApplyScanPresetForCurrentVfo(void);
const char *SIGINT_GetActiveScanPresetName(void);
void SIGINT_ClearActiveScanPreset(void);
bool SIGINT_IsChannelInEnabledBank(uint8_t Channel);
bool SIGINT_ToggleBankByKey(uint8_t Key);
bool SIGINT_FindFirstEnabledBankChannel(uint8_t *pChannel);
void SIGINT_SetEnabledBankMask(uint8_t Mask);
uint8_t SIGINT_GetEnabledBankMask(void);
const char *SIGINT_GetLastBankMessage(void);
void SIGINT_RecordEvent(SIGINT_EventType_t Type, const SIGINT_Record_t *pRecord);
const SIGINT_Record_t *SIGINT_GetLastRecord(void);
SIGINT_EventType_t SIGINT_GetLastEventType(void);

#endif
