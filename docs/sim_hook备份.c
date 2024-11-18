        /*
    case 0x8355512: // 获取网络状态0表示正在搜索 2表示可打紧急电话 3正常 getSimState
        changeTmp1 = 3;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp1);
        break;
    case 0x8355508: // 返回1表示正常getSimStateB，不能返回3
        changeTmp1 = 1;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp1);
        break;
    case 0x82D0612: // 获取SIM状态返回信息地址
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        changeTmp = 3; // 写入3模拟插入SIM卡
        uc_mem_write(MTK, changeTmp1 + 1, &changeTmp, 1);
        globalSprintfBuff[0] = 0x34;
        globalSprintfBuff[1] = 0x36;
        globalSprintfBuff[2] = 0x30;
        globalSprintfBuff[3] = 0x30;
        globalSprintfBuff[4] = 0x32;
        globalSprintfBuff[5] = 0;
        globalSprintfBuff[6] = 0;
        globalSprintfBuff[7] = 0;
        uc_mem_write(MTK, changeTmp1 + 22, &globalSprintfBuff, 7);
        break;*/

        /*
    case 0x8355512: // 获取网络状态0表示正在搜索 2表示可打紧急电话 3正常 getSimState
        changeTmp1 = 3;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp1);
        break;
    case 0x8355508: // 返回1表示正常getSimStateB，不能返回3
        changeTmp1 = 1;
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp1);
        break;
    case 0x82D0612: // 获取SIM状态返回信息地址
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        changeTmp = 3; // 写入3模拟插入SIM卡
        uc_mem_write(MTK, changeTmp1 + 1, &changeTmp, 1);
        globalSprintfBuff[0] = 0x34;
        globalSprintfBuff[1] = 0x36;
        globalSprintfBuff[2] = 0x30;
        globalSprintfBuff[3] = 0x30;
        globalSprintfBuff[4] = 0x32;
        globalSprintfBuff[5] = 0;
        globalSprintfBuff[6] = 0;
        globalSprintfBuff[7] = 0;
        uc_mem_write(MTK, changeTmp1 + 22, &globalSprintfBuff, 7);
        break;*/