#include "stm32f4xx.h"
#include "bl_usart.h"
//USART3 RX PB11
//USART3 TX PB10
//MODE: 8N1
//BAUDRATE: 115200
//DMA: TX and RX
//IT: RX/DMA_TX_TC/DMA_RX_TC

static bl_usart_rx_callback_t rx_callback;

static void usart_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}
// static void usart_dma_config(void)
// {
//     DMA_InitTypeDef DMA_InitStructure;
//     DMA_StructInit(&DMA_InitStructure);
//     //RX
//     DMA_InitStructure.DMA_Channel = DMA_Channel_4;//?
//     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
//     DMA_InitStructure.DMA_Memory0BaseAddr = 0;
//     DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//     DMA_InitStructure.DMA_BufferSize = 0;//?
//     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//     DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//     DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
//     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//     DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC16;
//     DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//     DMA_Init(DMA1_Stream1, &DMA_InitStructure);
//     // DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);
//     DMA_Cmd(DMA1_Stream1, DISABLE);

//     //TX
//     DMA_InitStructure.DMA_Channel = DMA_Channel_4;
//     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
//     DMA_InitStructure.DMA_Memory0BaseAddr = 0;
//     DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//     DMA_InitStructure.DMA_BufferSize = 0;
//     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//     DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//     DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
//     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//     DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC16;
//     DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//     DMA_Init(DMA1_Stream3, &DMA_InitStructure);
//     // DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
//     DMA_Cmd(DMA1_Stream3, DISABLE);

// }
static void usart_it_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    //usartRX
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // //DMA TX
    // NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
    // //DMA RX
    // NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
}
static void usart_lowlevel_init(void)
{
    USART_InitTypeDef USART_InitStructure;
    USART_StructInit(&USART_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStructure);
    // USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
    // USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);

}
void bl_usart_init(void)
{

    // usart_dma_config();
    usart_it_config();
    usart_lowlevel_init();
    usart_io_init();
}
//TLV
//type length value
void bl_usart_write(const uint8_t *data, uint32_t size)
{
    while(size--)
    {
        USART_SendData(USART3, *data++);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    }
    // while(size > 0)
    // {
    //     uint32_t chunk = size > 65536 ? 65536 : size;

    //     DMA_Cmd(DMA1_Stream3, DISABLE);
    //     DMA1_Stream3->NDTR = chunk;
    //     DMA1_Stream3->M0AR = (uint32_t)data;
    //     DMA_Cmd(DMA1_Stream3, ENABLE);
    //     while(DMA_GetCmdStatus(DMA1_Stream3) == ENABLE);
    //     data += chunk;
    //     size -= chunk;

    // }
}
void bl_usart_register_rx_callback(bl_usart_rx_callback_t callback)
{
    rx_callback = callback;
}

void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        if(rx_callback)
        {
            uint8_t data = (uint8_t)USART_ReceiveData(USART3);
            rx_callback(&data, 1);
        }
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
//DMA1_Stream3 for USART3 TX
void DMA1_Stream3_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_TC) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TC);
    }
}
//DMA1_Stream1 for USART3 RX
void DMA1_Stream1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TC != RESET))
    {
        DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TC);
    }
}
