#ifndef __DEFINED__

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long
#define MR_MOUSE_DOWN 1
#define MR_MOUSE_UP 2
#define MR_MOUSE_MOVE 3
#define MR_KEY_PRESS 4
#define MR_KEY_RELEASE 5

// nucleus os 定义
#define NU_DRIVER_SUSPEND 10 // 设备暂停
#define NU_EVENT_SUSPEND 7   // 事件暂停
#define NU_FINISHED 11       // 结速暂停
#define NU_MAILBOX_SUSPEND 3
#define NU_MEMORY_SUSPEND 9
#define NU_PARTITION_SUSPEND 8
#define NU_PIPE_SUSPEND 5
#define NU_PURE_SUSPEND 1  // 挂起
#define NU_QUEUE_SUSPEND 4 // 请求暂停
#define NU_READY 0
#define NU_SEMAPHORE_SUSPEND 6 // 信号量暂停
#define NU_SLEEP_SUSPEND 2     // 睡眠暂停
#define NU_TERMINATED 12

/**
 * 初始化屏幕缓存 240 * 320
 */
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define CONFIG_START_ADDRESS 0x80000000
#define CONFIG_SIZE 0x400000 // 4MB

#define TDMA_START_ADDRESS 0x80200000
#define TDMA_SIZE 0x400000 // 4MB

#define INIT_SRAM_START_ADDRESS 0x40000000
#define INIT_SRAM_SIZE 0x47000

#define RAM_START_ADDRESS 0
#define RAM_SIZE 0x300000 // 4MB

#define ROM_START_ADDRESS 0x8000000
#define ROM_SIZE 0x1000000 // 16MB
#define ROM_END_ADDRESS (ROM_START_ADDRESS + ROM_SIZE)

/**
 * 栈空间配置
 */
#define STACK_SIZE 4096                                    // 栈大小
#define STACK_START_ADDRESS (ROM_END_ADDRESS - STACK_SIZE) // 栈开始地址
#define STACK_END_ADDRESS STACK_START_ADDRESS              // 栈结束地址

#define SFI_TRIG 1   // 触发命令
#define SFI_MAC_EN 2 // Memory Access Control
#define SFI_WIP 4    // 表示正在写入中
#define SFI_EN 8     // 开启使能

#define RW_SFI_MAC_CTL 0x810a0000
#define RW_SFI_GPRAM_DATA 0x810a0800
#define RW_SFI_GPRAM_CMD_REG 0x810a0800  // 要发送的命令寄存器
#define RW_SFI_GPRAM_DATA_REG 0x810a0804 // 要发送的数据寄存器?
#define RW_SFI_GPRAM_BUSY_REG 0x83010a28 // Flash忙寄存器
#define RW_SFI_OUTPUT_LEN_REG 0x810a0004 // 向Flash中写入多少字节
#define RW_SFI_INPUT_LEN_REG 0x810a0008  // 从Flash中读取多少字节

#define SD_ARG_REG 0x810e0028       // SD 参数寄存器
#define SD_CMD_REG 0x810e0024       // SD 命令寄存器
#define SD_DATA_STAT_REG 0x810e002c // 读取SD 数据状态寄存器
#define SD_CMD_STAT_REG 0x810e0040  // 读取SD 命令状态寄存器

#define SD_DATA_RESP_REG0 0x810e0030 // SD 数据响应寄存器 r0
#define SD_DATA_RESP_REG1 0x810e0034 // SD 数据响应寄存器 r1
#define SD_DATA_RESP_REG2 0x810e0038 // SD 数据响应寄存器 r2
#define SD_DATA_RESP_REG3 0x810e003c // SD 数据响应寄存器 r3

#define SD_CMD_RESP_REG0 0x810e0000 // SDC 命令响应寄存器 r0
#define SD_CMD_RESP_REG1 0x810e0004 // SDC 命令响应寄存器 r1
#define SD_CMD_RESP_REG2 0x810e0008 // SDC 命令响应寄存器 r2
#define SD_CMD_RESP_REG3 0x810e000c // SDC 命令响应寄存器 r3

