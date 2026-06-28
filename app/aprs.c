/* Copyright 2026 K5-SIG contributors
 *
 * Licensed under the Apache License, Version 2.0.
 */

#include <stddef.h>
#include <string.h>

#include "app/aprs.h"

static APRS_Config_t gAprsConfig;

void APRS_Init(void)
{
	memset(&gAprsConfig, 0, sizeof(gAprsConfig));
	memcpy(gAprsConfig.Callsign, "NOCALL", 6);
	gAprsConfig.Ssid = 7;
	memcpy(gAprsConfig.Path, "WIDE1-1", 7);
	gAprsConfig.SymbolTable = '/';
	gAprsConfig.Symbol = '[';
	gAprsConfig.AutoBeaconEnabled = false;
}

void APRS_SetConfig(const APRS_Config_t *pConfig)
{
	if (pConfig == NULL)
		return;

	gAprsConfig = *pConfig;
}

const APRS_Config_t *APRS_GetConfig(void)
{
	return &gAprsConfig;
}

bool APRS_IsAutoBeaconEnabled(void)
{
	return gAprsConfig.AutoBeaconEnabled;
}
