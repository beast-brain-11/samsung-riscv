#include <stdio.h>
#include <string.h>
#include <ch32v00x.h>  // Include your board’s SDK

#define UART_RX_PIN GPIO_Pin_6 // RX pin connected to ESP32 TX (PD6)
#define UART_TX_PIN GPIO_Pin_5 // TX pin (not used in this case)

void uart_init() {
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clock for USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // Configure RX pin
    GPIO_InitStructure.GPIO_Pin = UART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Configure TX pin (if needed)
    GPIO_InitStructure.GPIO_Pin = UART_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Configure USART1
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

void uart_read() {
    char buffer[256];
    int index = 0;

    printf("Waiting for data from ESP32...\n");

    while (1) {
        // Wait until a byte is received
        while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

        char c = USART_ReceiveData(USART1);

        if (c == '\n' || index >= sizeof(buffer) - 1) {
            buffer[index] = '\0';  // Null-terminate the string
            printf("Received: %s\n", buffer);
            index = 0;  // Reset index for next message
        } else {
            buffer[index++] = c;
        }
    }
}

int main() {
    SystemInit();
    uart_init();
    uart_read();  // Start receiving data
    return 0;
}
