/* Copyright 2026 K5-SIG contributors
 *
 * Licensed under the Apache License, Version 2.0.
 */

#include <stddef.h>

#include "app/cw.h"

static CW_Config_t gCwConfig;

void CW_Init(void)
{
	gCwConfig.Wpm = 15;
	gCwConfig.PitchHz = 700;
	gCwConfig.KeyMode = CW_KEY_PRACTICE_ONLY;
	gCwConfig.TxEnabled = false;
}

void CW_SetConfig(const CW_Config_t *pConfig)
{
	if (pConfig == NULL)
		return;

	gCwConfig = *pConfig;
}

const CW_Config_t *CW_GetConfig(void)
{
	return &gCwConfig;
}

bool CW_IsTxArmed(void)
{
	return gCwConfig.TxEnabled;
}
