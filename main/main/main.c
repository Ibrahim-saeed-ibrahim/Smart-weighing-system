#define F_CPU 16000000UL  // for 16 MHz

#include "std_types.h"
#include "common_macros.h"
#include "lcd.h"
#include "gpio.h"
#include "keypad.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "hx711.h"
#include <stdlib.h>

#define enterpass1 0
#define ConfirmPass 1
#define mainmenu 2
#define PriceView 3
#define buy 4
#define resetpass 5
#define CALIBRATION_FACTOR 228.0

uint8 pass_length = 1;
uint8 key = 0;
// uint32 weight = 100;
uint8 password[1];
uint8 error = 0;
uint8 password2[1];
uint8 menuSelection = 0;
uint32 totalprice = 0;
char weightStr[10];

float weightcalc() {
	uint8 pressed = 0;
	float weight;
	while (pressed != 1) {
		weight = HX711_get_weight();
		LCD_moveCursor(0, 0);
		LCD_displayString("Weight:");
		dtostrf(weight, 6, 2, weightStr);
		LCD_displayString(weightStr);
		LCD_displayString(" Gm");
		pressed = KEYPAD_getPressedKey();
		_delay_ms(1000);
	}
	return weight;
}

uint8 enterpassword1(void) {
	if (error == 1) {
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 5, "Unmatched");
		_delay_ms(2000);
	}
	uint8 button;

	LCD_clearScreen();
	LCD_displayString("Plz enter pass:");
	LCD_moveCursor(1, 0);

	for (uint8 j = 0; j <= pass_length; j++) {
		button = KEYPAD_getPressedKey();
		_delay_ms(250);
		if (j == pass_length) {
			while (button != '=') {
				button = KEYPAD_getPressedKey();
				_delay_ms(250);
			}
		}
		password[j] = button;
		LCD_displayCharacter('*');
	}
	return ConfirmPass;
}

void enterpassword2(void) {
	uint8 button;

	LCD_clearScreen();
	LCD_displayString("Plz re-enter the");
	LCD_displayStringRowColumn(1, 0, "same pass: ");

	for (uint8 j = 0; j <= pass_length; j++) {
		button = KEYPAD_getPressedKey();
		_delay_ms(250);

		if (j == pass_length) {
			while (button != '=') {
				button = KEYPAD_getPressedKey();
				_delay_ms(250);
			}
		}
		password2[j] = button;
		LCD_displayCharacter('*');
	}
}

uint8 checkmatch(void) {
	error = 0;

	for (uint8 j = 0; j < pass_length; j++) {
		if (password2[j] != password[j]) {
			error = 1;
			return enterpass1;
		}
	}
	return mainmenu;
}

int main(void) {
	LCD_init();
	HX711_init();

	while (1) {
		switch (key) {
			case enterpass1:
			key = enterpassword1();
			break;

			case ConfirmPass:
			enterpassword2();
			key = checkmatch();
			LCD_clearScreen();
			break;

			case mainmenu:
			menuSelection = 0;
			LCD_displayString(" 1-Reset 2-Prices");
			LCD_displayStringRowColumn(1, 0, " 3-buy");

			if (menuSelection != 1 && menuSelection != 2 && menuSelection != 3) {
				menuSelection = KEYPAD_getPressedKey();
			}

			if (menuSelection == 1) {
				key = resetpass;
				} else if (menuSelection == 2) {
				key = PriceView;
				} else if (menuSelection == 3) {
				key = buy;
			}

			break;

			case PriceView: {
				LCD_clearScreen();
				LCD_displayString("Select: ");
				LCD_displayStringRowColumn(1, 0, "1-Fruits 2-Food");

				uint8 choice = KEYPAD_getPressedKey();
				while (choice != 1 && choice != 2) {
					choice = KEYPAD_getPressedKey();
				}

				if (choice == 1) {
					LCD_clearScreen();
					LCD_displayString(" Apples 15 egp");
					LCD_displayStringRowColumn(1, 0, " Banana 20 egp");
					_delay_ms(1500);
					} else if (choice == 2) {
					LCD_clearScreen();
					LCD_displayString(" Potato 10 egp");
					LCD_displayStringRowColumn(1, 0, " Carrot 30 egp");
					_delay_ms(1500);
				}
				key = mainmenu;
				LCD_clearScreen();
				break;
			}

			case buy: {
				LCD_clearScreen();
				uint16 selection = 0;
				uint16 appleprice = 15;
				uint16 bannanaprice = 20;
				uint16 potatoprice = 10;
				uint16 carrotprice = 30;
				float weight = 0;  // Changed to float

				LCD_displayString(" 1-Fruits");
				LCD_displayStringRowColumn(1, 0, " 2-Food");
				while (selection != 1 && selection != 2) {
					selection = KEYPAD_getPressedKey();
				}
				LCD_clearScreen();

				if (selection == 1) {
					selection = 0;
					LCD_displayString(" 1-Apples");
					LCD_displayStringRowColumn(1, 0, " 2-Bannana");
					while (selection != 1 && selection != 2) {
						selection = KEYPAD_getPressedKey();
					}
					if (selection == 1) {
						weight = weightcalc();
						totalprice = totalprice + ((uint32)weight * appleprice);
						} else if (selection == 2) {
						weight = weightcalc();
						totalprice = totalprice + ((uint32)weight * bannanaprice);
					}
					} else if (selection == 2) {
					selection = 0;
					LCD_displayString(" 1-Potato");
					LCD_displayStringRowColumn(1, 0, " 2-Carrots");
					while (selection != 1 && selection != 2) {
						selection = KEYPAD_getPressedKey();
					}
					if (selection == 1) {
						weight = weightcalc();
						totalprice = totalprice + ((uint32)weight * potatoprice);
						} else if (selection == 2) {
						weight = weightcalc();
						totalprice = totalprice + ((uint32)weight * carrotprice);
					}
				}

				selection = 0;
				LCD_clearScreen();
				LCD_displayString(" 1-Add items");
				LCD_displayStringRowColumn(1, 0, " 2-Get bill");
				while (selection != 1 && selection != 2) {
					selection = KEYPAD_getPressedKey();
				}
				if (selection == 1) {
					key = buy;
				}
				if (selection == 2) {
					LCD_clearScreen();
					LCD_displayString(" Total Price=");
					LCD_moveCursor(1, 0);
					LCD_intgerToString(totalprice);
					_delay_ms(3500);
					key = mainmenu;
					totalprice = 0;
				}
				LCD_clearScreen();
				break;
			}

			case resetpass:
			key = 0;
			break;
		}
	}
}