#define SDC_DATSTA_REG 0x810E0044 // MSDC数据状态寄存器
#define MSDC_CFG_REG 0x810E0000   // MSDC命令寄存器?
// 如果 MSDC_STA 寄存器的 MSDC_STA_FIFOCNT 位被置位，表示 FIFO 中有数据可以读取
#define MSDC_STA_REG 0x810E0004      // 这是 MSDC 命令状态寄存器，它可能包含各种状态标志，例如 FIFO 是否满、错误标志等。在这里，它也包括 FIFOCLR 位，当该位被置位时，MSDC 控制器将清除其内部 FIFO。
#define MSDC_DAT_REG 0x810E000C      // MSDC数据寄存器
#define EINT_MASK_CLR_REG 0x81010108 // 外部中断掩码寄存器

#define SystemTickReg 0x82000230

#define PIXEL565R(v) ((((u32)v >> 11) << 3) & 0xff) // 5位红色
#define PIXEL565G(v) ((((u32)v >> 5) << 2) & 0xff)  // 6位绿色
#define PIXEL565B(v) (((u32)v << 3) & 0xff)         // 5位蓝色

#define FLASH_BASE_ADDR 0x8800000
#define NOR_FLASH_BASE_ADDRESS 0x760000

#define DMA_GLBSTA 0x81020000

#define GPTIMER1_CON 0x81060000

#define SDC_CMD_CMD0 0x0000
#define SDC_CMD_CMD1 0x0181
#define SDC_CMD_CMD2 0x0502
#define SDC_CMD_CMD3_SD 0x0303
#define SDC_CMD_CMD3_MMC 0x0083
#define SDC_CMD_CMD4 0x0004
#define SDC_CMD_CMD7 0x0387
#define SDC_CMD_CMD8 0x0088
#define SDC_CMD_CMD9 0x0109
#define SDC_CMD_CMD10 0x010a
#define SDC_CMD_CMD11_MMC 0x188b
#define SDC_CMD_CMD12 0x438c
#define SDC_CMD_CMD13 0x008d
#define SDC_CMD_CMD15 0x000f
#define SDC_CMD_CMD16 0x0090
#define SDC_CMD_CMD17 0x0891
#define SDC_CMD_CMD18 0x1092
#define SDC_CMD_CMD20_MMC 0x3894

#define SDC_CMD_CMD24 0x2898
#define SDC_CMD_CMD25 0x3099
#define SDC_CMD_CMD26 0x009a
#define SDC_CMD_CMD27 0x009b
#define SDC_CMD_CMD28 0x039c
#define SDC_CMD_CMD29 0x039d
#define SDC_CMD_CMD30 0x089e
#define SDC_CMD_CMD32 0x00a0
#define SDC_CMD_CMD33 0x00a1
#define SDC_CMD_CMD34_MMC 0x00a2
#define SDC_CMD_CMD35_MMC 0x00a3
#define SDC_CMD_CMD36_MMC 0x00a4
#define SDC_CMD_CMD37_MMC 0x00a5
#define SDC_CMD_CMD38 0x03a6
#define SDC_CMD_CMD39_MMC 0x0227
#define SDC_CMD_CMD40_MMC 0x82a8
#define SDC_CMD_CMD41_SD 0x01a9
#define SDC_CMD_CMD42 0x2baa
#define SDC_CMD_CMD55 0x00b7
#define SDC_CMD_CMD56 0x00b8
#define SDC_CMD_ACMD6 0x0086
#define SDC_CMD_ACMD13 0x088d
#define SDC_CMD_ACMD22 0x0896
#define SDC_CMD_ACMD23 0x0097
#define SDC_CMD_ACMD42 0x00aa
#define SDC_CMD_ACMD51 0x08b3

