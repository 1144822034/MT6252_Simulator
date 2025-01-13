#include "main.h"

void SIM_TIDE_HANDLE(VM_SIM_DEV *sim_dev, u8 sim_num, int64_t value)
{

    sim_dev->rx_trigger_count = (value & 0xf) + 1;
    sim_dev->tx_trigger_count = ((value >> 16) & 0xf) + 1;
    printf("[sim%d]TIDE(tx:%d)(rx:%d)\n", sim_num, sim_dev->tx_trigger_count, sim_dev->rx_trigger_count);
    switch (sim_dev->event)
    {
    case SIM_DEV_EVENT_NONE:
        changeTmp2 = sizeof(SIM_ATR_RSP_DATA) / sizeof(SIM_ATR_RSP_DATA[0]);
        sim_dev->rx_remain_count = changeTmp2;
        my_memcpy(sim_dev->rx_buffer, SIM_ATR_RSP_DATA, changeTmp2);
        sim_dev->rx_buffer_index = 0;
        sim_dev->rx_current_index = 0;
        sim_dev->irq_channel = SIM_IRQ_RX;
        sim_dev->irq_start = true;
        sim_dev->event = SIM_DEV_EVENT_ATR_PRE;
        changeTmp1 = sim_dev->rx_remain_count;
        if (sim_num == 0)
            uc_mem_write(MTK, SIM1_COUNT, &changeTmp1, 4);
        if (sim_num == 1)
            uc_mem_write(MTK, SIM2_COUNT, &changeTmp1, 4);
        printf("[sim%d]开始发送TS、T0两个字节\n", sim_num);
        break;
    case SIM_DEV_EVENT_ATR_PRE: // TS,T0发送完毕
        printf("[sim%d]检查历史字节\n", sim_num);
        sim_dev->rx_current_index = 0;
        // 解析是否有历史字节
        u8 t0 = sim_dev->rx_buffer[1];
        if (t0 >> 7)
        {
            u8 len = t0 & 0xf;
            changeTmp1 = min(sim_dev->rx_remain_count, len);
            uc_mem_write(MTK, SIM1_COUNT, &changeTmp1, 4);
            if (sim_dev->rx_remain_count > len)
            { // 需要中断多次
                sim_dev->event = SIM_DEV_EVENT_ATR_PRE;
            }
            else
            {
                sim_dev->event = SIM_DEV_EVENT_CMD;
            }
            sim_dev->irq_channel = SIM_IRQ_RX;
            sim_dev->irq_start = true;
        }
        else
        { // 没有历史字节
            sim_dev->event = SIM_DEV_EVENT_CMD;
        }
        break;
    default:
        break;
    }
}

