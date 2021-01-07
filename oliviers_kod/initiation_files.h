/*
 * initiation_files.h
 *
 * Created: 2020-12-19 16:45:34
 *  Author: kadeko
 */ 


#ifndef INITIATION_FILES_H_
#define INITIATION_FILES_H_

void spi_init(void);
void uart_init(uint8_t baud_r);
uint16_t adc_read(uint8_t val);




#endif /* INITIATION_FILES_H_ */