#define RTF_NO_ERROR 0
#define RTF_ERROR_RESERVED -1
#define RTF_PARAM_ERROR -2
#define RTF_INVALID_FILENAME -3
#define RTF_DRIVE_NOT_FOUND -4
#define RTF_TOO_MANY_FILES -5
#define RTF_NO_MORE_FILES -6
#define RTF_WRONG_MEDIA -7
#define RTF_INVALID_FILE_SYSTEM -8
#define RTF_FILE_NOT_FOUND -9
#define RTF_INVALID_FILE_HANDLE -10
#define RTF_UNSUPPORTED_DEVICE -11
#define RTF_UNSUPPORTED_DRIVER_FUNCTION -12
#define RTF_CORRUPTED_PARTITION_TABLE -13
#define RTF_TOO_MANY_DRIVES -14
#define RTF_INVALID_FILE_POS -15
#define RTF_ACCESS_DENIED -16
#define RTF_STRING_BUFFER_TOO_SMALL -17
#define RTF_GENERAL_FAILURE -18
#define RTF_PATH_NOT_FOUND -19
#define RTF_FAT_ALLOC_ERROR -20
#define RTF_ROOT_DIR_FULL -21
#define RTF_DISK_FULL -22
#define RTF_TIMEOUT -23
#define RTF_BAD_SECTOR -24
#define RTF_DATA_ERROR -25
#define RTF_MEDIA_CHANGED -26
#define RTF_SECTOR_NOT_FOUND -27
#define RTF_ADDRESS_MARK_NOT_FOUND -28
#define RTF_DRIVE_NOT_READY -29
#define RTF_WRITE_PROTECTION -30
#define RTF_DMA_OVERRUN -31
#define RTF_CRC_ERROR -32
#define RTF_DEVICE_RESOURCE_ERROR -33
#define RTF_INVALID_SECTOR_SIZE -34
#define RTF_OUT_OF_BUFFERS -35
#define RTF_FILE_EXISTS -36
#define RTF_LONG_FILE_POS -37
#define RTF_FILE_TOO_LARGE -38
#define RTF_BAD_DIR_ENTRY -39
#define RTF_ATTR_CONFLICT -40            // Recoverable support: Can't specify FS_PROTECTION_MODE and FS_NONBLOCK_MODE
#define RTF_CHECKDISK_RETRY -41          // Recoverable support: used for CROSSLINK
#define RTF_LACK_OF_PROTECTION_SPACE -42 // UN-USED

#define IRQ_SEL0 0x81010000       // 中断选择寄存器
#define IRQ_MASK_STA_L 0x81010070 // 中断掩码寄存器
#define IRQ_MASK_STA_H 0x81010074 // 中断掩码寄存器
#define IRQ_Status 0x810100d8     // 中断状态寄存器

#define IRQ_EOI2 0x810100dc
#define IRQ_EOIL 0x810100a0 // 中断完成寄存器
#define IRQ_EOIH 0x810100a4 // 中断完成寄存器

#define FIQ_FEOI 0x810100D4 // 中断完成寄存器
#define IRQ_CLR_MASK_L 0x81010080
#define IRQ_CLR_MASK_H 0x81010084
#define IRQ_MASK_SET_L 0x81010090
#define IRQ_MASK_SET_H 0x81010094
#define IRQ_Status 0x810100d8 // 中断状态寄存器，表示哪个中断源请求中断，最大值63，最小0

#define RTC_IRQ_STATUS 0x810b0004 //

#define TCD_Current_Thread 0x4000b238

#define UART_Interrupt_Identification_REG 0x81030008
#define UART_LINE_STATUS_REG 0x81030014
// 应该是0x18，但有问题
#define IRQ_HANDLER 0x4000A290 // 中断入口地址
#define IRQ_HANDLER 0x4000A290 // 中断入口地址

typedef enum
{
    NotMounted,
    Initialized,
    Mounted,
    Accessible,
    SizeExceeded,
    HasFileSystem
} MountStates;

