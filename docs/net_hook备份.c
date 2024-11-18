
    case 0x83BEC08:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp);  // handle
        uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp1); // ip
        uc_reg_read(MTK, UC_ARM_REG_R2, &changeTmp2); // port
        uc_reg_read(MTK, UC_ARM_REG_R3, &changeTmp3); // sync_type 1异步 0同步
        globalSprintfBuff[3] = changeTmp1 & 0xff;
        globalSprintfBuff[2] = (changeTmp1 >> 8) & 0xff;
        globalSprintfBuff[1] = (changeTmp1 >> 16) & 0xff;
        globalSprintfBuff[0] = (changeTmp1 >> 24) & 0xff;
        printf("mr_connect(ip:%d.%d.%d.%d,port:%d,syncType:%d)\n", globalSprintfBuff[0], globalSprintfBuff[1], globalSprintfBuff[2], globalSprintfBuff[3], changeTmp2, changeTmp3);
        // 目前只处理异步
        sh1 = create_socket(changeTmp);
        sprintf(sh1->ip, "%d.%d.%d.%d", globalSprintfBuff[0], globalSprintfBuff[1], globalSprintfBuff[2], globalSprintfBuff[3]);
        sprintf(sh1->port, "%d", changeTmp2);
        if (!strcmp(sh1->ip, "10.0.0.172"))
        { // 如果是代理IP，重定向服务器IP
            strcpy(sh1->ip, "43.136.107.137");
        }
        connect_socket_inner(sh1);
        break;
    case 0x83BEC01: // 关闭socket句柄
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        printf("mr_closeSocket(%x)\n", changeTmp1);
        sh1 = get_socket(changeTmp1);
        close_socket_inner(sh1);
        delete_socket(sh1);
        break;
    case 0x83BF686: // 发送数据
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp2);
        uc_reg_read(MTK, UC_ARM_REG_R2, &changeTmp3);
        sh1 = get_socket(changeTmp1);
        uc_mem_read(MTK, changeTmp2, &(sh1->sendBuff), changeTmp3);
        /*
        if (!strcmp(sh1->ip, "10.0.0.172")) // 是代理
        {                                   // 如果是代理，重定向IP
            // 提取HOST
            char *first = strstr(sh1->sendBuff, "\r\nHost:");
            if (first)
            {
                first += 8; // 过滤掉\r\nHost:部分
                char *second = strstr(first, "\r\n");
                int len = second - first;
                memset(sh1->ip, 0, len);
                memcpy(sh1->ip, first, len);
            }
            redirect_socket_inner(sh1);
        }*/
        changeTmp = send_socket_inner(sh1, changeTmp3);
        changeTmp = -1;
        printf("send>>>%s\n", sh1->sendBuff);
        printf("mr_send(handle:%x,buff:%x,len:%x)(ret:%x)\n", changeTmp1, changeTmp2, changeTmp3, changeTmp);
        // 直接返回结果
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp);
        changeTmp = 0x83BF68A + 1;
        uc_reg_write(MTK, UC_ARM_REG_R1, &changeTmp);
        break;
    case 0x83BF5C2: // 读取数据
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp2);
        uc_reg_read(MTK, UC_ARM_REG_R2, &changeTmp3);
        sh1 = get_socket(changeTmp1);
        changeTmp = receive_socket_inner(sh1, changeTmp3);
        uc_mem_write(MTK, changeTmp2, &(sh1->revBuff), changeTmp);
        printf("recv>>>%s\n", sh1->revBuff);
        printf("mr_recv(handle:%x,buff:%x,len:%x)(ret:%x)\n", changeTmp1, changeTmp2, changeTmp3, changeTmp);
        // 直接返回结果
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp);
        changeTmp = 0x83BF5C6 + 1;
        break;
    case 0x83BF804:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        printf("mr_sendTo(%x)\n", changeTmp1);
        break;
    case 0x83BF5C8:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        printf("mr_recvFrom(%x)\n", changeTmp1);
        break;
    case 0x83BEFA8:
        if (!isEnterCallback) // 防止嵌套执行
        {
            uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1); // domain
            uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp2); // callback
            uc_mem_read(MTK, changeTmp1, &globalSprintfBuff, 128);
            printf("mr_getHostByName(s:%s)", globalSprintfBuff);
            changeTmp3 = get_host_ip_inner(globalSprintfBuff);
            StartCallback(changeTmp2, address, changeTmp3);
            printf("ret(%x)\n", changeTmp3);
        }
        else
            isEnterCallback = false;
        // changeTmp3 = get_host_ip(globalSprintfBuff);
        // SimulateFunc(changeTmp2, changeTmp3, 0, 0); // 回调结果
        break;

        
    case 0x83BEC08:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp);  // handle
        uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp1); // ip
        uc_reg_read(MTK, UC_ARM_REG_R2, &changeTmp2); // port
        uc_reg_read(MTK, UC_ARM_REG_R3, &changeTmp3); // sync_type 1异步 0同步
        globalSprintfBuff[3] = changeTmp1 & 0xff;
        globalSprintfBuff[2] = (changeTmp1 >> 8) & 0xff;
        globalSprintfBuff[1] = (changeTmp1 >> 16) & 0xff;
        globalSprintfBuff[0] = (changeTmp1 >> 24) & 0xff;
        printf("mr_connect(ip:%d.%d.%d.%d,port:%d,syncType:%d)\n", globalSprintfBuff[0], globalSprintfBuff[1], globalSprintfBuff[2], globalSprintfBuff[3], changeTmp2, changeTmp3);
        // 目前只处理异步
        sh1 = create_socket(changeTmp);
        sprintf(sh1->ip, "%d.%d.%d.%d", globalSprintfBuff[0], globalSprintfBuff[1], globalSprintfBuff[2], globalSprintfBuff[3]);
        sprintf(sh1->port, "%d", changeTmp2);
        if (!strcmp(sh1->ip, "10.0.0.172"))
        { // 如果是代理IP，重定向服务器IP
            strcpy(sh1->ip, "43.136.107.137");
        }
        connect_socket_inner(sh1);
        break;
    case 0x83BEC01: // 关闭socket句柄
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        printf("mr_closeSocket(%x)\n", changeTmp1);
        sh1 = get_socket(changeTmp1);
        close_socket_inner(sh1);
        delete_socket(sh1);
        break;
    case 0x83BF686: // 发送数据
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp2);
        uc_reg_read(MTK, UC_ARM_REG_R2, &changeTmp3);
        sh1 = get_socket(changeTmp1);
        uc_mem_read(MTK, changeTmp2, &(sh1->sendBuff), changeTmp3);
        /*
        if (!strcmp(sh1->ip, "10.0.0.172")) // 是代理
        {                                   // 如果是代理，重定向IP
            // 提取HOST
            char *first = strstr(sh1->sendBuff, "\r\nHost:");
            if (first)
            {
                first += 8; // 过滤掉\r\nHost:部分
                char *second = strstr(first, "\r\n");
                int len = second - first;
                memset(sh1->ip, 0, len);
                memcpy(sh1->ip, first, len);
            }
            redirect_socket_inner(sh1);
        }*/
        changeTmp = send_socket_inner(sh1, changeTmp3);
        changeTmp = -1;
        printf("send>>>%s\n", sh1->sendBuff);
        printf("mr_send(handle:%x,buff:%x,len:%x)(ret:%x)\n", changeTmp1, changeTmp2, changeTmp3, changeTmp);
        // 直接返回结果
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp);
        changeTmp = 0x83BF68A + 1;
        uc_reg_write(MTK, UC_ARM_REG_R1, &changeTmp);
        break;
    case 0x83BF5C2: // 读取数据
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp2);
        uc_reg_read(MTK, UC_ARM_REG_R2, &changeTmp3);
        sh1 = get_socket(changeTmp1);
        changeTmp = receive_socket_inner(sh1, changeTmp3);
        uc_mem_write(MTK, changeTmp2, &(sh1->revBuff), changeTmp);
        printf("recv>>>%s\n", sh1->revBuff);
        printf("mr_recv(handle:%x,buff:%x,len:%x)(ret:%x)\n", changeTmp1, changeTmp2, changeTmp3, changeTmp);
        // 直接返回结果
        uc_reg_write(MTK, UC_ARM_REG_R0, &changeTmp);
        changeTmp = 0x83BF5C6 + 1;
        break;
    case 0x83BF804:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        printf("mr_sendTo(%x)\n", changeTmp1);
        break;
    case 0x83BF5C8:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        printf("mr_recvFrom(%x)\n", changeTmp1);
        break;
    case 0x83BEFA8:
        if (!isEnterCallback) // 防止嵌套执行
        {
            uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1); // domain
            uc_reg_read(MTK, UC_ARM_REG_R1, &changeTmp2); // callback
            uc_mem_read(MTK, changeTmp1, &globalSprintfBuff, 128);
            printf("mr_getHostByName(s:%s)", globalSprintfBuff);
            changeTmp3 = get_host_ip_inner(globalSprintfBuff);
            StartCallback(changeTmp2, address, changeTmp3);
            printf("ret(%x)\n", changeTmp3);
        }
        else
            isEnterCallback = false;
        // changeTmp3 = get_host_ip(globalSprintfBuff);
        // SimulateFunc(changeTmp2, changeTmp3, 0, 0); // 回调结果
        break;