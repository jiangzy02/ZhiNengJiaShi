//硬件驱动
#include "../UART6_ESP01/UART6.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>



/*
************************************************************
*   函数名称：   Usart6_Init
*
*   函数功能：   串口6初始化
*
*   入口参数：   baud：设定的波特率
*
*   返回参数：   无
*
*   说明：     TX-Pc0     RX-PC1
************************************************************
*/
void Usart6_Init(unsigned int baud)
{

    GPIO_InitTypeDef gpio_initstruct;
    USART_InitTypeDef usart_initstruct;
    NVIC_InitTypeDef nvic_initstruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART6, ENABLE);

    //PC0   TXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_0;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_initstruct);

    //PC1   RXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_1;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_initstruct);

    usart_initstruct.USART_BaudRate = baud;
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;        //无硬件流控
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                        //接收和发送
    usart_initstruct.USART_Parity = USART_Parity_No;                                    //无校验
    usart_initstruct.USART_StopBits = USART_StopBits_1;                             //1位停止位
    usart_initstruct.USART_WordLength = USART_WordLength_8b;                            //8位数据位
    USART_Init(UART6, &usart_initstruct);

    USART_Cmd(UART6, ENABLE);                                                      //使能串口

    USART_ITConfig(UART6, USART_IT_RXNE, ENABLE);                                  //使能接收中断

    nvic_initstruct.NVIC_IRQChannel = UART6_IRQn;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_initstruct);

}

/*
************************************************************
*   函数名称：   Usart_SendString
*
*   函数功能：   串口数据发送
*
*   入口参数：   USARTx：串口组
*               str：要发送的数据
*               len：数据长度
*
*   返回参数：   无
*
*   说明：
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);                                 //发送数据
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);     //等待发送完成
    }

}

/*
************************************************************
*   函数名称：   UsartPrintf
*
*   函数功能：   格式化打印
*
*   入口参数：   USARTx：串口组
*               fmt：不定长参
*
*   返回参数：   无
*
*   说明：
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);                         //格式化
    va_end(ap);

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }

}