typedef enum
{
    FS_NO_ERROR = 0,
    FS_ERROR_RESERVED = -1,
    FS_PARAM_ERROR = -2,
    FS_INVALID_FILENAME = -3,
    FS_DRIVE_NOT_FOUND = -4,
    FS_TOO_MANY_FILES = -5,
    FS_NO_MORE_FILES = -6,
    FS_WRONG_MEDIA = -7,
    FS_INVALID_FILE_SYSTEM = -8,
    FS_FILE_NOT_FOUND = -9,
    FS_INVALID_FILE_HANDLE = -10,
    FS_UNSUPPORTED_DEVICE = -11,
    FS_UNSUPPORTED_DRIVER_FUNCTION = -12,
    FS_CORRUPTED_PARTITION_TABLE = -13,
    FS_TOO_MANY_DRIVES = -14,
    FS_INVALID_FILE_POS = -15,
    FS_ACCESS_DENIED = -16,
    FS_STRING_BUFFER_TOO_SMALL = -17,
    FS_GENERAL_FAILURE = -18,
    FS_PATH_NOT_FOUND = -19,
    FS_FAT_ALLOC_ERROR = -20,
    FS_ROOT_DIR_FULL = -21,
    FS_DISK_FULL = -22,
    FS_TIMEOUT = -23,
    FS_BAD_SECTOR = -24,
    FS_DATA_ERROR = -25,
    FS_MEDIA_CHANGED = -26,
    FS_SECTOR_NOT_FOUND = -27,
    FS_ADDRESS_MARK_NOT_FOUND = -28,
    FS_DRIVE_NOT_READY = -29,
    FS_WRITE_PROTECTION = -30,
    FS_DMA_OVERRUN = -31,
    FS_CRC_ERROR = -32,
    FS_DEVICE_RESOURCE_ERROR = -33,
    FS_INVALID_SECTOR_SIZE = -34,
    FS_OUT_OF_BUFFERS = -35,
    FS_FILE_EXISTS = -36,
    FS_LONG_FILE_POS = -37,
    FS_FILE_TOO_LARGE = -38,
    FS_BAD_DIR_ENTRY = -39,
    FS_ATTR_CONFLICT = -40,
    FS_CHECKDISK_RETRY = -41,
    FS_LACK_OF_PROTECTION_SPACE = -42,
    FS_SYSTEM_CRASH = -43,
    FS_FAIL_GET_MEM = -44,
    FS_READ_ONLY_ERROR = -45,
    FS_DEVICE_BUSY = -46,
    FS_ABORTED_ERROR = -47,
    FS_QUOTA_OVER_DISK_SPACE = -48,
    FS_PATH_OVER_LEN_ERROR = -49,
    FS_APP_QUOTA_FULL = -50,
    FS_VF_MAP_ERROR = -51,
    FS_DEVICE_EXPORTED_ERROR = -52,
    FS_DISK_FRAGMENT = -53,
    FS_DIRCACHE_EXPIRED = -54,
    FS_QUOTA_USAGE_WARNING = -55,
    FS_ERR_DIRDATA_LOCKED = -56,
    FS_INVALID_OPERATION = -57,
    FS_ERR_VF_PARENT_CLOSED = -58,
    FS_ERR_UNSUPPORTED_SERVICE = -59,

    FS_ERR_INVALID_JOB_ID = -81,
    FS_ERR_ASYNC_JOB_NOT_FOUND = -82,

    FS_MSDC_MOUNT_ERROR = -100,
    FS_MSDC_READ_SECTOR_ERROR = -101,
    FS_MSDC_WRITE_SECTOR_ERROR = -102,
    FS_MSDC_DISCARD_SECTOR_ERROR = -103,
    FS_MSDC_PRESNET_NOT_READY = -104,
    FS_MSDC_NOT_PRESENT = -105,

    FS_EXTERNAL_DEVICE_NOT_PRESENT = -106,
    FS_HIGH_LEVEL_FORMAT_ERROR = -107,

    FS_CARD_BATCHCOUNT_NOT_PRESENT = -110,

    FS_FLASH_MOUNT_ERROR = -120,
    FS_FLASH_ERASE_BUSY = -121,
    FS_NAND_DEVICE_NOT_SUPPORTED = -122,
    FS_FLASH_OTP_UNKNOWERR = -123,
    FS_FLASH_OTP_OVERSCOPE = -124,
    FS_FLASH_OTP_WRITEFAIL = -125,
    FS_FDM_VERSION_MISMATCH = -126,
    FS_FLASH_OTP_LOCK_ALREADY = -127,
    FS_FDM_FORMAT_ERROR = -128,

    FS_FDM_USER_DRIVE_BROKEN = -129,
    FS_FDM_SYS_DRIVE_BROKEN = -130,
    FS_FDM_MULTIPLE_BROKEN = -131,

    FS_LOCK_MUTEX_FAIL = -141,
    FS_NO_NONBLOCKMODE = -142,
    FS_NO_PROTECTIONMODE = -143,

    FS_DISK_SIZE_TOO_LARGE = (FS_MSDC_MOUNT_ERROR),

    FS_MINIMUM_ERROR_CODE = -65536
} fs_error_enum;

#define MSDC_Handle_Adr 0xF01D2894

#endif