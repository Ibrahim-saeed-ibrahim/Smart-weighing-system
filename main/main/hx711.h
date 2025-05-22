/*
 * hx711.h
 *
 * Created: 5/20/2025 1:41:56 PM
 *  Author: Zeyad Tarek
 */ 


#ifndef HX711_H_
#define HX711_H_

#include <avr/io.h>
#include <util/delay.h>

#define HX711_DT_PIN   PIND
#define HX711_DT_DDR   DDRD
#define HX711_DT_PORT  PORTD
#define HX711_DT       PD2

#define HX711_SCK_DDR  DDRD
#define HX711_SCK_PORT PORTD
#define HX711_SCK      PD3

void HX711_init(void);
int32_t HX711_read_raw(void);
float HX711_get_weight(void);
void HX711_tare(void);
void HX711_set_scale(float scale);

#endif