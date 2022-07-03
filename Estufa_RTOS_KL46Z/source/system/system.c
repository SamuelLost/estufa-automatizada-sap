//#include <stdio.h>
//
//#include "MKL46Z4.h"
//
//#include "kl46z/lcd.h"
//#include "kl46z/adc.h"
//#include "kl46z/delay.h"
//#include "kl46z/gpio.h"
//
//#include "system/system.h"
//
//// ================================================================================
//// FUNCTION IMPLEMENTATION
//// ================================================================================
//
//void sysStartupScreen(void) {
//	lcdClearDisplay(&lcd);
//	lcdSetCursor(&lcd, 0, 0);
//	lcdWriteString(&lcd, "   Controle de");
//	lcdSetCursor(&lcd, 1, 0);
//	lcdWriteString(&lcd, "   Estufa v1.1");
//	delay_ms(1000);
//}
//
//void sysComponentsInit(void) {
//	sysLcdInit();
//	sysAdcInit();
//	sysPwmInit();
//}
//
//void sysLcdInit(void) {
//	pin_handler_t rs;
//	rs.port = pinPORT_E;
//	rs.pin = 1;
//	gpioPinInit(&rs, gpioOUTPUT);
//
//	pin_handler_t en;
//	en.port = pinPORT_E;
//	en.pin = 0;
//	gpioPinInit(&en, gpioOUTPUT);
//
//	pin_handler_t d4;
//	d4.port = pinPORT_E;
//	d4.pin = 22;
//	gpioPinInit(&d4, gpioOUTPUT);
//
//	pin_handler_t d5;
//	d5.port = pinPORT_E;
//	d5.pin = 23;
//	gpioPinInit(&d5, gpioOUTPUT);
//
//	pin_handler_t d6;
//	d6.port = pinPORT_B;
//	d6.pin = 20;
//	gpioPinInit(&d6, gpioOUTPUT);
//
//	pin_handler_t d7;
//	d7.port = pinPORT_E;
//	d7.pin = 30;
//	gpioPinInit(&d7, gpioOUTPUT);
//
//	lcd.data[0] = d4;
//	lcd.data[1] = d5;
//	lcd.data[2] = d6;
//	lcd.data[3] = d7;
//	lcd.rs = rs;
//	lcd.en = en;
//
//	lcdInitModule(&lcd);
//
//	lcdCreateChar(&lcd, lcdCUSTOM_CHAR_1, lcd_degree_char);
//}
//
//void sysAdcInit(void) {
//	portSetPinMux(pinPORT_E, 20, pinMUX_ALT_0);
//	adcGetDefaultConfig(&humidity_sensor);
//	humidity_sensor.channel = 0;
//	adcInitModule(&humidity_sensor);
//
//	portSetPinMux(pinPORT_B, 1, pinMUX_ALT_0);
//	adcGetDefaultConfig(&temperature_sensor);
//	temperature_sensor.channel = 9;
//	adcInitModule(&temperature_sensor);
//
//	while(!adcCalibration());
//}
//
//void sysPwmInit(void) {
//
//}
//
//
