
#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../Lib/sdl2-2.0.10/include/SDL2/SDL.h"
#include "../Lib/unicorn-2.0.1-min/unicorn/unicorn.h"
#include <pthread.h>
#include "typedef.h"
#include "defined.h"
#include "config.h"

typedef struct vm_event_
{
    u32 event;
    u32 r0;
    u32 r1;
} vm_event;

typedef struct VM_DMA_CONFIG_
{
    int control;
    DMA_MASTER_CHANEL chanel;
    DMA_DATA_DIRECTION direction;
    DMA_DATA_BYTE_ALIGN align;
    int data_addr;
    int transfer_count;
    int config_finish;
    int transfer_end_interrupt_enable;
    u8 cacheBuffer[4096];
} VM_DMA_CONFIG;

VM_DMA_CONFIG vm_dma_msdc_config;
VM_DMA_CONFIG vm_dma_sim1_config;
VM_DMA_CONFIG vm_dma_sim2_config;

u32 changeTmp = 0;
u32 changeTmp1 = 0;
u32 changeTmp2 = 0;
u32 changeTmp3 = 0;
static uc_engine *MTK;

typedef struct VM_SIM_DEV_
{
    SIM_DEV_EVENT event;
    bool irq_start;
    u8 is_rst;
    u8 rx_finish;
    SIM_IRQ_CHANNEL irq_channel;
    u32 irq_enable;
    u32 control;

    u8 rx_buffer[1024]; // 1kb txBuffer缓存
    u8 rx_remain_count;
    // 接收字节下标
    u8 rx_buffer_index;
    // 本次命令接收字节下标
    u32 rx_current_index;

    u8 tx_buffer[1024]; // 1kb txBuffer缓存
    u8 tx_buffer_index;
    u8 T0RxData[512];
    u32 t0_tx_count;
    u32 rx_trigger_count;
    u32 tx_trigger_count;
} VM_SIM_DEV;

VM_SIM_DEV vm_sim1_dev;
VM_SIM_DEV vm_sim2_dev;

u32 lastAddress = 0;
u32 debugRamAddress = 0;

void SIM_TIDE_HANDLE(VM_SIM_DEV *sim_dev, u8 sim_num, int64_t value);
void SIM_IRQ_HANDLE(VM_SIM_DEV *sim_dev, u8 sim_num, int64_t value);
void SIM_BASE_HANDLE(VM_SIM_DEV *sim_dev, u8 sim_num, int64_t value);
void SIM_DATA_HANDLE(VM_SIM_DEV *sim_dev, u8 sim_num, u8 isWrite, int64_t value);
void handle_sim_tx_cmd(VM_SIM_DEV *sim_dev, u8 sim_num, u32 data_count, u32 dma_data_addr);
void handle_sim_rx_cmd(VM_SIM_DEV *sim_dev, u8 sim_num, u32 data_count, u32 dma_data_addr);

u8 SIM_ATR_RSP_DATA[] = {0x3b, 0x00, 1, 2, 3, 4, 5, 6};
// u8 SIM_ATR_RSP_DATA[] = {0x3B, 0x9F, 0x94, 0x80, 0x1F, 0xC7, 0x80, 0x31, 0xE0, 0x73, 0xFE, 0x21, 0x13, 0x57, 0x58, 0x48, 0x55, 0x53, 0x49, 0x4D, 0x01, 0xF9};
//  9f表示存在历史字符，且长度为15
//  TS=3B T0=9F TA1=94 TD1=80 TD2=1F TA3=C7(时钟停止休止符：无优先 级别指示符：A、B、和C) TCK=F9 Protocal=TO AtrBinarySize=22 AtrHistorySize=15 AtrHistorySize=8031E073FE21135758485553494D01
u8 SIM_CMD_SELECT[] = {0xa0, 0xa4, 0x00, 0x00, 0x02};     // 选择文件命令
u8 SIM_CMD_GET_RESPONSE[] = {0xa0, 0xc0, 0x0, 0x0, 0x16}; // 读取响应命令

// select df.gsm
u8 SIM_CMD_SELECT_DF_GSM[] = {0x7f, 0x20};
// u8 SIM_RSP_SF_7F20[] = {0x62, 0x1E, 0x82, 0x02, 0x38, 0x02, 0x83, 0x02, 0x7F, 0x20, 0xA5, 0x07, 0x80, 0x01, 0x00, 0x83, 0x02, 0x6F, 0x38, 0xC6, 0x08, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00};
u8 SIM_RSP_SF_7F20[] = {0xA0, 0xC0, 0x32, 0x32, 0x0F, 0x32, 0x32, 0x32, 0x08, 0x2F, 0x05, 0x04, 0x34, 0x01, 0xFF, 0x55, 0x01, 0x02, 0x32, 0x32, 0x90, 0x32};

u32 lastSIM_DMA_ADDR = 0;
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
void StartCallback(u32 callbackFuncAddr, u32 r0);
bool StartInterrupt(u32, u32);
void dumpMemoryToFile(char *filename, u32 virt_addr, int size);
void EnqueueVMEvent(u32 event, u32 r0, u32 r1);
vm_event *DequeueVMEvent();
void dumpCpuInfo();
void hookRamCallBack(uc_engine *uc, uc_mem_type type, uint64_t address, uint32_t size, int64_t value, u32 data);