#pragma once

#include <stddef.h>
#include <stdint.h>

extern void Spectrum_Init(uint32_t sample_rate_hz);
extern void Spectrum_UpdateFromInterleaved(const int16_t *samples,
                                    size_t sample_count,
                                    uint32_t channels);
extern void Spectrum_DrawIfDue(void);
extern uint32_t Spectrum_GetBinCount(void);
extern uint16_t Spectrum_GetLevel(uint32_t index);
extern uint16_t Spectrum_GetBinColor(uint32_t index);
extern uint16_t Spectrum_GetMaxHeight(void);