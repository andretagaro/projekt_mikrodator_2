#ifndef UART_util
#define UART_util

#define BAUDRATE 9600
#define BAUDVALUE (((F_CPU / (BAUDRATE * 16UL))) - 1)
#define BAUD_PRESCALE (((( F_CPU/16) + (BAUDRATE/2)) / (BAUDRATE))-1)

void usart_send_string(unsigned char *str);
void usart_init(void);
void usart_TX(unsigned char);
unsigned char usart_RX(void);

#endif