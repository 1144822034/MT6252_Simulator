#include "main.h"
#include "myui.c"
#include "mysocket.c"

typedef struct _bitmap
{
    u32 xsize;
    u32 ysize;
    u8 color_depth;
    u32 row_bytes;
    u32 palette_ptr;
    u32 data_ptr;
    u8 inited;
    u32 screen_buff_size;
    u8 *real_screen_buff;
} bitmap;
enum
{
    SD_DMA_Read,
    SD_DMA_Write
};
u32 SEND_SDCMD_CACHE; // SD命令缓存
u32 SDCMD_CACHE;
u32 SEND_SDDATA_CACHE; // SD数据缓存
u32 SD_READ_ADDR;      // SD文件系统读取地址
u32 SD_Write_ADDR;     // SD文件系统写入地址

u32 DMA_Data_Transfer_Ptr;    // DMA搬运数据源地址
u32 DMA_Transfer_Bytes_Count; // DMA搬运数据字节数
u32 SD_Multi_Read_Count;      // 连续读块数
u32 SD_DMA_Type;              // 0表示读取SD数据块 1表示写SD数据块

bitmap screen_bit;
u32 screenBufferSize = 480 * 320;

u8 ucs2Tmp[128] = {0}; // utf16-le转utf-8 缓存空间

socketHandle *sh1;

struct uc_context *callback_context;

struct uc_context *timer_isr_context;

FILE *SD_File_Handle;
FILE *FLASH_File_Handle;
static pthread_mutex_t mutex; // 线程锁
clock_t render_time;
clock_t last_timer_interrupt_time;
clock_t last_rtc_interrupt_time;
u32 lastFlashTime;
// 中断间隔
u32 interruptPeroidms = 3;
u32 lastAddress = 0;

static SDL_Window *window;
static SDL_Keycode isKeyDown = SDLK_UNKNOWN;
static bool isMouseDown = false;
static uc_engine *MTK;
static pthread_t emu_thread;
static pthread_t screen_render_thread;
int debugType = 0;
u8 globalSprintfBuff[256] = {0};
u8 sprintfBuff[256] = {0};
u8 currentProgramDir[256] = {0};

u32 stackCallback[17];
bool isEnterCallback;
int simulateKey = -1;
int simulatePress = -1;
u32 changeTmp = 0;
u32 changeTmp1 = 0;
u32 changeTmp2 = 0;
u32 changeTmp3 = 0;
u32 lastSprintfPtr = 0;

u32 lcdUpdateFlag = 0;
u32 size_32mb = 1024 * 1024 * 32;
u32 size_16mb = 1024 * 1024 * 16;
u32 size_8mb = 1024 * 1024 * 8;
u32 size_4mb = 1024 * 1024 * 4;
u32 size_1mb = 1024 * 1024;
u32 size_2kb = 1024 * 2;
u8 *dataCachePtr;

int irq_nested_count;
u32 *isrStackPtr;
u32 isrStackList[10][17];

u8 *ROM_MEMPOOL;
u8 *RAM_MEMPOOL;
u8 *RAM40_POOL;
u8 *RAMF0_POOL;

u8 enterIrqSave;
u32 buff1, buff2;
char *pp;

bool needUpdateLCD;
// 最多四层
u32 LCD_Layer_Address[4];

clock_t currentTime = 0;

u32 IRQ_MASK_SET_L_Data;

struct SF_Control
{
    u8 cmd;
    u32 address;
    u32 data;
    u32 len;
    u8 cmdRev; // 1 = 命令已接收
    bool sendDataMode;
    u32 sendDataCount;
    u32 sentCount;
    u32 cacheData[64];
    u32 readDataCount;
    u8 *SendData;
    u8 *InputData;
};

struct SF_Control SF_C_Frame;

u32 sendCount;

/**
key_pad_comm_def->keypad[72]解释index=17表示开机按钮
假如key_pad[2] = 0x3d；kbd_map_temp_reg=0x3d，值匹配，就是按下按键2
另外keypad中0-16对应kbd_map_temp_reg中低16位
另外keypad中17-32对应kbd_map_temp_reg中高16位
另外keypad中33-48对应kbd_map_temp_reg1中低16位
另外keypad中49-72对应kbd_map_temp_reg1中高16位
*/
u8 keypaddef[77] = {
    /*keypad*/
    0x12, 0x49, 0x0F, 0x1A,
    0x15, 0xFE, 0xFE, 0xFE,
    0x17, 0x10, 0x03, 0x02,
    0x01, 0x11, 0xFE, 0xFE,
    0xFE, 0x17, 0x1B, 0x06,
    0x05, 0x04, 0x0E, 0xFE,
    0xFE, 0xFE, 0x17, 0x7B,
    0x09, 0x08, 0x07, 0x14,
    0xFE, 0xFE, 0xFE, 0x17,
    0x7C, 0x0B, 0x00, 0x0A,
    0x16, 0xFE, 0xFE, 0xFE,
    0x17, 0xFE, 0xFE, 0xFE,
    0xFE, 0xFE, 0xFE, 0xFE,
    0xFE, 0x17, 0xFE, 0xFE,
    0xFE, 0xFE, 0xFE, 0xFE,
    0xFE, 0xFE, 0x17, 0xFE,
    0xFE, 0xFE, 0xFE, 0xFE,
    0xFE, 0xFE, 0xFE, 0x17,
    /* period */
    0xC4, 0x09, 0x00, 0x00,
    /* power_key_index */
    0x17};

int utf16_len(char *utf16)
{
    int len = 0;
    while (*utf16++ != 0)
        len++;
    return len;
}

int ucs2_to_utf8(const unsigned char *in, int ilen, unsigned char *out, int olen)
{
    int length = 0;
    if (!out)
        return length;
    char *start = NULL;
    char *pout = out;
    for (start = in; start != NULL && start < in + ilen - 1; start += 2)
    {
        unsigned short ucs2_code = *(unsigned short *)start;
        if (0x0080 > ucs2_code)
        {
            /* 1 byte UTF-8 Character.*/
            if (length + 1 > olen)
                return -1;

            *pout = (char)*start;
            length++;
            pout++;
        }
        else if (0x0800 > ucs2_code)
        {
            /*2 bytes UTF-8 Character.*/
            if (length + 2 > olen)
                return -1;
            *pout = ((char)(ucs2_code >> 6)) | 0xc0;
            *(pout + 1) = ((char)(ucs2_code & 0x003F)) | 0x80;
            length += 2;
            pout += 2;
        }
        else
        {
            /* 3 bytes UTF-8 Character .*/
            if (length + 3 > olen)
                return -1;

            *pout = ((char)(ucs2_code >> 12)) | 0xE0;
            *(pout + 1) = ((char)((ucs2_code & 0x0FC0) >> 6)) | 0x80;
            *(pout + 2) = ((char)(ucs2_code & 0x003F)) | 0x80;
            length += 3;
            pout += 3;
        }
    }

    return length;
}

void keyEvent(int type, int key)
{
    // printf("keyboard(%x)\n", key);
    if (key >= 0x30 && key <= 0x39)
    { // 数字键盘1-9
        simulateKey = key - 0x30;
    }
    else if (key == 0x77) // w
    {
        simulateKey = 14; // 上
    }
    else if (key == 0x73) // s
    {
        simulateKey = 15; // 下
    }
    else if (key == 0x61) // a
    {
        simulateKey = 16; // 左
    }
    else if (key == 0x64) // d
    {
        simulateKey = 17; // 右
    }

    else if (key == 0x66) // f
    {
        simulateKey = 18; // OK
    }
    else if (key == 0x71) // q
    {
        simulateKey = 20; // 左软
    }
    else if (key == 0x65) // e
    {
        simulateKey = 21; // 右软
    }
    else if (key == 0x7a) // z
    {
        simulateKey = 22; // 拨号
    }
    else if (key == 0x63) // c
    {
        simulateKey = 23; // 挂机
    }

    else if (key == 0x6e) // n
    {
        simulateKey = 10; // *
    }
    else if (key == 0x6d) // m
    {
        simulateKey = 11; // #
    }
    simulatePress = type == 4 ? 1 : 0;
}

