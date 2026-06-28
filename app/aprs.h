/* Copyright 2026 K5-SIG contributors
 *
 * Licensed under the Apache License, Version 2.0.
 */

#ifndef APP_APRS_H
#define APP_APRS_H

#include <stdbool.h>
#include <stdint.h>

#define APRS_CALLSIGN_MAX_LEN 6
#define APRS_PATH_MAX_LEN     16
#define APRS_COMMENT_MAX_LEN  32

typedef struct {
	char Callsign[APRS_CALLSIGN_MAX_LEN + 1];
	uint8_t Ssid;
	char Path[APRS_PATH_MAX_LEN + 1];
	char SymbolTable;
	char Symbol;
	char Comment[APRS_COMMENT_MAX_LEN + 1];
	bool AutoBeaconEnabled;
} APRS_Config_t;

void APRS_Init(void);
void APRS_SetConfig(const APRS_Config_t *pConfig);
const APRS_Config_t *APRS_GetConfig(void);
bool APRS_IsAutoBeaconEnabled(void);

#endif
