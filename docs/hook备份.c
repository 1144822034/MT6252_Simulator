

        
        // gpt中断间隔1s
        if (currentTime >= last_gpt_interrupt_time)
        {
            last_gpt_interrupt_time = currentTime + 1000;
            if (IRQ_MASK_SET_L_Data & 10)
            {
                isIRQ_Disable();
                if (!irqDisable)
                {

                    /*
                    printf("定时中断检测---------开始(%x,%x)\n", irq_lr, irq_pc);
                    dumpCpuInfo();
                    printf("定时中断检测---------结束");*/
                    uc_reg_read(MTK, UC_ARM_REG_LR, &irq_lr[irq_nested_count]); // 保存中断前的lr
                    uc_reg_read(MTK, UC_ARM_REG_CPSR, &irq_cpsr[irq_nested_count]);
                    // 自动备份r0-r3
                    uc_reg_read(MTK, UC_ARM_REG_R0, &isrStackR0[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R1, &isrStackR1[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R2, &isrStackR2[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R3, &isrStackR3[irq_nested_count]);

                    uc_reg_read(MTK, UC_ARM_REG_R4, &isrStackR4[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R5, &isrStackR5[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R6, &isrStackR6[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R7, &isrStackR7[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R8, &isrStackR8[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R9, &isrStackR9[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R10, &isrStackR10[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R11, &isrStackR11[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R12, &isrStackR12[irq_nested_count]);
                    uc_reg_read(MTK, UC_ARM_REG_R13, &isrStackR13[irq_nested_count]);

                    irq_pc[irq_nested_count] = address; // 保存中断返回地址
                    if (irq_cpsr[irq_nested_count] & 0x20)
                    {
                        irq_pc[irq_nested_count] += 1;
                    }
                    changeTmp1 = 0x50000004;
                    uc_reg_write(MTK, UC_ARM_REG_LR, &changeTmp1); // LR更新为特殊寄存器]
                    lastGptType = lastGptType > 1 ? 1 : 2;
                    WriteMemory(0x81060010, &lastGptType, 4);
                    printf("GPT ISR %d\n", lastGptType);
                    changeTmp1 = 10; // 请求10号中断
                    WriteMemory(IRQ_Status, &changeTmp1, 4);
                    changeTmp1 = IRQ_HANDLER;

                    // 跳转到中断入口
                    uc_reg_write(MTK, UC_ARM_REG_PC, &changeTmp1);
                    irq_nested_count++; // 嵌套中断数++
                    return;
                }
            }
        }
    case 0x8223F66: // 过sub_8223f5c(L1层的)
        changeTmp = 0;
        WriteReg(0, &changeTmp);
        break;

    case 0x809404a:
    case 0x8094064: // 过sub_8094040(L1层的)
        changeTmp = 0x5555;
        WriteMemory(0xa0000000, &changeTmp, 4);
        break;

    // 继续过 SD 检测
    case 0x081A194A:
        changeTmp = 2;
        WriteMemory(0x81060010, &changeTmp, 4);
        break;

    case 0x80017DC: // 过方法sub_80017C0
        changeTmp = 2;
        WriteMemory(0x8200021C, &changeTmp, 4);
        changeTmp = 0x3FFFFFF << 16;
        WriteMemory(0x82000224, &changeTmp, 4);
        changeTmp = 660;
        WriteMemory(0x82000228, &changeTmp, 4);
        break;
    case 0x803A426: // 过sub_803A3D0的while循环
        changeTmp = 0;
        WriteMemory(0x82050000, &changeTmp, 2);
        break;
    case 0x80D2CA4:
        // 过sub_80D2CA4，不知道在做什么
        ReadReg(5, &changeTmp);
        changeTmp2 = 0xff;
        WriteMemory(changeTmp + 3, &changeTmp2, 1);
        break;

    /*
    if (address == 0x822014a)
    {
        ReadReg(0, &changeTmp);
        ReadMemory(changeTmp + 0x60, &changeTmp1, 4);
        ReadMemory(changeTmp1 + 8, &changeTmp2, 4);
        // 尝试读取gui buffer指针
        printf("buff ptr(%x)\n", changeTmp2);
        char *pp = changeTmp - 0xf0000000 + RAMF0_POOL;

        changeTmp1 = 0;
    }

    */
    /*
    if (debugType == 11)
    {
        dumpCpuInfo();
        debugType = 11;
    }
    if (debugType == 10 && address == 0x40009BE8) // 9F78保存 A2E4恢复 // 中断恢复上下文操作 9be8(中断栈)与9bcc(非中断栈)恢复到中断前的任务 a290为中断入口
    {
        uc_reg_read(MTK, UC_ARM_REG_SP, &changeTmp1); // 获取SP
        char *pp = changeTmp1 - 0x40000000 + RAM40_POOL;
        dumpCpuInfo();
        debugType = 11;
    }
    if (address == 0x4000A064)
    {                                                 // 将保存上下文之后的SP指针更新到线程中
        uc_reg_read(MTK, UC_ARM_REG_SP, &changeTmp1); // 获取SP
        char *pp = changeTmp1 - 0x40000000 + RAM40_POOL;
        changeTmp1 = 0;
        debugType = 10;
    }*/
    /*
    if (address == 0x800A006)
    {
        debugType = 10;
        printf("MMI_TASK\n");
    }*/
    /*
    if (address == 0x8018638)
    {
        printf("L1FrameInterrupt\n");
    }
    if (address == 0x800A0DE)
    {
        printf("EMMI_Timer\n");
    }
    if (address == 0x8002E7A)
    {
        changeTmp1 = 0;
        changeTmp = 0;
        ReadReg(4, &changeTmp);
        ReadReg(0, &changeTmp1);
        ReadMemory(0x4000B240, &changeTmp3, 4);
        printf("处理中断(入口：%x)(中断号：%d)(中断数:%d)\n", changeTmp1, changeTmp, changeTmp3);
    }*/
    /*
    if (address == 0x80033f0)
    {
        dumpCpuInfo();
        printf("按键高优先级中断\n");
    }
*/
    /*
    if (address == 0x81A13b4)
    {
        printf("drv_get_duration_ms from (%x)\n", lastAddress);
        changeTmp1 = 0;
    }
    if (address == 0x817CC56)
    {
        ReadMemory(SystemTickReg, &changeTmp2, 4);
        ReadReg(0, &changeTmp1);
        printf("ClkStopped (%x)\n", changeTmp1);
        changeTmp1 = 0;
    }*/
    // SD_WaitDatRdyOrTo

    /*
    if (address == 0x831E59A)
    {
        ReadReg(0, &changeTmp);
        printf("空闲Cluster数量(%x)\n", changeTmp);
        changeTmp1 = 0;
    }

    if (address == 0x82CB99A)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        printf("比较SectorSize(%x) <> (%x)\n", changeTmp, changeTmp1);
        changeTmp1 = 0;
    }
    if (address == 0x82CB98C)
    {
        ReadReg(5, &changeTmp);
        changeTmp += 0xc;
        printf("读取SectorSize(从 %x ,调用:%x)\n", changeTmp, lastAddress);
        changeTmp = 0;
    }*/
    /*
    if (address == 0x8160164)
    {
        ReadMemory(0xf0161e48, &changeTmp1, 4);
        changeTmp1 += 0x78;
        printf("读取CSD寄存器(%x )\n", changeTmp1);
        changeTmp1 = 0;
    }*/
    /*
    if (address == 0x8160184)
    {
        ReadReg(1, &changeTmp1);
        ReadReg(4, &changeTmp2);
        ReadReg(3, &changeTmp3);
        changeTmp = changeTmp1 + changeTmp2;
        printf("读取CSD寄存器(%x -> %x)\n", changeTmp, changeTmp3);
        changeTmp1 = 0;
    }*/
    /*
    if (address == 0x816EF0C)
    {
        ReadMemory(0xF01F14C4, &changeTmp, 4);
        printf("读取r_blk_ptr(v:%x)\n", changeTmp);
        changeTmp1 = 0;
    }*/
    /*
    if (address == 0x816F094)
    {
        ReadReg(0, &changeTmp1);
        printf("解析SD v1.0容量(%x)\n", changeTmp1);
        changeTmp1 = 0;
    }*/
    /*
    if (address == 0x824119C)
    {
        ReadReg(4, &changeTmp1);
        printf("获取Dev状态成功(%d)\n", (int)changeTmp1);
        changeTmp1 = 0;
    }*/
    /*
    if (address == 0x833b074)
    {
        ReadReg(0, &changeTmp1);
        printf("make new file hanlde (%x,%x)\n", changeTmp1, lastAddress);
        changeTmp1 = 0;
    }
    if (address == 0x8002872)
    {
        ReadReg(1, &changeTmp1);
        printf("need start timer(%x)\n", changeTmp1);
    }

    if (address == 0x80028A2)
    {
        ReadReg(1, &changeTmp1);
        printf("Expired Timer Call(%x)\n", changeTmp1);
    }
    if (address == 0x4000974C)
    {
        ReadReg(0, &changeTmp1);
        printf("kal_routine(%x)\n", changeTmp1);
        debugType = 0;
    }*/
    /*
    if (address == 0x4000A51C) // tmt_timer_interrupt
    {
        // printf("TMT_No_Timer_Active()\n");
    }
    if (address == 0x4000A108) // tmt_timer_interrupt
    {
        // printf("TCT_Activate_HISR()\n");
    }*/
    // 需要返回不等于-1
    /*
    if (address == 0x84A) // 截取要验证的Flash地址
    {
        ReadReg(1, &changeTmp);
        ReadReg(2, &changeTmp1);
        changeTmp2 = 0;
        ReadMemory(changeTmp, &changeTmp2, 1);
        // printf("Verify Flash Address[%x] RequireValue[%x] RealValue[%x]\n", changeTmp, changeTmp1, changeTmp2);
    }*/
    /*
    if (address == 0x81e9580)
    {
        ReadReg(0, &changeTmp1);
        ReadReg(1, &changeTmp2);
        printf("process cb table(table_index:%d,event_id:%x)\n", changeTmp1, changeTmp2);
    }*/
    /*
    if (address == 0x83647b8)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        printf("mmi_frm_proc_con_create_and_execute(%x,%x)\n", changeTmp, changeTmp1);
        changeTmp = 0;
    }
    if (address == 0x83647E6)
    {
        ReadReg(1, &changeTmp1);
        printf("mmi_frm_proc_con_create_and_execute() status_callback(%x)\n", changeTmp1);
    }*/
    /*
    if (address == 0x8385a1c)
    {//mmi_bootup_ani_completed默认other_workers ok
        ReadReg(0, &changeTmp);
        changeTmp1 = 1;
        WriteMemory(changeTmp + 8, &changeTmp1, 1);
    }*/
    /**/
    /*
    if (address == 0x81EAAC2)
    {
        ReadReg(1, &changeTmp1);
        printf("execute_evt_cb_static_(%x)\n", changeTmp1);
    }
    if (address == 0x81EAADA)
    {
        ReadReg(1, &changeTmp1);
        printf("execute_evt_cb_dynamic(%x)\n", changeTmp1);
    }*/
    /*
    if (address == 0x835D836 || address == 0x82f3bcc)
    {
        ReadReg(0, &changeTmp);
        if (changeTmp > 0)
        {
            memset(&globalSprintfBuff, 0, 128);
            memset(&ucs2Tmp, 0, 128);
            ReadMemory(changeTmp, &globalSprintfBuff, 128);
            utf16_to_utf8(&globalSprintfBuff, utf16_len(&globalSprintfBuff), &ucs2Tmp);
            printf("get_string(%s) last(%x)\n", ucs2Tmp, lastAddress);
        }
    }*/
    /*
    if (address == 0x84a)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        ReadReg(2, &changeTmp2);
        printf("SF_DAL_CheckReadyAndResume(SF_MTD_Data:%x,FlashAddress:%x,lastByte:%x)\n", changeTmp, changeTmp1, changeTmp2);
        changeTmp1=0;
    }*/
    /*
    if (address == 0x8245b68)
    {
        ReadReg(0, &changeTmp1);
        printf("mmi_frm_scrn_enter(parent_id:%d)\n", changeTmp1);
    }
    if (address == 0x837027C)
    {
        ReadReg(0, &changeTmp1);
        printf("fs_check_result(%d)\n", changeTmp1);
    }
    if (address == 0x837548A)
    {
        ReadReg(0, &changeTmp1);
        printf("sim inited??(%d)\n", changeTmp1);
    }*/
    /*
        if (address == 0x81A80DA)
        {
            ReadReg(0, &changeTmp1);
            printf("dequeue_post_evt(%x)\n", changeTmp1);
        }
        if (address == 0x0837144C)
        {
            ReadReg(0, &changeTmp1);
            printf("mmi_frm_proc_execute(%x)\n", changeTmp1);
        }
        if (address == 0x82b7820)
        {
            ReadReg(0, &changeTmp1);
            printf("enqueue_post_evt(%x)\n", changeTmp1);
        }
        if (address == 0x81A8098)
        {
            ReadReg(1, &changeTmp1);
            printf("post_event(%x)\n", changeTmp1);
        }
        if (address == 0x81a8040)
        {
            printf("mmi_frm_invoke_post_event\n");
        }
        if (address == 0x8347ce0)
        {
            printf("mmi_bootup_flow_completed\n");
        }*/

    /*
    if (address == 0x82d0554)
    {
        ReadReg(1, &changeTmp1);
        printf("process_cb_table(%x)\n", changeTmp1);
    }*/
    /*
    if (address == 0x822014A) // 将屏幕缓存刷新到硬件
    {
        lcdUpdateFlag = 1;
    }*/
    /*
    if (address == 0x81643c4)
    {
        ReadReg(0, &changeTmp);
        ReadReg(3, &changeTmp3);
        printf("resizer_init(color=%x,addr=%x)\n", changeTmp, changeTmp3);
    }*/
    /*
    if (address == 0x80ae278)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        ReadReg(4, &changeTmp2);
        printf("l4c_eq_power_on_lind(poweronmode:%d,battery_status:%d,flight_mode:%d)\n", changeTmp, changeTmp1, changeTmp2);
    }*/
    /*
    if (address == 0x81EFBD6)
    {
        ReadReg(0, &changeTmp);
        printf("uem get battery status[%x]\n", changeTmp);
        dumpCpuInfo();
        changeTmp = 0xff;
        WriteReg(0, &changeTmp); // 返回满状态
    }*/
    /*
    if (address == 0x832C398)
    {
        ReadReg(0, &changeTmp);
        printf("handle key code[%x]\n", changeTmp);
    }
    if (address == 0x832C39C)
    {
        ReadReg(0, &changeTmp);
        printf("handle key type[%x]\n", changeTmp);
    }*/
    /*
    if (address == 0x82439d4)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        printf("SetProtocolEventHandler(%d,%x)\n", changeTmp, changeTmp1);
    }

    if (address == 0x80484EC)
    {
        ReadReg(3, &changeTmp1);
        ReadMemory(0xF029EEA0, &changeTmp, 1);
        ReadMemory(0xF029EEA1, &changeTmp2, 1);
        changeTmp &= 0xff;
        changeTmp2 &= 0xff;
        printf("Callback ProtocolEventHandler(%x)(is_launch:%x,isboot:%x)(\n", changeTmp1, changeTmp, changeTmp2);
        changeTmp = 0;
    }*/
    /*
    if (address == 0x84c8c54) // BatteryRsp
    {
        ReadReg(0, &changeTmp);
        changeTmp1 = 0;
        ReadMemory(changeTmp + 4, &changeTmp1, 4);
        printf("enter battery resp entry[%x]\n", changeTmp1);
        changeTmp = 0;
    }*/
    /*
    if (address == 0xd36)
    { // 向flash写入1字节命令3字节地址
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        printf("SFI_GPRAM_Write_C1A3(cmd:%x,address:%x)\n", changeTmp, changeTmp1);
    }*/
    /*
    // 读取flash状态寄存器
    if (address == 0x3B6)
    {
        ReadReg(4, &changeTmp);
        printf("SF_DAL_Dev_ReadStatus[%x]\n", changeTmp);
    }*/
    /*
    if (address == 0x8EA)
    {
        ReadReg(0, &changeTmp);
        printf("SF_DAL_FLAG_BUSY_CLR return[%x]\n", changeTmp);
    }*/
    /*
        if (address == 0xD52)
        {
            ReadReg(0, &changeTmp);
            ReadReg(1, &changeTmp1);
            ReadReg(2, &changeTmp2);
            printf("SFI_GPRAM_Write(GPRAM:%x,Buff:%x,length:%x)\n", changeTmp, changeTmp1, changeTmp2);
        }
        */
    /*
 if (address == 0x928)
 {
     ReadReg(0, &changeTmp);
     ReadReg(1, &changeTmp1);
     ReadReg(2, &changeTmp2);
     ReadReg(3, &changeTmp3);
     printf("SF_DAL_ProgramData(%x,Address:%x,Data:%x,length:%d)\n", changeTmp, changeTmp1, changeTmp2, changeTmp3);
 }*/
    /*
    if (address == 0x800A0F2)
    {
        printf("===================power on==========\n");
        printf("gdi_init() start\n");
    }*/
    /*
    if (address == 0x801FECE)
    {
        ReadReg(2, &changeTmp);
        printf("lc4_main run functable l4c_cnf_ft(%x)\n", changeTmp);
    }*/
    /*
    if (address == 0x8013758)
    {
        u16 a[4];
        ReadReg(0, &changeTmp);
        ReadMemory(changeTmp, &a, 8);
        printf("process_ilm a(%d,%d,%d)\n", a[0], a[1], a[3]);
    }*/
    /*
    if (address == 0x81a7d6c)
    {
        changeTmp = 0;
        ReadMemory(0xf023421e, &changeTmp, 1);
        printf("read circleq messages[%d]\n", changeTmp);
    }*/
    /*
 if (address == 0x800A0CE)
 {
     changeTmp = 0;
     ReadReg(0, &changeTmp);
     printf("mmi handle msgid [%d]\n", changeTmp);
 }*/
    /*
        if (address == 0x8230480)
        {
            printf("l4 timer callback");
        }
    */
    /*
      if (address == 0x8ac7e50)
      {
          printf("AccessRecordInt:%x\n");
      }
      */

    /*
    if (address == 0x824786c)
    {
        dumpCpuInfo();
        printf("gdi mutex lock()\n");
    }*/
    // if (false)

    /*
    if (address == 0x81a7ba0)
    {
        printf("msg_get_ext_queue from [%x]\n", lastAddress);
    }*/
    /*
    if (address == 0x823fe32)
    {
        ReadReg(0, &changeTmp);
        ReadMemory(changeTmp, &globalSprintfBuff, sizeof(struct ilm_struct));
        struct ilm_struct *p = (struct ilm_struct *)globalSprintfBuff;
        ReadMemory(p->local_para_ptr, &changeTmp1, 4);
        printf("msg_send_ext_queue(src_mod_id:%d,dest_mod_id:%d,msg_id:%d,local_ptr:%x|%x,peer_ptr:%x)\n", p->src_mod_id, p->dest_mod_id, p->msg_id, p->local_para_ptr, changeTmp1, p->peer_buff_ptr);
    }*/
    /*
    if (address == 0x800B640)
    {
        // 读取event_group
        ReadReg(3, &changeTmp);
        ReadMemory(changeTmp, &changeTmp, 4);
        printf("kbd_task_main handle[event_group:%x]\n", changeTmp);
    }*/
    /*
    if (address == 0x800B69A)
    {
        ReadReg(0, &changeTmp);
        ReadMemory(changeTmp, &changeTmp1, 4);
        ReadMemory(changeTmp + 4, &changeTmp2, 4);
        printf("kbd_task_main [v2:%x int[%x,%x]]\n", changeTmp, changeTmp1, changeTmp2);
    }*/
    /*

    if (address == 0x819F288)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        printf("isr num[%d] address:[%x]\n",changeTmp, changeTmp1);
        dumpCpuInfo();
        changeTmp = 0;
    }*/
    /*
    if (address == 0x8036520)
    {
        ReadMemory(0x4000b210, &changeTmp, 4);
        changeTmp += 0x5c;
        ReadMemory(changeTmp, &changeTmp1, 4);
        u32 addr = findTaskName(changeTmp1);
        if (addr > 0)
        {
            ReadMemory(addr, &globalSprintfBuff, 128);
            printf("TCC_Task_Shell execute entry[%x] name[%s] last[%x]\n", changeTmp1, globalSprintfBuff, lastAddress);
        }
    }*/
    /*
    if (address == 0x83129f0)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        ReadReg(2, &changeTmp2);
        printf("QUC_Receive_From_Queue(queue:%x,message:%x,size:%x)\n", changeTmp, changeTmp1, changeTmp2);

    }*/
    /*
    if (address == 0x81c794c)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        ReadMemory(changeTmp - 4, &changeTmp, 4);
        printf("TCC_Resume_Task(entry:%x,flag:%x)\n", changeTmp, changeTmp1);

    }*/
    /*
    if (address == 0x800ec30)
    {
        ReadMemory(0xf023421e, &changeTmp, 1);
        ReadMemory(0xf023421c, &changeTmp1, 1);
        printf("OslReadCircularQ::circq_messages[%x] circq_read[%x]\n", changeTmp & 0xff, changeTmp1 & 0xff);
    }*/

    /*
    if (address == 0x8006bd4)
    { // kal_create_task
        ReadReg(0, &changeTmp);
        ReadReg(3, &changeTmp1);
        struct task_list *tpr = &tl[taskPoint++];
        tpr->name = changeTmp;
        tpr->entry = changeTmp1;
        ReadMemory(changeTmp, &globalSprintfBuff, 128);
        printf("kal_create_task(name:%s,entry:%x)\n", globalSprintfBuff, changeTmp1);
    }
    if (address == 0x81c7a1e)
    {
        ReadReg(0, &changeTmp1);
        ReadReg(1, &changeTmp);
        ReadMemory(changeTmp1, &changeTmp1, 4);
        ReadMemory(changeTmp1 - 4, &changeTmp1, 4);
        u32 addr = findTaskName(changeTmp1);
        if (addr > 0)
        {
            ReadMemory(addr, &globalSprintfBuff, 128);
            printf("TCC_Suspend_Task(name:%s,entry:%x,flag:%d)\n", globalSprintfBuff, changeTmp1, changeTmp);
        }
    }*/
    /*
    else if (address == 0x8239db0)
    {
        ReadReg(0, &changeTmp);
        ReadReg(1, &changeTmp1);
        printf("stack_int_set_active_module_id(task_index:%d,module_id:%d)\n", changeTmp, changeTmp1);

    }*/

    /*
    else if (address == 0x819fb6c)
    { // kal_create_event_group
        int r0;
        ReadReg(0, &r0);
        ReadMemory(r0, globalSprintfBuff, 128);
        printf("[kal_create_event_group]:%s\n", globalSprintfBuff);
        changeTmp = 0;
    }*/
    /*
     else if (address == 0x819f570)
     { // kal_create_mutex
         int r0;
         ReadReg(0, &r0);
         ReadMemory(r0, globalSprintfBuff, 128);
         printf("[kal_create_mutex]:%s\n", globalSprintfBuff);
         r0 = 0;
     }
     */
    // 过方法sub_800372C 结束


    
    /*
case 0x8500E74:
    uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
    printf("mr_readFile ret(%x)\n", changeTmp1);
    break;
case 0x84fe830:
    uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
    uc_mem_read(MTK,changeTmp1, &globalSprintfBuff, 128);
    printf("mr_readFile(%s)(%x)\n", globalSprintfBuff, lastAddress);
    break;
case 0x84ff448:
    uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
    printf("mr_malloc(%x)(%x)\n", changeTmp1, lastAddress);
    break;
case 0x84feffc:
    uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
    printf("mr_free(%x)(%x)\n", changeTmp1, lastAddress);
    break;*/
    /*
case 0x8160194:
    uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
    uc_reg_read(MTK,UC_ARM_REG_R1, &lastSDAddress);
    printf("SD_ReadSingleBlock(%x)(%x)\n", changeTmp1, lastSDAddress);
    break;
case 0x81512a0: // SD卡连续读数据块
    uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
    uc_reg_read(MTK,UC_ARM_REG_R1, &changeTmp2);
    printf("SD_ReadMultiBlock(%x)(%x)\n", changeTmp1, changeTmp2);
    break;


case 0x8151444: // SD卡连续写数据块
    uc_reg_read(MTK,UC_ARM_REG_R0, &changeTmp1);
    uc_reg_read(MTK,UC_ARM_REG_R1, &changeTmp2);
    // printf("SD_WriteMultiBlock(%x)(%x)\n", changeTmp1, changeTmp2);
    break;*/
    /*
case 0x850025C: // 0x8500260: // 过滤strcmp(*#220807#)
    uc_reg_read(MTK,UC_ARM_REG_R1, &changeTmp1);
    pp = getRealMemPtr(changeTmp1);
    changeTmp2 = 0;
    break;
case 0x083BF18C: // mr_plat
    dumpCpuInfo();
    changeTmp1 = 0;
    break;
    case 0x8238f78:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        uc_mem_read(MTK, changeTmp1, &globalSprintfBuff, 128);
        printf("nvram_trace(%s)(%x)\n", globalSprintfBuff);
        break;
    case 0x82C9A1C:
        uc_reg_read(MTK, UC_ARM_REG_R0, &changeTmp1);
        uc_mem_read(MTK, changeTmp1, &globalSprintfBuff, 128);
        printf("unk_string(%s)(%x)\n", globalSprintfBuff, lastAddress);
        break;

    */

   
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