void mouseEvent(int type, int data0, int data1)
{
    //    uc_mem_read(MTK,0x4000AD38, &changeTmp, 4);
    //    uc_mem_read(MTK,0x4000AD30, &changeTmp1, 4);
    //    printf("[SDL](TMD_STATE:%x,TMD_Time_Slice:%x)\n", changeTmp, changeTmp1);
    /*
        if (!hasISR)
        {
            isrStack[0] = 0xF01D283C;
            isrStack[1] = 1;
            isrStack[2] = 0;
            isrStack[4] = 0;
            ISR_Start_Address = 0x823f7b7;
            ISR_End_Address = 0x823F7D8;
            requireSetIsrStack = true;
            hasISR = true;
        }
        */
    // Update_RTC_Time();
    uc_mem_read(MTK, 0x4000ad10, &changeTmp1, 4);
    uc_mem_read(MTK, changeTmp1, &changeTmp2, 4);
    uc_mem_read(MTK, changeTmp1 + 20, &changeTmp1, 4);
    printf("AC_TM_L(%x)(%x)(%x)\n", changeTmp1, changeTmp2, changeTmp3);
    uc_mem_read(MTK, 0x4000ad28, &changeTmp2, 4);
    uc_mem_read(MTK, 0x4000ad2c, &changeTmp3, 4);
    printf("TMD_Timer_State(%x),TMD_Timer(%x)\n", changeTmp3, changeTmp2);
    debugType = 10;
    dumpCpuInfo();
}

// 更新RTC时钟寄存器
void Update_RTC_Time()
{
    changeTmp1 = 10;
    uc_mem_write(MTK, 0x810b0014, &changeTmp1, 4); // 秒
    changeTmp1 = 30;
    uc_mem_write(MTK, 0x810b0018, &changeTmp1, 4); // 分
    changeTmp1 = 12;
    uc_mem_write(MTK, 0x810B001C, &changeTmp1, 4); // 时
    changeTmp1 = 1;
    uc_mem_write(MTK, 0x810b0020, &changeTmp1, 4); // 日
    changeTmp1 = 0;
    uc_mem_write(MTK, 0x810b0024, &changeTmp1, 4); // 星期
    changeTmp1 = 1;
    uc_mem_write(MTK, 0x810b0028, &changeTmp1, 4); // 月
    changeTmp1 = 13;
    uc_mem_write(MTK, 0x810b002c, &changeTmp1, 4); // 年
}

