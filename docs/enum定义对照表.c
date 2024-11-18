typedef struct {
    void* mr_malloc;
    void* mr_free;
    void* mr_realloc;
    void* memcpy;
    void* memmove;
    void* strcpy;
    void* strncpy;
    void* strcat;
    void* strncat;
    void* memcmp;
    void* strcmp;
    void* strncmp;
    void* strcoll;
    void* memchr;
    void* memset;
    void* strlen;
    void* strstr;
    void* sprintf;
    void* atoi;
    void* strtoul;
    void* rand;
    void* reserve0;
    void* reserve1;
    void** _mr_c_internal_table;
    void** _mr_c_port_table;
    void* _mr_c_function_new;
    void* mr_printf;
    void* mr_mem_get;
    void* mr_mem_free;
    void* mr_drawBitmap;
    void* mr_getCharBitmap;
    void* g_mr_timerStart;
    void* g_mr_timerStop;
    void* mr_getTime;
    void* mr_getDatetime;
    void* mr_getUserInfo;
    void* mr_sleep;
    void* mr_plat;
    void* mr_platEx;
    void* mr_ferrno;
    void* mr_open;
    void* mr_close;
    void* mr_info;
    void* mr_write;
    void* mr_read;
    void* mr_seek;
    void* mr_getLen;
    void* mr_remove;
    void* mr_rename;
    void* mr_mkDir;
    void* mr_rmDir;
    void* mr_findStart;
    void* mr_findGetNext;
    void* mr_findStop;
    void* mr_exit;
    void* mr_startShake;
    void* mr_stopShake;
    void* mr_playSound;
    void* mr_stopSound;
    void* mr_sendSms;
    void* mr_call;
    void* mr_getNetworkID;
    void* mr_connectWAP;
    void* mr_menuCreate;
    void* mr_menuSetItem;
    void* mr_menuShow;
    void* reserve;
    void* mr_menuRelease;
    void* mr_menuRefresh;
    void* mr_dialogCreate;
    void* mr_dialogRelease;
    void* mr_dialogRefresh;
    void* mr_textCreate;
    void* mr_textRelease;
    void* mr_textRefresh;
    void* mr_editCreate;
    void* mr_editRelease;
    void* mr_editGetText;
    void* mr_winCreate;
    void* mr_winRelease;
    void* mr_getScreenInfo;
    void* mr_initNetwork;
    void* mr_closeNetwork;
    void* mr_getHostByName;
    void* mr_socket;
    void* mr_connect;
    void* mr_closeSocket;
    void* mr_recv;
    void* mr_recvfrom;
    void* mr_send;
    void* mr_sendto;
    u16** mr_screenBuf;
    u32* mr_screen_w;
    u32* mr_screen_h;
    u32* mr_screen_bit;
    void* mr_bitmap;
    void* mr_tile;
    u16** mr_map;
    void* mr_sound;
    void* mr_sprite;
    char* pack_filename;
    char* start_filename;
    char* old_pack_filename;
    char* old_start_filename;
    char** mr_ram_file;
    u32* mr_ram_file_len;
    u8* mr_soundOn;
    u8* mr_shakeOn;
    char** LG_mem_base;  // VM 内存基址
    u32* LG_mem_len;   // VM 内存大小
    char** LG_mem_end;   // VM 内存终止
    u32* LG_mem_left;  // VM 剩余内存
    u8* mr_sms_cfg_buf;
    void* mr_md5_init;
    void* mr_md5_append;
    void* mr_md5_finish;
    void* _mr_load_sms_cfg;
    void* _mr_save_sms_cfg;
    void* _DispUpEx;
    void* _DrawPoint;
    void* _DrawBitmap;
    void* _DrawBitmapEx;
    void* DrawRect;
    void* _DrawText;
    void* _BitmapCheck;
    void* _mr_readFile;
    void* mr_wstrlen;
    void* mr_registerAPP;
    void* _DrawTextEx;  // 1936
    void* _mr_EffSetCon;
    void* _mr_TestCom;
    void* _mr_TestCom1;  // 1938
    void* c2u;           // 1939
    void* _mr_div;  // 1941
    void* _mr_mod;
    u32* LG_mem_min;
    u32* LG_mem_top;
    void* mr_updcrc;            // 1943
    char* start_fileparameter;  // 1945
    void* mr_sms_return_flag;   // 1949
    void* mr_sms_return_val;
    void* mr_unzip;           // 1950
    void* mr_exit_cb;  // 1951
    u32* mr_exit_cb_data;   // 1951
    char* mr_entry;           // 1952,V2000-V2002不支持
    void* mr_platDrawChar;    // 1961
} mr_table;


enum {
   MR_NET_ID_MOBILE,                  //移动
   MR_NET_ID_CN,          // 联通gsm
   MR_NET_ID_CDMA,       //联通CDMA
   MR_NET_ID_NONE       //未插卡
};
