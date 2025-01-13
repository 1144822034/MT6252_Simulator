// 定时中断配置，单位毫秒
#define Timer_Interrupt_Duration 1000
// SD卡镜像文件配置
#define SD_CARD_IMG_PATH "Rom\\fat32.img"
// Rom Flash文件配置
#define FLASH_IMG_PATH "Rom\\flash.img"
// Rom Flash lock配置
#define FLASH_IMG_LOCK_PATH "Rom\\flash.lock"
// 系统固件文件
#define ROM_PROGRAM_BIN "Rom\\08000000.bin"
// LCD屏幕宽度
#define LCD_SCREEN_WIDTH 240
// LCD屏幕高度
#define LCD_SCREEN_HEIGHT 320
// CPU中断服务地址
#define CPU_ISR_CB_ADDRESS 0x50000000

// 中断间隔
#define interruptPeroidms 5