void loop()
{
    SDL_Event ev;
    bool isLoop = true;
    while (isLoop)
    {
        while (SDL_WaitEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
            {
                isLoop = false;
                break;
            }
            switch (ev.type)
            {
            case SDL_KEYDOWN:
                if (isKeyDown == SDLK_UNKNOWN)
                {
                    isKeyDown = ev.key.keysym.sym;
                    keyEvent(MR_KEY_PRESS, ev.key.keysym.sym);
                }
                break;
            case SDL_KEYUP:
                if (isKeyDown == ev.key.keysym.sym)
                {
                    isKeyDown = SDLK_UNKNOWN;
                    keyEvent(MR_KEY_RELEASE, ev.key.keysym.sym);
                }
                break;
            case SDL_MOUSEMOTION:
                if (isMouseDown)
                {
                    mouseEvent(MR_MOUSE_MOVE, ev.motion.x, ev.motion.y);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                isMouseDown = true;
                mouseEvent(MR_MOUSE_DOWN, ev.motion.x, ev.motion.y);
                break;
            case SDL_MOUSEBUTTONUP:
                isMouseDown = false;
                mouseEvent(MR_MOUSE_UP, ev.motion.x, ev.motion.y);
                break;
            }
        }
    }
}

/**
 * 读取文件
 * 读取完成后需要释放
 */
u8 *readFile(const char *filename, u32 *size)
{
    FILE *file;
    u8 *tmp;
    long file_size;
    u8 flag;
    // 打开文件 a.txt
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Failed to open file:%s\n", filename);
        return NULL;
    }

    // 移动文件指针到文件末尾，获取文件大小
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    *size = file_size;
    rewind(file);
    // 为 tmp 分配内存
    tmp = (u8 *)malloc(file_size * sizeof(u8));
    if (tmp == NULL)
    {
        printf("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    // 读取文件内容到 tmp 中
    size_t result = fread(tmp, 1, file_size, file);
    if (result != file_size)
    {
        printf("Failed to read file");
        free(tmp);
        fclose(file);
        return NULL;
    }
    fclose(file);
    return tmp;
}

u8 *readSDFile(u32 startPos, u32 size)
{
    u8 *tmp;
    u8 flag;
    if (SD_File_Handle == NULL)
    {
        return NULL;
    }
    // 为 tmp 分配内存
    tmp = (u8 *)malloc(size);
    if (tmp == NULL)
    {
        printf("申请文件内存失败");
        return NULL;
    }
    if (fseek(SD_File_Handle, startPos, SEEK_SET) > 0)
    {
        printf("移动文件指针失败");
        return NULL;
    }
    // 读取文件内容到 tmp 中
    size_t result = fread(tmp, 1, size, SD_File_Handle);
    if (result != size)
    {
        printf("读取SD卡文件失败\n");
        free(tmp);
        return NULL;
    }
    return tmp;
}

bool writeSDFile(u8 *Buffer, u32 startPos, u32 size)
{
    u8 flag;
    if (SD_File_Handle == NULL)
    {
        return false;
    }
    if (fseek(SD_File_Handle, startPos, SEEK_SET) > 0)
    {
        printf("移动文件指针失败\n");
        return false;
    }
    size_t result = fwrite(Buffer, 1, size, SD_File_Handle);
    if (result != size)
    {
        printf("写入文件失败\n");
        return false;
    }
    return true;
}

void *readFlashFile(u32 startPos, u32 size)
{
    u8 *tmp;
    u8 flag;
    if (FLASH_File_Handle == NULL)
    {
        return NULL;
    }
    // 为 tmp 分配内存
    tmp = (u8 *)malloc(size);
    if (tmp == NULL)
    {
        printf("申请文件内存失败");
        return NULL;
    }
    if (fseek(FLASH_File_Handle, startPos, SEEK_SET) > 0)
    {
        printf("移动文件指针失败");
        return NULL;
    }
    // 读取文件内容到 tmp 中
    size_t result = fread(tmp, 1, size, SD_File_Handle);
    if (result != size)
    {
        printf("读取Flash文件失败\n");
        free(tmp);
        return NULL;
    }
    return tmp;
}

bool writeFlashFile(u8 *Buffer, u32 startPos, u32 size)
{
    u8 flag;
    if (FLASH_File_Handle == NULL)
    {
        return false;
    }
    if (fseek(FLASH_File_Handle, startPos, SEEK_SET) > 0)
    {
        printf("移动文件指针失败\n");
        return false;
    }
    size_t result = fwrite(Buffer, 1, size, FLASH_File_Handle);
    if (result != size)
    {
        printf("写入文件失败\n");
        return false;
    }
    return true;
}
/**
 * 初始化模拟CPU引擎与内存
 *
 */
void initMtkSimalator()
{
    uc_err err;
    uc_hook trace;
    err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &MTK);
    if (err)
    {
        printf("Failed on uc_open() with error returned: %u (%s)\n", err, uc_strerror(err));
        return NULL;
    }

    ROM_MEMPOOL = malloc(size_16mb);
    RAM_MEMPOOL = malloc(size_8mb);

    // 映射寄存器
    err = uc_mem_map_ptr(MTK, 0x80000000, size_8mb, UC_PROT_ALL, malloc(size_8mb));
    // GPIO_BASE_ADDRESS
    err = uc_mem_map_ptr(MTK, 0x81000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    err = uc_mem_map_ptr(MTK, 0x82000000, size_4mb, UC_PROT_ALL, malloc(size_4mb));
    err = uc_mem_map_ptr(MTK, 0x83000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    err = uc_mem_map_ptr(MTK, 0x84000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    err = uc_mem_map_ptr(MTK, 0x85000000, size_8mb, UC_PROT_ALL, malloc(size_8mb));
    // 未知 分区
    err = uc_mem_map_ptr(MTK, 0x70000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    err = uc_mem_map_ptr(MTK, 0x78000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    err = uc_mem_map_ptr(MTK, 0x90000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    err = uc_mem_map_ptr(MTK, 0xA0000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    err = uc_mem_map_ptr(MTK, 0xA1000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    err = uc_mem_map_ptr(MTK, 0xA2000000, size_8mb, UC_PROT_ALL, malloc(size_8mb));
    err = uc_mem_map_ptr(MTK, 0xA3000000, size_8mb, UC_PROT_ALL, malloc(size_8mb));
    err = uc_mem_map_ptr(MTK, 0xE5900000, size_8mb, UC_PROT_ALL, malloc(size_8mb));
    RAMF0_POOL = malloc(size_16mb);
    err = uc_mem_map_ptr(MTK, 0xF0000000, size_16mb, UC_PROT_ALL, RAMF0_POOL);
    err = uc_mem_map_ptr(MTK, 0x01FFF000, size_1mb, UC_PROT_ALL, malloc(size_1mb));
    // 映射ROM
    err = uc_mem_map_ptr(MTK, 0x08000000, size_16mb, UC_PROT_ALL, ROM_MEMPOOL);
    // 中断栈
    err = uc_mem_map_ptr(MTK, 0x50000000, size_1mb, UC_PROT_ALL, malloc(size_1mb));

    if (err)
    {
        printf("Failed mem  Rom map: %u (%s)\n", err, uc_strerror(err));
        return NULL;
    }
    // 映射RAM
    err = uc_mem_map_ptr(MTK, 0, size_8mb, UC_PROT_ALL, RAM_MEMPOOL);

    // 映射外部INIT_SRAM
    RAM40_POOL = malloc(size_8mb);
    err = uc_mem_map_ptr(MTK, 0x40000000, size_8mb, UC_PROT_ALL, RAM40_POOL);
    if (err)
    {
        printf("Failed mem map: %u (%s)\n", err, uc_strerror(err));
        return NULL;
    }
    // hook kal_fatal_error_handler
    // err = uc_hook_add(uc, &trace, UC_HOOK_CODE, hookCodeCallBack, 0, 0, 0xFFFFFFFF);
    err = uc_hook_add(MTK, &trace, UC_HOOK_BLOCK, hookBlockCallBack, 1, 0x8363840, 0x8363858);
    err = uc_hook_add(MTK, &trace, UC_HOOK_BLOCK, hookBlockCallBack, 2, 0x82acba8, 0x82acbab);
    err = uc_hook_add(MTK, &trace, UC_HOOK_BLOCK, hookBlockCallBack, 3, 0x82ac688, 0x82ac68b);
    err = uc_hook_add(MTK, &trace, UC_HOOK_BLOCK, hookBlockCallBack, 4, 0x50000000, 0x50000004);
    err = uc_hook_add(MTK, &trace, UC_HOOK_BLOCK, hookBlockCallBack, 5, 0x50000008, 0x5000000b);
    err = uc_hook_add(MTK, &trace, UC_HOOK_BLOCK, hookBlockCallBack, 6, 0x82D2A22, 0x82D2A24);
    err = uc_hook_add(MTK, &trace, UC_HOOK_BLOCK, hookBlockCallBack, 7, 0x8239244, 0x8239248);

    err = uc_hook_add(MTK, &trace, UC_HOOK_CODE, hookCodeCallBack, 0, 0, 0xF4FFFFFF);
    err = uc_hook_add(MTK, &trace, UC_HOOK_MEM_READ, hookRamCallBack, 0, 0x80000000, 0x82000000);
    err = uc_hook_add(MTK, &trace, UC_HOOK_MEM_READ, hookRamCallBack, 0, 0x82050000, 0x83000000);
    err = uc_hook_add(MTK, &trace, UC_HOOK_MEM_READ, hookRamCallBack, 0, 0x90000000, 0xF0000000);
    err = uc_hook_add(MTK, &trace, UC_HOOK_MEM_WRITE, hookRamCallBack, 1, 0x81000000, 0xF0000000);
    if (err != UC_ERR_OK)
    {
        printf("add hook err %u (%s)\n", err, uc_strerror(err));
        return NULL;
    }
    /*
    err = uc_hook_add(uc, &trace, UC_HOOK_MEM_READ_UNMAPPED, hookRamCallBack, 2, 0, 0xFFFFFFFF);
    err = uc_hook_add(uc, &trace, UC_HOOK_MEM_WRITE_UNMAPPED, hookRamCallBack, 3, 0, 0xFFFFFFFF);
    err = uc_hook_add(uc, &trace, UC_HOOK_MEM_FETCH_UNMAPPED, hookRamCallBack, 4, 0, 0xFFFFFFFF);
    */

    if (err != UC_ERR_OK)
    {
        printf("add hook err %u (%s)\n", err, uc_strerror(err));
        return NULL;
    }
}

void dumpCpuInfo()
{
    u32 r0 = 0;
    u32 r1 = 0;
    u32 r2 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 msp = 0;
    u32 pc = 0;
    u32 lr = 0;
    u32 cpsr = 0;
    uc_reg_read(MTK, UC_ARM_REG_PC, &pc);
    uc_reg_read(MTK, UC_ARM_REG_SP, &msp);
    uc_reg_read(MTK, UC_ARM_REG_CPSR, &cpsr);
    uc_reg_read(MTK, UC_ARM_REG_LR, &lr);
    uc_reg_read(MTK, UC_ARM_REG_R0, &r0);
    uc_reg_read(MTK, UC_ARM_REG_R1, &r1);
    uc_reg_read(MTK, UC_ARM_REG_R2, &r2);
    uc_reg_read(MTK, UC_ARM_REG_R3, &r3);
    uc_reg_read(MTK, UC_ARM_REG_R4, &r4);
    printf("r0:%x r1:%x r2:%x r3:%x r4:%x r5:%x r6:%x r7:%x r8:%x r9:%x\n", r0, r1, r2, r3, r4);
    printf("msp:%x cpsr:%x(thumb:%x)(mode:%x) lr:%x pc:%x lastPc:%x irq_c(%x)\n", msp, cpsr, (cpsr & 0x20) > 0, cpsr & 0x1f, lr, pc, lastAddress, irq_nested_count);
    printf("------------\n");
}

void RunArmProgram(void *startAddr)
{

    u32 startAddress = (u32)startAddr;
    uc_err p;
    // 启动前工作

    // 过寄存器检测
    changeTmp = 2;
    uc_mem_write(MTK, 0x81060010, &changeTmp, 2);
    // 过方法sub_80017C0
    changeTmp = 2;
    uc_mem_write(MTK, 0x8200021C, &changeTmp, 4);
    changeTmp = 0x3FFFFFF << 16;
    uc_mem_write(MTK, 0x82000224, &changeTmp, 4);
    changeTmp = 660;
    uc_mem_write(MTK, 0x82000228, &changeTmp, 4);

    // 模拟按键开机启动
    SimulatePressKey(0x17, 1);
    changeTmp1 = 0x26409001; // 开启memory dump
    uc_mem_write(MTK, 0xF016AD20, &changeTmp1, 4);
    // SRAM开始的320字节(0x140)内存要保留，用于异常处理，异常处理代码在reset后从flash复制。
    // 跳过最开始的地址映射部分
    changeTmp = 3;
    uc_mem_write(MTK, 0x81000040, &changeTmp, 4);
    // 过sub_819E8EC方法
    u32 unk_data = 0x20;
    uc_mem_write(MTK, UART_LINE_STATUS_REG, &unk_data, 4);
    // 过sub_8000D9C方法，这里貌似也可以直接跳过
    unk_data = 25168;
    uc_mem_write(MTK, 0x80010008, &unk_data, 4);
    // 过sub_8703796方法
    unk_data = 2;
    uc_mem_write(MTK, 0x08000AD4, &unk_data, 2);
    // 还原Flash数据
    int flashDataSize = 0;
    // if (readFile("Rom\\flash.lock", &flashDataSize) && flashDataSize > 0)
    // {
    //    char *flashDataTmp = readFile(FLASH_IMG_PATH, &flashDataSize);
    //    uc_mem_write(MTK,0x8780000, flashDataTmp, size_4mb);
    // }
    p = uc_emu_start(MTK, startAddress, startAddress + 1, 0, 0);
    // p = uc_emu_start(MTK, 3, 8, 0, 0);

    if (p == UC_ERR_READ_UNMAPPED)
        printf("模拟错误：此处内存不可读\n", p);
    else if (p == UC_ERR_WRITE_UNMAPPED)
        printf("模拟错误：此处内存不可写\n");
    else if (p == UC_ERR_FETCH_UNMAPPED)
        printf("模拟错误：此处内存不可执行\n");
    else if (p != UC_ERR_OK)
        printf("模拟错误：(未处理)%s\n", uc_strerror(p));
    dumpCpuInfo();
}

void *ThreadRun(void *p)
{
    RunArmProgram(p);
}

void ScreenRenderThread()
{
    while (1)
    {
        currentTime = clock();
        renderGdiBufferToWindow();
        if (currentTime > lastFlashTime)
        {
            lastFlashTime = currentTime + 100;
            fflush(stdout);
        }
        usleep(1000);
    }
}

int main(int argc, char *args[])
{
    SetConsoleOutputCP(CP_UTF8);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    window = SDL_CreateWindow("IHD316(MTK6252) Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    initMtkSimalator();

    if (MTK != NULL)
    {
        u32 size = 0;
        u8 *tmp = 0;

        tmp = readFile(ROM_PROGRAM_BIN, &size);
        uc_mem_write(MTK, 0x08000000, tmp, size);
        free(tmp);
        uc_context_alloc(MTK, &callback_context);
        uc_context_alloc(MTK, &timer_isr_context);

        SD_File_Handle = fopen(SD_CARD_IMG_PATH, "r+b");
        if (SD_File_Handle == NULL)
            printf("没有SD卡镜像文件，跳过加载\n");
        /*
    FLASH_File_Handle = fopen(FLASH_IMG_PATH, "r+b");
    if (FLASH_File_Handle == NULL)
        printf("没有Flash数据文件，跳过加载");
    else
    {
        u8 *tmp2 = readFlashFile(0, 1);
        if (*tmp2 == 0)
        { // 如果没有初始化，从rom中读取一次数据
            u8 *tmp3 = malloc(size_1mb);
            uc_mem_read(MTK,0x8780000, tmp3, size_1mb);
            writeFlashFile(tmp3, 0, size_1mb);
            free(tmp3);
        }
        else
        {
            // 读取数据到缓存
            u8 *tmp = readFlashFile(0, size_1mb);
            uc_mem_write(MTK,0x8780000, tmp, size_1mb);
            free(tmp);
        }
        free(tmp2);
    }*/
        // 禁用缓冲自动刷新
        setvbuf(stdout, NULL, _IOFBF, 10240); // 设置缓冲区大小为 10k 字节
        // 启动emu线程
        pthread_create(&emu_thread, NULL, ThreadRun, 0x8000000);
        pthread_create(&screen_render_thread, NULL, ScreenRenderThread, 0);
        printf("Unicorn Engine Initialized\n");
    }
    loop();
    if (SD_File_Handle != NULL)
        fclose(SD_File_Handle);
    if (FLASH_File_Handle != NULL)
        fclose(FLASH_File_Handle);
    return 0;
}

void renderGdiBufferToWindow()
{
    // 获取窗口的表面
    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    u8 *buffPtr;
    u8 *realPtr;
    u8 li;
    u32 pz = 0;
    u16 color;
    u8 r;
    u8 g;
    u8 b;
    if (lcdUpdateFlag)
    {
        for (li = 0; li < 4; li++)
        {
            pz = LCD_Layer_Address[li];
            if (pz > 0)
            {
                realPtr = getRealMemPtr(pz);
                for (u16 i = 0; i < 320; i++)
                {
                    for (u16 j = 0; j < 240; j++)
                    {
                        pz = (j + i * 240);
                        color = *((u16 *)realPtr + pz);
                        // 不是透明的，覆盖上个图层颜色
                        if (color != 0x1f)
                            *((Uint32 *)screenSurface->pixels + pz) = SDL_MapRGB(screenSurface->format, PIXEL565R(color), PIXEL565G(color), PIXEL565B(color));
                    }
                }
            }
        }

        // 更新窗口
        SDL_UpdateWindowSurface(window);

        lcdUpdateFlag = false;
    }
}
void hookBlockCallBack(uc_engine *uc, uint64_t address, uint32_t size, void *user_data)
{
    u32 data = (u32)user_data;
    switch (data)
    {
    case 1:
    { // 默认sim卡初始化完毕
        changeTmp1 = 1;
        uc_mem_write(MTK, 0xf029eeb5, &changeTmp1, 1);
    }
    break;
    case 2:
    { // MSDC_DMATransferFirst
        uc_reg_read(MTK, UC_ARM_REG_R0, &DMA_Data_Transfer_Ptr);
        uc_reg_read(MTK, UC_ARM_REG_R1, &DMA_Transfer_Bytes_Count);
        uc_reg_read(MTK, UC_ARM_REG_R2, &SD_DMA_Type);
        DMA_Transfer_Bytes_Count *= 4;
        // printf("MSDC_DMATransferFirst(%x,%x,%x)\n", DMA_Data_Transfer_Ptr, DMA_Transfer_Bytes_Count, SD_DMA_Type);
        //  如果是已经缓存的，把目标地址换为0xd820
        uc_mem_read(MTK, 0xF01D28d2, &changeTmp1, 2);
        if (changeTmp1 & 1)
        {
            DMA_Data_Transfer_Ptr = 0xd820;
        }
    }
    break;
    case 3:
        // MSDC_DMATransferFinal
        if (DMA_Data_Transfer_Ptr > 0)
        {
            if (SD_DMA_Type == SD_DMA_Read)
            {
                dataCachePtr = readSDFile(SD_READ_ADDR, DMA_Transfer_Bytes_Count);
                if (dataCachePtr != NULL)
                {
                    uc_mem_write(MTK, DMA_Data_Transfer_Ptr, dataCachePtr, DMA_Transfer_Bytes_Count);
                    // printf("read fat32.img(a:%x,p:%x,bc:%x)\n", SD_READ_ADDR, DMA_Data_Transfer_Ptr, DMA_Transfer_Bytes_Count);
                    free(dataCachePtr);
                }
            }
            else
            {
                // printf("write fat32.img(%x,%x,%x)\n", SD_Write_ADDR, DMA_Data_Transfer_Ptr, DMA_Transfer_Bytes_Count);
                u8 *cache = malloc(DMA_Transfer_Bytes_Count);
                if (cache != NULL)
                {
                    uc_mem_read(MTK, DMA_Data_Transfer_Ptr, cache, DMA_Transfer_Bytes_Count);
                    writeSDFile(cache, SD_Write_ADDR, DMA_Transfer_Bytes_Count);
                    free(cache);
                }
            }
            DMA_Data_Transfer_Ptr = 0;
        }
        break;
    case 4: // 中断恢复
        RestoreCpuContext(&isrStackList[--irq_nested_count]);
        break;
    case 5: // 回调恢复
        RestoreCpuContext(&stackCallback);
        break;
    case 6: // mr_sprintf
        uc_mem_read(MTK, 0xF028EDC4, &globalSprintfBuff, 128);
        printf("mr_sprintf(%s)(%x)\n", globalSprintfBuff, lastAddress);
        break;

    default:
        break;
    }
}

void hookRamCallBack(uc_engine *uc, uc_mem_type type, uint64_t address, uint32_t size, int64_t value, u32 data)
{
    // 合并图像时，层数越小越底层，层数越大越顶层
    switch (address)
    {
    case 0x82050000: // 写1变0
        changeTmp = 0;
        uc_mem_write(MTK, (u32)address, &changeTmp, 2);
        break;
    case 0xa0000000:
        // 0x8094064 0x809404a过sub_8094040(L1层的)
        changeTmp = 0x5555;
        uc_mem_write(MTK, (u32)address, &changeTmp, 4);
        break;
    /*
// 解决间隔短抛出的assert错误的两个关键变量L1_init有初始化
case 0xF01DC4FC:
    if (data == 1)
    {
        // todo
    }
    break;*/
    case 0x9000000c: // LCD Interface Frame Transfer Register
        if (value == 0 && data == 1)
        {
            // 需要更新显示
            lcdUpdateFlag = true;
        }
        break;
    case 0x9000014c: // Layer 3 Address
        if (data == 1)
        {
            LCD_Layer_Address[3] = value;
        }
        break;
    case 0x9000011c: // Layer 2 Address
        if (data == 1)
        {
            LCD_Layer_Address[2] = value;
        }
        break;
    case 0x900000ec: // Layer 1 Address
        if (data == 1)
        {
            LCD_Layer_Address[1] = value;
        }
        break;
    case 0x900000bc: // Layer 0 Address
        if (data == 1)
        {
            LCD_Layer_Address[0] = value;
        }
        break;

    case RW_SFI_OUTPUT_LEN_REG: // 要写入的数据长度
        if (data == 1)
        {
            SF_C_Frame.sendDataCount = value;
            if (SF_C_Frame.InputData != NULL)
            {
                free(SF_C_Frame.InputData);
                SF_C_Frame.InputData = NULL;
            }
            SF_C_Frame.InputData = malloc(value);
        }

        break;
    case RW_SFI_INPUT_LEN_REG: // 要读取的数据长度
        if (data == 1)
        {
            SF_C_Frame.readDataCount = value;
            if (SF_C_Frame.SendData != NULL)
            {
                free(SF_C_Frame.SendData);
                SF_C_Frame.SendData = NULL;
            }
            SF_C_Frame.SendData = malloc(value);
        }

        break;
    case RW_SFI_GPRAM_CMD_REG: // FLash数据寄存器
    {
        if (data == 1) // 写入数据
        {
            SF_C_Frame.cmd = value & 0xff;
            SF_C_Frame.address = (value >> 24) | (((value >> 16) & 0xff) << 8) | (((value >> 8) & 0xff) << 16); // 分别是原前8位，中8位，高8位
            // printf("RW_SFI_GPRAM_DATA = (cmd:%x,address:%x)\n", SF_C_Frame.cmd, SF_C_Frame.address);
        }
        else // 读取数据
        {
            switch (SF_C_Frame.cmd)
            {
            case 0x9f: // 读取Flash芯片信息
                uc_mem_write(MTK, RW_SFI_GPRAM_CMD_REG, &(SF_C_Frame.SendData[SF_C_Frame.sentCount]), 4);
                SF_C_Frame.sentCount += 4;
                SF_C_Frame.sendDataCount -= 4;
                if (SF_C_Frame.sendDataCount == 0) // 数据发送完成
                {
                    changeTmp = 2;
                    uc_mem_write(MTK, RW_SFI_MAC_CTL, &changeTmp, 4);
                    SF_C_Frame.sendDataMode = false;
                    free(SF_C_Frame.SendData);
                    SF_C_Frame.SendData = NULL;
                }
                break;
            default:
                // printf("未处理的SPI FLASH命令(%x)\n", SF_C_Frame.cmd);
                break;
            }
        }
        break;
    }

    case RW_SFI_MAC_CTL: // Flash控制寄存器
    {
        // dumpCpuInfo();
        if (data == 1)
        {
            if (value & 4)
            {
                SF_C_Frame.cmdRev = 1;
            }
        }
        else
        {
            if (SF_C_Frame.cmdRev)
            {
                // printf("Exec SFI Cmd[0x%x][0x%d]\n", SF_C_Frame.cmd,SF_C_Frame.address);
                //  触发命令发送和启用宏模式
                switch (SF_C_Frame.cmd)
                {
                case 0x2: // 写一页数据
                          // 计算页地址，并擦除所在页
                    // changeTmp1 = (SF_C_Frame.address / 256) * 256;
                    sendCount = SF_C_Frame.sendDataCount - 4; // 减去1cmd 3addr就是实际写入长度
                    changeTmp = 0x8000000 | SF_C_Frame.address;
                    // printf("flash addr::%x\n", SF_C_Frame.address);
                    //  地址4字节对齐
                    uc_mem_write(MTK, changeTmp, &(SF_C_Frame.cacheData), sendCount);
                    // writeFlashFile(&SF_C_Frame.cacheData, SF_C_Frame.address - 0x780000, sendCount);
                    SF_C_Frame.cmdRev = 0;
                    // printf("Write Flash Address[%x] Value[%x] Size[%d]\n", SF_C_Frame.address, SF_C_Frame.cacheData[0], sendCount);
                    break;
                case 0x5:          // 读状态寄存器
                    changeTmp = 0; // 表示不忙
                    uc_mem_write(MTK, RW_SFI_GPRAM_CMD_REG, &changeTmp, 4);
                    break;
                case 0x1:  // 写状态寄存器
                case 0x6:  // 允许写入
                case 0xb9: // 切换到正常模式
                case 0xaf: // 切换到深度掉电模式
                case 0x50: // 未知
                case 0x38: // 未知
                    if (SF_C_Frame.cmdRev == 1)
                    {
                        SF_C_Frame.cmdRev = 2;
                    }
                    else
                    {
                        changeTmp = 2;
                        SF_C_Frame.cmdRev = 0;
                        uc_mem_write(MTK, RW_SFI_MAC_CTL, &changeTmp, 4);
                    }
                    break;
                case 0x9f: // 读取三字节Flash ID信息
                    SF_C_Frame.sendDataMode = true;
                    SF_C_Frame.SendData[0] = 1;
                    SF_C_Frame.SendData[1] = 2;
                    SF_C_Frame.SendData[2] = 3;
                    SF_C_Frame.cmdRev = 0;
                    break;
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0xA10003F6:
    {
        if (data == 0)
        {
            changeTmp = 0x8888;
            uc_mem_write(MTK, (u32)address, &changeTmp, 2);
        }
        break;
    }
    case 0x81020000: // 跟MSDC有关的
    {
        changeTmp = 0x8000;
        uc_mem_write(MTK, (u32)address, &changeTmp, 4);
        break;
    }
    case SDC_DATSTA_REG:
    {
        changeTmp = 0x8000;
        uc_mem_write(MTK, (u32)address, &changeTmp, 4);
        break;
    }
    case 0x810C0090: // 读寄存器，返回0x10过sub_8122d8c的while
    {
        if (data == 0)
        {
            changeTmp = 0x10;
            uc_mem_write(MTK, (u32)address, &changeTmp, 4);
        }
        break;
    }
    case SD_CMD_STAT_REG: // 读取SD 命令状态寄存器
    {
        changeTmp = 1;
        uc_mem_write(MTK, (u32)address, &changeTmp, 4); // 写1表示命令回复成功 2超时 4crc校验错误
        break;
    }
    case SD_DATA_RESP_REG0:
    { // SD 命令响应数据寄存器 r0,r1,r2,r3每个寄存器占用4字节
        switch (SDCMD_CACHE)
        {
        case SDC_CMD_CMD0: // 进入SPI模式
            // printf("SD卡 进入SPI模式(%x)\n", SEND_SDDATA_CACHE);
            break;
        case SDC_CMD_CMD1:
            break;
        case SDC_CMD_CMD2: // 用于请求 SD 卡返回 CID (Card Identification Number)数据(128位响应)
            // printf("SD卡 获取CID寄存器(%x)\n", SEND_SDDATA_CACHE);
            changeTmp1 = 0xF016C1C4;
            uc_mem_write(MTK, SD_DATA_RESP_REG0, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD7: // 用于选择或取消选择一张 SD 卡
            // printf("取消或选择SD卡(%x)\n", SEND_SDDATA_CACHE);
            break;
        case SDC_CMD_CMD8: // 询问SD卡的版本号和电压范围
            changeTmp1 = 0x1aa;
            uc_mem_write(MTK, SD_DATA_RESP_REG0, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD9: // 获取SD卡的CSD寄存器（Card-Specific Data Register）(128位响应)
            // printf("SD卡 获取CSD寄存器(%x)\n", SEND_SDDATA_CACHE);
            //  changeTmp1 = 0x400E0032;//原始数据
            changeTmp1 = 0x0000e004; // int*转换到char*
            uc_mem_write(MTK, SD_DATA_RESP_REG0, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD55: // 用于通知SD卡，下一个命令将是应用命令（ACMD）
            // printf("SD卡ACMD模式开启(%x)\n", SEND_SDDATA_CACHE);
            changeTmp1 = 0x20;
            uc_mem_write(MTK, SD_DATA_RESP_REG0, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD41_SD: // 初始化SD命令
            // printf("初始化SD卡\n");
            //  bit 31 = 1：卡已经准备好，可以进行后续操作。
            //  bit 30 = 0：该卡为标准容量卡 SDSC，不是 SDHC/SDXC 高容量卡。
            //  bit 23-15 = 0xFF：卡支持的电压范围是 2.7V到3.6V。
            changeTmp1 = 0x80FF8000; // 普通容量SD卡
            uc_mem_write(MTK, SD_DATA_RESP_REG0, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD3_SD: // SEND_RELATIVE_ADDR (RCA)在 SD 卡的初始化过程中为卡分配一个相对地址
            // printf("SD卡 分配相对地址(%x)\n", SEND_SDDATA_CACHE);
            changeTmp1 = 0x3001;
            uc_mem_write(MTK, SD_DATA_RESP_REG0, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD12: // 结束连续多数据块传输
            // printf("结束SD卡连续读\n");
            break;
        case SDC_CMD_CMD13: // 查询 SD 卡的状态，并返回卡的当前状态信息
            // printf("SD卡 查询SD卡状态(%x)\n", SEND_SDDATA_CACHE);
            // 0x100 = R1_READY_FOR_DATA_8
            changeTmp1 = 0x100;
            uc_mem_write(MTK, SD_DATA_RESP_REG0, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD16: // 该命令用于设置数据块的长度
            // printf("SD卡 设置SD数据块长度(%x)\n", SEND_SDDATA_CACHE);
            // DMA_Transfer_Bytes_Count = SEND_SDDATA_CACHE;
            break;
        case SDC_CMD_CMD17: // 它的作用是从 SD 卡中读取一个单独的数据块
            SD_READ_ADDR = SEND_SDDATA_CACHE;
            // printf("SD Ready Read:(0x%x)\n", SEND_SDDATA_CACHE);
            //  printf("调用地址(%x)\n", lastAddress);
            break;
        case SDC_CMD_CMD18:
            SD_READ_ADDR = SEND_SDDATA_CACHE;
            // DMA_Transfer_Bytes_Count = SD_Multi_Read_Count;
            // printf("SD Ready Multi-Read:(0x%x)\n", SEND_SDDATA_CACHE);
            break;
        case SDC_CMD_CMD24:
            SD_Write_ADDR = SEND_SDDATA_CACHE;
            // printf("SD Ready Write:(0x%x)\n", SEND_SDDATA_CACHE);
            break;
        case SDC_CMD_CMD25:
            SD_Write_ADDR = SEND_SDDATA_CACHE;
            // SD_Read_Count = SD_Multi_Read_Count;
            // printf("SD Ready Multi-Write:(%x)\n", SEND_SDDATA_CACHE);
            break;
        case SDC_CMD_ACMD42: // 卡检测信号通常用于检测 SD 卡是否插入或取出
            // printf("SD卡 检查是否插入或取出(%x)\n", SEND_SDDATA_CACHE);
            break;
        case SDC_CMD_ACMD51: // 请求 SD 卡返回其 SCR (SD Card Configuration Register)寄存器
            // printf("SD卡 读取SCR寄存器(%x)\n", SEND_SDCMD_CACHE);
            break;
        default:
            // printf("未处理SD_DATA_RESP_REG_0(%x,CMD:%x)", SEND_SDDATA_CACHE, SEND_SDCMD_CACHE);
            // printf("(%x)\n", lastAddress);
            break;
        }
        break;
    }
    case SD_DATA_RESP_REG1:
    {
        switch (SDCMD_CACHE)
        {
        case SDC_CMD_CMD2: // 返回CID寄存器
            changeTmp1 = 0x77;
            uc_mem_write(MTK, SD_DATA_RESP_REG1, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD9: // 返回CSD寄存器
            // changeTmp1 = 0x77590000;
            changeTmp1 = 0x000ff577; // int*转换到char*
            uc_mem_write(MTK, SD_DATA_RESP_REG1, &changeTmp1, 4);
            break;
        default:
            // printf("未处理SD_DATA_RESP_REG_1(CMD:%x)", SEND_SDCMD_CACHE);
            // printf("(%x)\n", lastAddress);
            break;
        }
        break;
    }
    case SD_DATA_RESP_REG2:
    {
        switch (SDCMD_CACHE)
        {
        case SDC_CMD_CMD2: // 返回CID寄存器
            changeTmp1 = 0;
            uc_mem_write(MTK, SD_DATA_RESP_REG2, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD9: // 返回CSD寄存器
            // changeTmp1 = 0x7FF09000;
            changeTmp1 = 0x00090ff7; // int*转换到char*
            uc_mem_write(MTK, SD_DATA_RESP_REG2, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD17: //?
            break;
        default:
            // printf("未处理SD_DATA_RESP_REG_2(CMD:%x)", SEND_SDCMD_CACHE);
            // printf("(%x)\n", lastAddress);
            break;
        }
        break;
    }
    case SD_DATA_RESP_REG3:
    {

        switch (SDCMD_CACHE)
        {
        case SDC_CMD_CMD2: // 返回CID寄存器
            changeTmp1 = 0x3;
            uc_mem_write(MTK, SD_DATA_RESP_REG3, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD9: // 返回CSD寄存器
            // changeTmp1 = 0x0A400000;
            changeTmp1 = 0x000004a0; // int*转换到char*
            uc_mem_write(MTK, SD_DATA_RESP_REG3, &changeTmp1, 4);
            break;
        case SDC_CMD_ACMD51: // 读取SCR寄存器
            changeTmp1 = 0;
            uc_mem_write(MTK, SD_DATA_RESP_REG3, &changeTmp1, 4);
            break;
        default:
            // printf("未处理SD_DATA_RESP_REG_3(CMD:%x)", SEND_SDCMD_CACHE);
            //  printf("(%x)\n", lastAddress);
            break;
        }
        break;
    }
    case SD_CMD_RESP_REG0:
    {
        switch (SDCMD_CACHE)
        {
        case 0:
            break;
        case SDC_CMD_CMD2: // CID响应
            changeTmp1 = 0xF016C1C4;
            uc_mem_write(MTK, SD_CMD_RESP_REG0, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD7:
            break;
        case SDC_CMD_CMD13: //?
            break;
        case SDC_CMD_CMD16: //?
            break;
        case SDC_CMD_CMD17: //?
            break;
        case SDC_CMD_CMD18: //?
            break;
        case SDC_CMD_CMD24: //?
            break;
        case SDC_CMD_ACMD42:
            break;
        default:
            // printf("未处理SD_DATA_RESP_REG_0(ACMD:%x)", SEND_SDCMD_CACHE);
            //  printf("(%x)\n", lastAddress);
            break;
        }
        break;
    }
    case SD_CMD_RESP_REG1:
    {
        switch (SDCMD_CACHE)
        {
        case SDC_CMD_CMD2:
            changeTmp1 = 0x77;
            uc_mem_write(MTK, SD_CMD_RESP_REG1, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD13:
            break;
        case SDC_CMD_CMD16:
            break;
        case SDC_CMD_CMD17:
            break;
        case SDC_CMD_CMD18:
            break;
        case SDC_CMD_CMD55:
            break;
        case SDC_CMD_ACMD51:
            break;
        default:
            changeTmp = 0;
            uc_mem_write(MTK, SD_CMD_RESP_REG1, &changeTmp, 4);
            //  printf("未处理SD_DATA_RESP_REG_1(ACMD:%x)", SEND_SDCMD_CACHE);
            //  printf("(%x)\n", lastAddress);
            break;
        }
        break;
    }
    case SD_CMD_RESP_REG2:
    {
        switch (SDCMD_CACHE)
        {
        case 0:
            break;
        case SDC_CMD_CMD2:
            changeTmp1 = 0;
            uc_mem_write(MTK, SD_CMD_RESP_REG2, &changeTmp1, 4);
            break;
        case SDC_CMD_CMD3_SD:
            break;
        case SDC_CMD_CMD7:
            break;
        case SDC_CMD_CMD8:
            break;
        case SDC_CMD_CMD9:
            break;
        case SDC_CMD_CMD12:
            break;
        case SDC_CMD_CMD13: // 查询 SD 卡的状态，并返回卡的当前状态信息
            break;
        case SDC_CMD_CMD17:
            break;
        case SDC_CMD_CMD18:
            break;
        case SDC_CMD_CMD24:
            break;
        case 0x90:
            break;
        case SDC_CMD_CMD55:
            break;
        case SDC_CMD_ACMD51:
            break;
        case 0x40000000:
            break;
        default:
            // printf("未处理SD_DATA_RESP_REG_2(ACMD:%x)", SEND_SDCMD_CACHE);
            //  printf("(%x)\n", lastAddress);
            break;
        }
        break;
    }
    case SD_CMD_RESP_REG3:
    {
        switch (SDCMD_CACHE)
        {
        case SDC_CMD_CMD2:
            changeTmp1 = 0x3;
            uc_mem_write(MTK, SD_CMD_RESP_REG3, &changeTmp1, 4);
            break;
        case 0x8b3:
            break;
        default:
            // printf("未处理SD_DATA_RESP_REG_3(ACMD:%x)", SEND_SDCMD_CACHE);
            // printf("(%x)\n", lastAddress);
            break;
        }
        break;
    }

    case SD_ARG_REG: // 读取SD 参数寄存器
    {
        if (data == 1)
        {
            SEND_SDDATA_CACHE = value;
        }
        break;
    }
    case SD_CMD_REG: // SD 命令寄存器
    {
        if (data == 1)
        {
            SEND_SDCMD_CACHE = value;
            // 取0x40000000后16位
            SDCMD_CACHE = value & 0xffff;
        }
        break;
    }
    case 0xF015E327:
    {
        if (value == 0 && data == 1)
        {
            confirm("warning", "sd filesystem mount fail");
        }
        break;
    }
        /*
        case IRQ_EOIL: // 置1表示中断正在处理中，置0表示处理完成
            if (data == 1)
                printf("中断结束(%x)\n", value);
            break;*/
        /*
            case IRQ_MASK_STA_L: // 置1屏蔽中断，置0允许中断
                if (data == 1)
                {
                    printf("中断状态设置(%x)\n", value);
                }
                break;*/
    case IRQ_MASK_SET_L: // 置1表示禁用对应mask位置，表示不进入中断处理函数
    {
        if (data == 1) // 禁止中断
        {
            IRQ_MASK_SET_L_Data &= ~value;
            // printf("中断掩码设置(%x)\n", value);
        }

        break;
    }

    case IRQ_CLR_MASK_L: // 置1表示可以进入中断处理函数
    {
        if (data == 1)
        {
            // printf("中断掩码清除(%x)\n", value);
            IRQ_MASK_SET_L_Data |= value;
        }
        break;
    }
    default:
        if (address >= RW_SFI_GPRAM_DATA_REG && address <= RW_SFI_GPRAM_DATA_REG + 256) // 假设缓存256字节
        {
            if (data == 1)
            {
                u32 off = address - RW_SFI_GPRAM_DATA_REG;
                off /= 4;
                SF_C_Frame.cacheData[off] = value;
            }
            // printf("Write Flash CacheData(off:%x,value:%x)\n", off, value);
        }
        /*
        // 声音相关寄存器
        if (address >= 0x83000000 && address <= 0x84000000)
        {
            if (data == 1)
            {
                printf("[Sound]%x", address);
                printf(" %x\n", value);
            }
        }*/
        /*
        if (data == 2)
        {
            sprintf(globalSprintfBuff, "address (%x) is unmapping", address);
            confirm("memory read error", globalSprintfBuff);
        }
        else if (data == 3)
        {
            sprintf(globalSprintfBuff, "address(%x) is unmapping", address);
            confirm("memory write error", globalSprintfBuff);
        }
        else if (data == 4)
        {
            sprintf(globalSprintfBuff, "address (%x),code is %d", address, data);
            confirm("error memory operation", globalSprintfBuff);
        }*/
        break;
    }
    /*
        if (address == 0x4b000 && data == 1)
        {
            printf("la======%x====", lastAddress);
            address=0;
        }*/
}

// 是否禁用IRQ中断
bool isIRQ_Disable(u32 cpsr)
{
    return (cpsr & (1 << 7));
}
// 获取真机内存地址
char *getRealMemPtr(u32 ptr)
{
    if (ptr > 0xf0000000)
    {
        return ptr - 0xf0000000 + RAMF0_POOL;
    }
    else if (ptr > 0x40000000)
    {
        return ptr - 0x40000000 + RAM40_POOL;
    }
    else
        return RAM_MEMPOOL + ptr;
}

void SimulatePressKey(u8 key, u8 is_press)
{
    u8 kv;
    bool found = false;
    for (u8 i = 0; i < 72; i++)
    {
        if (keypaddef[i] == key)
        {
            found = true;
            kv = i;
            break;
        }
    }
    if (found)
    {
        // kv是对应的寄存器第几位
        changeTmp = 1;                                // 状态改变
        uc_mem_write(MTK, 0x81070000, &changeTmp, 4); // 有按键按下
        changeTmp = (kv >= 0 && kv < 16) ? (is_press << kv) : 0;
        changeTmp = 0xffff & (~changeTmp);
        uc_mem_write(MTK, 0x81070004, &changeTmp, 2);
        changeTmp = (kv >= 16 && kv < 32) ? (is_press << (kv - 16)) : 0;
        changeTmp = 0xffff & (~changeTmp);
        uc_mem_write(MTK, 0x81070008, &changeTmp, 2);
        changeTmp = (kv >= 32 && kv < 48) ? (is_press << (kv - 32)) : 0;
        changeTmp = 0xffff & (~changeTmp);
        uc_mem_write(MTK, 0x8107000C, &changeTmp, 2);
        changeTmp = (kv >= 48 && kv < 64) ? (is_press << (kv - 48)) : 0;
        changeTmp = 0xffff & (~changeTmp);
        uc_mem_write(MTK, 0x81070010, &changeTmp, 2);
        // 连续按下间隔 t = v / 32ms
        changeTmp = 32;
        uc_mem_write(MTK, 0x81070018, &changeTmp, 2);
    }
}

/**
 * pc指针指向此地址时执行(未执行此地址的指令)
 */

void hookCodeCallBack(uc_engine *uc, uint64_t address, uint32_t size, void *user_data)
{
    switch (address)
    {
        /*
    case 0x8018516:
        uc_reg_read(MTK,UC_ARM_REG_R1, &changeTmp1);
        uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp);
        printf("l1audio.postHisr() %x(%x)\n", changeTmp1, changeTmp);
        break;

    case 0x80184FC:
        uc_reg_read(MTK,UC_ARM_REG_R1, &changeTmp1);
        uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp);
        printf("l1audio.postHisr() %x(%x)\n", changeTmp1, changeTmp);
        break;
        */
        /*
    case 0x819f274:
    {
        uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp);
        uc_reg_read(MTK,UC_ARM_REG_R1, &changeTmp1);
        uc_reg_read(MTK,UC_ARM_REG_R2, &changeTmp2);
        uc_mem_read(MTK,changeTmp2, &globalSprintfBuff, 128);
        printf("IRQ_Register_LISR num[%d] entry[%x] comment: %s\n", changeTmp, changeTmp1, globalSprintfBuff);
        break;
    }*/
    /*
    case 0x83D5E74:
        changeTmp1 = 0xB28000;
        uc_reg_write(MTK,4, &changeTmp1); // 650kb重写
        break;
    */
    case 0x83D1C28: // mr_mem_get()
        changeTmp1 = 0;
        uc_mem_write(MTK, 0xF0166068, &changeTmp1, 4); // 为什么在初始化的时候这里设置为1，原本应该为0
        break;
    case 0x82ACC00: // 模拟器不需要中断回调DMA，所以默认is_poll为true
        changeTmp1 = 1;
        uc_reg_write(MTK, UC_ARM_REG_R5, &changeTmp1);
        break;
        // case 0x81A0F1C:// stack_print
        /*
        case 0x81B1DA4: // tst_sys_trace
            uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
            uc_mem_read(MTK, changeTmp1, &globalSprintfBuff, 128);
            printf("tst_sys_trace(%s)(%x)\n", globalSprintfBuff);
            break;
        case 0x8239244: // 这两个内容相同但地址不一样?
        case 0x8a3dbe0:
            uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp1);
            uc_mem_read(MTK, changeTmp1, &globalSprintfBuff, 128);
            printf("kal_prompt_trace(%s)(%x)\n", globalSprintfBuff, lastAddress);
            break;*/
        /*
    case 0x823dbe0: // kal_prompt_trace_mr
        uc_reg_read(MTK,UC_ARM_REG_R1, &changeTmp1);
        uc_mem_read(MTK,changeTmp1, &globalSprintfBuff, 128);
        printf("kal_prompt_trace_mr(%s)\n", globalSprintfBuff);
        break;
        */
        /*
    case 0x8240088: //_Ven_AT_L___sprintf
                    //    case 0x8A3BDE0: // kal_prompt_trace
        // uc_reg_read(MTK,UC_ARM_REG_R0, &lastSprintfPtr); // 记录第一个参数
        break;
    case 0x82400AE: //_Ven_AT_L___sprintf
        // uc_mem_read(MTK,lastSprintfPtr, &globalSprintfBuff, 128); // 输出处理后的数据
        // printf("_Ven_AT_L___sprintf(%s)\n", globalSprintfBuff);
        break;
    case 0x83CE898:
        uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp);
        uc_reg_read(MTK,UC_ARM_REG_R1, &changeTmp1);
        uc_reg_read(MTK,UC_ARM_REG_R2, &changeTmp2);
        printf("idle refresh sim status(%x,%x,%x)\n", changeTmp, changeTmp1, changeTmp2);
        break;
    case 0x835D824:
        uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp2);
        changeTmp3 = lastAddress;
        break;*/
        /*
    case 0x835D836:
        uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
        uc_mem_read(MTK,changeTmp1, &globalSprintfBuff, 128);
        ucs2_to_utf8(globalSprintfBuff, 128, sprintfBuff, 128);
        printf("ui_get_string(id:%x)(%s)(%x)\n", changeTmp2, sprintfBuff, changeTmp3);
        break;*/
        /*
    case 0x8261CA8:
    case 0x8261Cb0:
    {
        uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
        ReadReg(4, &changeTmp2);
        uc_mem_read(MTK,changeTmp1, &globalSprintfBuff, 128);
        ucs2_to_utf8(globalSprintfBuff, 128, sprintfBuff, 128);
        printf("get_string(id:%x)(%s)(%x)\n", changeTmp2, sprintfBuff, lastAddress);
        break;
    }*/
        /*
        case 0x81D51D8:
            confirm("error", "kal fatal error has occured");
            break;
        case 0x82ac688: // MSDC_DMATransferFinal
            break;*/
        /*
    case 0x82EEBF8:
        uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
        printf("抛出异常(%d)(%x)\n", changeTmp1, lastAddress);
        changeTmp1 = 0;
        break;*/
    case 0x83890C8:
        // srv_charbat_get_charger_status默认返回1，是充电状态
        changeTmp1 = 1;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp1);
        break;
    case 0x8370220: // 直接返回开机流程任务全部完成
        changeTmp1 = 1;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp1);
        // printf("mmi_frm_proc_con_can_complete(%x)\n", changeTmp1);
        break;
    case 0x80E7482:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp);
        // printf("nvram_checksum_compare(%x,%x)\n", changeTmp, changeTmp1);
        // todo 强制过检测，原因后面再看
        uc_reg_write(MTK, UC_ARM_REG_R2, &changeTmp);
        break;
    case 0x8093FB2: // 强制过8093ffa方法
        changeTmp = 1;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp);
        break;
    case 0x80D2CA4:
        // 过sub_80D2CA4，不知道在做什么
        uc_reg_read(MTK, UC_ARM_REG_R5, &changeTmp);
        changeTmp2 = 0xff;
        uc_mem_write(MTK, changeTmp + 3, &changeTmp2, 1);
        break;
    case 0x80601ec:
    case 0x80601ac: // 过sub_8060194的while(L1D_WIN_Init_SetCommonEvent) 暂时去不掉
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp);
        uc_mem_write(MTK, 0x82000000, &changeTmp, 4);
        break;
    case 0x8223F66: // 过sub_8223f5c(L1层的) 暂时去不掉
        changeTmp = 0;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp);
        break;
    case 0x800DA28: // 暂时去不掉
        changeTmp = 0;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp);
        break;
    case 0x4000801E: // 过方法sub_87035D4
        changeTmp = 1;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp);
        break;
    default:
        if (simulateKey != -1)
        {
            // 按键中断8号中断线
            if (StartInterrupt(8, address))
            {
                SimulatePressKey(simulateKey, simulatePress);
                simulateKey = -1;
            }
        }
        else if (currentTime >= last_timer_interrupt_time)
        {
            last_timer_interrupt_time = currentTime + interruptPeroidms;
            // 定时中断2号中断线
            StartInterrupt(2, address);
        }
        /*
        else if (currentTime >= last_rtc_interrupt_time)
        {
            last_rtc_interrupt_time = currentTime + 500;
            if (StartInterrupt(14, address))
            {
                Update_RTC_Time();
                printf("update rtc\n");
            }
        }
        else if (debugType == 10)
        {
            StartInterrupt(12, address);
            debugType = 0;
        }*/
        break;
    }
}
// 通过中断进行回调
bool StartInterrupt(u32 irq_line, u32 lastAddr)
{
    if (IRQ_MASK_SET_L_Data & (1 << irq_line))
    {
        uc_reg_read(MTK, UC_ARM_REG_CPSR, &changeTmp);
        if (!isIRQ_Disable(changeTmp))
        {
            changeTmp1 = 0x50000004;
            SaveCpuContext(&isrStackList[irq_nested_count++], lastAddr);

            uc_reg_write(MTK, UC_ARM_REG_LR, &changeTmp1); // LR更新为特殊寄存器

            changeTmp1 = irq_line;
            uc_mem_write(MTK, IRQ_Status, &changeTmp1, 4);

            changeTmp1 = IRQ_HANDLER;
            // 跳转到中断入口
            uc_reg_write(MTK, UC_ARM_REG_PC, &changeTmp1);
            return true;
        }
    }
    return false;
}

void StartCallback(u32 callbackAddr, u32 backAddr, u32 r0)
{
    isEnterCallback = true;
    changeTmp1 = 0x50000008;
    SaveCpuContext(&stackCallback, backAddr);
    // 开始回调
    uc_reg_write(MTK, UC_ARM_REG_R0, &r0);
    uc_reg_write(MTK, UC_ARM_REG_PC, &callbackAddr);
    uc_reg_write(MTK, UC_ARM_REG_LR, &changeTmp1);
}

void SaveCpuContext(u32 *stackCallbackPtr, u32 backAddr)
{
    uc_reg_read(MTK, UC_ARM_REG_CPSR, stackCallbackPtr);
    if (*stackCallbackPtr++ & 0x20)
        backAddr += 1;
    // 保存状态
    uc_reg_read(MTK, UC_ARM_REG_R0, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R1, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R2, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R3, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R4, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R5, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R6, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R7, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R8, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R9, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R10, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R11, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R12, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_R13, stackCallbackPtr++);
    uc_reg_read(MTK, UC_ARM_REG_LR, stackCallbackPtr++);
    *stackCallbackPtr = backAddr;
}

void RestoreCpuContext(u32 *stackCallbackPtr)
{
    // 还原状态
    uc_reg_write(MTK, UC_ARM_REG_CPSR, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R0, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R1, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R2, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R3, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R4, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R5, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R6, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R7, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R8, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R9, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R10, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R11, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R12, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_R13, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_LR, stackCallbackPtr++);
    uc_reg_write(MTK, UC_ARM_REG_PC, stackCallbackPtr++);
}