void SIM_IRQ_HANDLE(VM_SIM_DEV *sim_dev, u8 sim_num, int64_t value)
{
    sim_dev->irq_enable = value;
    printf("[sim%d] irq enable", sim_num);
    printf("(%x)", lastAddress);
    if (value & SIM_IRQ_TX)
        printf(" [TX] ");
    if (value & SIM_IRQ_RX)
        printf(" [RX] ");
    if (value & SIM_IRQ_TOUT)
        printf(" [TOUT] ");
    if (value & SIM_IRQ_NOATR)
        printf(" [NOATR] ");
    if (value & SIM_IRQ_RXERR)
        printf(" [RXERR] ");
    if (value & SIM_IRQ_T0END)
        printf(" [T0END] ");
    if (value & SIM_IRQ_T1END)
        printf(" [T1END] ");
    if (value & 0 == 0)
        printf("[NONE]");
    printf("(%x)\n", value);
}
void SIM_BASE_HANDLE(VM_SIM_DEV *sim_dev, u8 sim_num, int64_t value)
{
    sim_dev->control = value;
    printf("[sim%d] control(%x)\n", sim_num, value);
}
void SIM_DATA_HANDLE(VM_SIM_DEV *sim_dev, u8 sim_num, u8 isWrite, int64_t value)
{
    if (isWrite == 0)
    {
        changeTmp1 = sim_dev->rx_buffer[sim_dev->rx_buffer_index++];
        sim_dev->rx_current_index++;
        sim_dev->rx_remain_count--;

        uc_mem_write(MTK, SIM2_COUNT, &(sim_dev->rx_remain_count), 4);
        if (sim_num == 0)
            uc_mem_write(MTK, SIM1_DATA, &changeTmp1, 4);
        if (sim_num == 1)
            uc_mem_write(MTK, SIM2_DATA, &changeTmp1, 4);
        printf("[sim%d] read data(%x)(last:%x)\n", sim_num, changeTmp1, lastAddress);
    }
    else
    {
        sim_dev->tx_buffer[sim_dev->tx_buffer_index++] = value;
        printf("[sim%d] write data(%x)\n", sim_num, value);
    }
}
// 接收设备向SIM发送的命令
void handle_sim_tx_cmd(VM_SIM_DEV *sim_dev, u8 sim_num, u32 data_count, u32 dma_data_addr)
{

    uc_mem_read(MTK, dma_data_addr, &sim_dev->T0RxData, data_count);
    printf("[sim%d]开始解析SIM卡命令(字节数：%d)\n", sim_num, data_count);
    for (int i = 0; i < sim_dev->tx_buffer_index; i++)
    {
        printf("%x ", sim_dev->tx_buffer[i]);
    }
    printf("\n");
    u32 flag = sim_dev->tx_buffer_index == 5 && my_mem_compare(sim_dev->tx_buffer, SIM_CMD_SELECT, sim_dev->tx_buffer_index) == 1;
    flag |= (sim_dev->tx_buffer_index == 5 && my_mem_compare(sim_dev->tx_buffer, SIM_CMD_GET_RESPONSE, sim_dev->tx_buffer_index) == 1);
    if (flag)
    {
        sim_dev->rx_buffer_index = 0;
        sim_dev->rx_current_index = 0;
        changeTmp1 = 0;
        changeTmp2 = 0;
        if (sim_dev->tx_buffer[0] == 0xa0) // sim卡的命令响应
        {
            if (sim_dev->tx_buffer[1] == 0xa4)
            {
                printf("[sim%d]SIM卡命令[select file]", sim_num);
                for (int i = 0; i < data_count; i++)
                {
                    printf("%x ", sim_dev->T0RxData[i]);
                }
                printf("\n");
                sim_dev->t0_tx_count = data_count;
                sim_dev->irq_channel = SIM_IRQ_T0END; // 进入中断使接收命令完成，等待设备开启DMA接收响应数据
                sim_dev->irq_start = true;
                changeTmp1 = 0x9f;
                changeTmp2 = (sizeof(SIM_RSP_SF_7F20) / sizeof(SIM_RSP_SF_7F20[0]));
            }
            else if (sim_dev->tx_buffer[1] == 0xc0) // get response
            {
                changeTmp1 = 0x90;
                changeTmp2 = 0x00;
            }
            else
            {
            }
            if (changeTmp1 > 0)
            {
                if (sim_num == 0)
                {
                    uc_mem_write(MTK, SIM1_SW1_REG, &changeTmp1, 4);
                    uc_mem_write(MTK, SIM1_SW2_REG, &changeTmp2, 4);
                }
                if (sim_num == 1)
                {
                    uc_mem_write(MTK, SIM2_SW1_REG, &changeTmp1, 4);
                    uc_mem_write(MTK, SIM2_SW2_REG, &changeTmp2, 4);
                }
            }
        }
    }
    // 命令处理完成置0
    sim_dev->tx_buffer_index = 0;
}
// 处理SIM卡向设备发送的数据
void handle_sim_rx_cmd(VM_SIM_DEV *sim_dev, u8 sim_num, u32 data_count, u32 dma_data_addr)
{
    printf("[sim%d]开始响应SIM命令\n", sim_num);
    changeTmp1 = 0;
    changeTmp2 = 0;
    if (sim_dev->t0_tx_count > 0)
    {
        sim_dev->rx_buffer_index = 0;
        sim_dev->rx_current_index = 0;
        if (sim_dev->t0_tx_count == 2 && my_mem_compare(&sim_dev->T0RxData, &SIM_CMD_SELECT_DF_GSM, sim_dev->t0_tx_count))
        {
            printf("[sim_cmd]select df.gsm(%x)(%x)\n", data_count, dma_data_addr);
            // 复制响应数据
            uc_mem_write(MTK, dma_data_addr, &SIM_RSP_SF_7F20, data_count);
            sim_dev->irq_channel = SIM_IRQ_T0END;
            sim_dev->irq_start = true;
            changeTmp1 = 0x90; // 有数据响应
            changeTmp2 = 0;
        }
        else
        {
            printf("未响应的SIM卡命令");
        }
        my_memset(&sim_dev->T0RxData, 0, sim_dev->t0_tx_count);
    }
    if (changeTmp1 > 0)
    {
        if (sim_num == 0)
        {
            uc_mem_write(MTK, SIM1_SW1_REG, &changeTmp1, 4);
            uc_mem_write(MTK, SIM1_SW2_REG, &changeTmp2, 4);
        }
        if (sim_num == 1)
        {
            uc_mem_write(MTK, SIM2_SW1_REG, &changeTmp1, 4);
            uc_mem_write(MTK, SIM2_SW2_REG, &changeTmp2, 4);
        }
    }
}