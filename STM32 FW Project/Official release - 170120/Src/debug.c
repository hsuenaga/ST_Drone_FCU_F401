#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

#include "stm32f4xx_hal.h"
#include "sensor_service.h"

extern UART_HandleTypeDef huart1;

#ifdef DEBUG
int myprintf(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    char temp[255];
    int len;
    // Limit the length of string to 254
    len = vsnprintf(temp, 254, format, arg);
    usart_puts(temp, len);
    return len;
}
    
int usart_puts(const char *str, int len) 
{
    //putc(*str ++);
    //while (huart1.Lock == HAL_LOCKED);
    HAL_UART_Transmit(&huart1, (uint8_t *)str, len, 1000);
    return 0;
}

static int usart_puts2(const char *str, int len)
{
	const char *strp = str;

	while (len > 0) {
		char *lf = memchr(strp, '\n', len);
		if (lf == NULL) {
			return usart_puts(strp, len);
		}
		else if (lf == strp) {
			usart_puts("\r\n", 2);
			len--;
			strp++;
		}
		else if (*(lf - 1) == '\r') {
			int count = lf - strp + 1;
			usart_puts(strp, count);
			len -= count;
			strp = lf + 1;
		}
		else {
			int count = lf - strp;
			usart_puts(strp, count);
			len -= count;
			usart_puts("\r\n", 2);
			len--;
			strp = lf + 1;
		}
	}
	return 0;
 }

int _write(int fd, const char *str, int len)
{
	extern uint32_t ConnectionBleStatus;

	switch (fd) {
	case 2:
		if (W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)) {
			Stderr_Update((uint8_t *)str, len);
		}
		break;
	case 1:
	default:
		if (W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
			Term_Update((uint8_t *)str, len);
		}
		break;
	}
	usart_puts2(str, len);
	return len;
}

void initDebug(void)
{
	static char stdinbuf[BUFSIZ];
	static char stderrbuf[BUFSIZ];

	memset(stdinbuf, 0, sizeof(stdinbuf));
	memset(stderrbuf, 0, sizeof(stderrbuf));
	setbuf(stdin, stdinbuf);
	fflush(stdin);
	setbuf(stderr, stderrbuf);
	fflush(stderr);
}

#endif
