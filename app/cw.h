/* Copyright 2026 K5-SIG contributors
 *
 * Licensed under the Apache License, Version 2.0.
 */

#ifndef APP_CW_H
#define APP_CW_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	CW_KEY_STRAIGHT = 0,
	CW_KEY_PADDLE_IAMBIC_A,
	CW_KEY_PADDLE_IAMBIC_B,
	CW_KEY_PRACTICE_ONLY,
} CW_KeyMode_t;

typedef struct {
	uint8_t Wpm;
	uint16_t PitchHz;
	CW_KeyMode_t KeyMode;
	bool TxEnabled;
} CW_Config_t;

void CW_Init(void);
void CW_SetConfig(const CW_Config_t *pConfig);
const CW_Config_t *CW_GetConfig(void);
bool CW_IsTxArmed(void);

#endif
