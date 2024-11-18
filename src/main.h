
#ifndef MAIN_H

#define MAIN_H
#include "typedef.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../Lib/sdl2-2.0.10/include/SDL2/SDL.h"
#include "../Lib/unicorn-2.0.1-min/unicorn/unicorn.h"
#include <pthread.h>
#include "defined.h"
#include "config.h"

/**
 * 定义
 * 0-1024 为栈空间
 * 1024-4096为代码空间
 */
char *getRealMemPtr(u32 ptr);
void SimulatePressKey(u8, u8);
void RunArmProgram(void *);
void hookBlockCallBack(uc_engine *uc, uint64_t address, uint32_t size, void *user_data);
void hookCodeCallBack(uc_engine *uc, uint64_t address, uint32_t size, void *user_data);
void hookRamCallBack(uc_engine *uc, uc_mem_type type, uint64_t address, uint32_t size, int64_t value, u32 data);
void onCPRSChange(uc_engine *uc, uint64_t address, uint32_t size, u32 data);
void SaveCpuContext(u32 *stackPtr, u32 backAddr);
void RestoreCpuContext(u32 *stackPtr);
void renderGdiBufferToWindow(void);
void Update_RTC_Time(void);
int utf16_len(char *utf16);
bool writeSDFile(u8 *Buffer, u32 startPos, u32 size);
u8 *readSDFile(u32 startPos, u32 size);
bool writeFlashFile(u8 *Buffer, u32 startPos, u32 size);
void *readFlashFile(u32 startPos, u32 size);
void StartCallback(u32 callbackAddr, u32 backAddr, u32 r0);
bool StartInterrupt(u32, u32);
#endif