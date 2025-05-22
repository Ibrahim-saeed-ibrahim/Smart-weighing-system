/*
 * hx711.c
 *
 * Created: 5/20/2025 1:42:12 PM
 *  Author: Zeyad Tarek
 */

#include "hx711.h"
#include <util/delay.h>

static long offset = 0;
static float scale = 1.0;

void HX711_init(void) {
    HX711_DT_DDR &= ~(1 << HX711_DT);     // DT ?????
    HX711_SCK_DDR |= (1 << HX711_SCK);    // SCK ?????
    HX711_SCK_PORT &= ~(1 << HX711_SCK);  // ???? ?? SCK = 0
}

int32_t HX711_read_raw(void) {
    int32_t count = 0;

    // ????? ??? ???? DT = LOW (???? ???????)
    while (HX711_DT_PIN & (1 << HX711_DT));

    // ????? 24 ?? ?? HX711
    for (uint8_t i = 0; i < 24; i++) {
        HX711_SCK_PORT |= (1 << HX711_SCK);
        _delay_us(1);

        count = count << 1;
        if (HX711_DT_PIN & (1 << HX711_DT)) {
            count++;
        }

        HX711_SCK_PORT &= ~(1 << HX711_SCK);
        _delay_us(1);
    }

    // ???? 25 ?????? ????? (gain = 128)
    HX711_SCK_PORT |= (1 << HX711_SCK);
    _delay_us(1);
    HX711_SCK_PORT &= ~(1 << HX711_SCK);
    _delay_us(1);

    // ??????? ?? ?????? ??????? (sign extension)
    if (count & 0x800000) {
        count |= 0xFF000000; // ?? ???? 24 = 1 ???? ???? -> ??? 1s
    }

    return count;
}

float HX711_get_weight(void) {
    int32_t reading = HX711_read_raw();
    return (reading - offset) / scale;
}

void HX711_tare(void) {
    offset = HX711_read_raw();  // ????? ????? ?????? = 0
}

void HX711_set_scale(float s) {
    scale = s;
}
