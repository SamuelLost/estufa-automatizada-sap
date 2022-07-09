// ================================================================================
// LIBRARIES
// ================================================================================

#include <stdio.h>

#include "MKL46Z4.h"
#include "clock_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#include "kl46z/lcd.h"
#include "kl46z/adc.h"
#include "kl46z/delay.h"
#include "kl46z/gpio.h"

// ================================================================================
// ADC AND SENSOR CONTROL
// ================================================================================

#define SENSOR_QUANTITY 2
#define PTB2_PIN 		2
typedef enum {
	sensorHUMIDITY,
	sensorTEMPERATURE
} sensor_id;

typedef struct {
	sensor_id id;
	uint32_t value;
} sensor_handle_t;

adc_config_t temperature_sensor;

adc_config_t humidity_sensor;

// ================================================================================
// LCD CONTROL
// ================================================================================

#define lcdDEGREE_CHAR 0x00

const uint8_t lcd_degree_char[] = {
		0b00110,
		0b01001,
		0b00110,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000
};

lcd_handler_t lcd;

// ================================================================================
// TASKS HANDLERS
// ================================================================================

TaskHandle_t lcd_task;

TaskHandle_t humidity_task;

TaskHandle_t temperature_task;

TaskHandle_t menu_task;

// ================================================================================
// FUNCTION PROTOTYPES
// ================================================================================

void sysComponentsInit(void);

void sysStartupScreen(void);

void sysLcdInit(void);

void sysAdcInit(void);

void sysPwmInit(void);

// ================================================================================
// TASKS PROTOTYPES
// ================================================================================

void taskMenu(void *pvParameters);

void taskHumidity(void *pvParameters);

void taskTemperature(void *pvParameters);

void taskLcd(void *pvParameters);

void taskControlLed(void *pvParameters);
// ================================================================================
// MAIN CODE
// ================================================================================
void ADC0_init(void) {
	SIM->SCGC5 |= (1 << 10); /* clock to PORTE */
	PORTB->PCR[1] |= 0; /* PTB1 analog input */

	SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */
	ADC0->SC2 &= ~0x40; /* software trigger */
	ADC0->SC3 |= 0x07; /* 32 samples average */
	/* clock div by 4, long sample time, single ended 16 bit, bus clock */
	ADC0->CFG1 = 0x40 | 0x10 | 0x0C | 0x00;
}
int main(void) {

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	    /* Init FSL debug console. */
	    BOARD_InitDebugConsole();
	#endif
	sysComponentsInit();
	sysStartupScreen();
//	sysAdcInit();
//	sysPwmInit();
	QueueHandle_t sensor_queue = xQueueCreate(SENSOR_QUANTITY, sizeof(sensor_handle_t));
	void *pvParameters;
//	taskControlLed(pvParameters);
//	taskTemperature(sensor_queue);
	xTaskCreate(taskTemperature,
			"TaskTemperature",
			configMINIMAL_STACK_SIZE,
			sensor_queue,
			1,
			&temperature_task);

//	xTaskCreate(taskHumidity,
//			"TaskHumidity",
//			configMINIMAL_STACK_SIZE,
//			sensor_queue,
//			1,
//			&humidity_task);
//
	xTaskCreate(taskLcd,
			"TaskLcd",
			configMINIMAL_STACK_SIZE * 4,
			sensor_queue,
			1,
			&lcd_task);

//	xTaskCreate(taskMenu,
//			"TaskMenu",
//			configMINIMAL_STACK_SIZE,
//			(void *) sensor_queue,
//			1,
//			&menu_task);

	xTaskCreate(taskControlLed,
			"TaskPWM",
			configMINIMAL_STACK_SIZE,
			NULL,
			1,
			NULL);
//
	vTaskStartScheduler();
	while(1) {
//		ADC0->SC1[0] = 9; /* start conversion on channel 0 */
//		while(!(ADC0->SC1[0] & 0x80)) { } /* wait for COCO */
//		result = ADC0->R[0]; /* read conversion result and clear COCO flag */
//		//		temperature = result * 330.0 / 65536; /* convert voltage to temperature */
//		//		printf("\r\nTemp = %6.2dC", temperature); /* convert to string */
//
//		float temperature = result * 330.0 / 65536;
//
//		PRINTF("Temperatura %d C\r\n", (int) temperature);
	}
}

// ================================================================================
// TASKS IMPLEMENTATION
// ================================================================================

void taskTemperature(void *pvParameters) {
	QueueHandle_t sensor_queue = (QueueHandle_t) pvParameters;

	sensor_handle_t sensor;
	sensor.id = sensorTEMPERATURE;
	SIM->SCGC5 |= (1 << 13);
	PORTE->PCR[31] |= (1 << 8);
	GPIOE->PDDR |= (1 << 31);
	while(1) {
		bool flag = false;
//		ADC0->SC1[0] = 9; /* start conversion on channel 0 */
		do {
			flag = adcCalibration();
		} while(flag == false);
		float temperature = (int) ((adcReadInput(temperature_sensor.channel)*330.0)/65536.0);
//		while(!(ADC0->SC1[0] & 0x80)) { } /* wait for COCO */
//		result = ADC0->R[0]; /* read conversion result and clear COCO flag */
		//		temperature = result * 330.0 / 65536; /* convert voltage to temperature */
		//		printf("\r\nTemp = %6.2dC", temperature); /* convert to string */

//		float temperature = result * 330.0 / 65536;
		if((int) temperature > 30) {
			GPIOE->PSOR |= (1 << 31);
//			PRINTF("Cooler ligado!\n\r");
//			delay_ms(500);
		} else{
			GPIOE->PCOR |= (1 << 31);
//			PRINTF("Cooler desligado!\n\r");
//			delay_ms(500);
		}
		sensor.value = (int) temperature;
		xQueueSendToBack(sensor_queue, &sensor, portMAX_DELAY);
//		PRINTF("Temperatura %d ºC\r\n", (int) temperature);
//		delay_ms(1000);
		vTaskDelay(100/portTICK_RATE_MS);
//		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

void taskHumidity(void *pvParameters) {
	QueueHandle_t sensor_queue = (QueueHandle_t) pvParameters;

	sensor_handle_t sensor;
	sensor.id = sensorHUMIDITY;

	while(1) {
		sensor.value = (int) (100 - (adcReadInput(humidity_sensor.channel)*100.0)/65536.0);
//		sensor.value = (int) adcReadInput(humidity_sensor.channel);
		PRINTF("Umidade: %d%%\r\n", sensor.value);
		xQueueSendToBack(sensor_queue, &sensor, portMAX_DELAY);
		vTaskDelay(100/portTICK_RATE_MS);
	}
}

void taskLcd(void *pvParameters) {
	QueueHandle_t sensor_queue = (QueueHandle_t) pvParameters;

	sensor_handle_t sensors[SENSOR_QUANTITY];

	char sensor_string[10];

	while(1) {
		xQueueReceive(sensor_queue, &sensors[0], portMAX_DELAY);
		xQueueReceive(sensor_queue, &sensors[1], portMAX_DELAY);

		for(int i = 0; i < SENSOR_QUANTITY; i++) {
			itoa(sensors[i].value, sensor_string, 10);

			switch(sensors[i].id) {
				case sensorHUMIDITY:
					lcdClearDisplay(&lcd);
					lcdSetCursor(&lcd, 0, 0);
					lcdWriteString(&lcd, "Umidade: ");
					lcdWriteString(&lcd, sensor_string);
					lcdWriteString(&lcd, "%");
					break;
				case sensorTEMPERATURE:
					lcdClearDisplay(&lcd);
					lcdSetCursor(&lcd, 0, 0);
					lcdWriteString(&lcd, "Temp. : ");
					lcdWriteString(&lcd, sensor_string);
					lcdWriteChar(&lcd, lcdDEGREE_CHAR);
					lcdWriteString(&lcd, "C");
					PRINTF("Temperatura: %d\n\r", sensors[i].value);
					break;
			}
			delay_ms(500);
		}
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

void taskMenu(void *pvParameters) {
	while(1);
}

// ================================================================================
// FUNCTION IMPLEMENTATION
// ================================================================================

void sysStartupScreen(void) {
	lcdClearDisplay(&lcd);
	lcdSetCursor(&lcd, 0, 0);
	lcdWriteString(&lcd, "   Controle de");
	lcdSetCursor(&lcd, 1, 0);
	lcdWriteString(&lcd, "   Estufa v1.1");
	delay_ms(1000);
}

void sysComponentsInit(void) {
	sysLcdInit();
	sysAdcInit();
	sysPwmInit();
}

void sysLcdInit(void) {
	pin_handler_t rs;
	rs.port = pinPORT_E;
	rs.pin = 1;
	gpioPinInit(&rs, gpioOUTPUT);

	pin_handler_t en;
	en.port = pinPORT_E;
	en.pin = 0;
	gpioPinInit(&en, gpioOUTPUT);

	pin_handler_t d4;
	d4.port = pinPORT_E;
	d4.pin = 22;
	gpioPinInit(&d4, gpioOUTPUT);

	pin_handler_t d5;
	d5.port = pinPORT_E;
	d5.pin = 23;
	gpioPinInit(&d5, gpioOUTPUT);

	pin_handler_t d6;
	d6.port = pinPORT_B;
	d6.pin = 20;
	gpioPinInit(&d6, gpioOUTPUT);

	pin_handler_t d7;
	d7.port = pinPORT_E;
	d7.pin = 30;
	gpioPinInit(&d7, gpioOUTPUT);

	lcd.data[0] = d4;
	lcd.data[1] = d5;
	lcd.data[2] = d6;
	lcd.data[3] = d7;
	lcd.rs = rs;
	lcd.en = en;

	lcdInitModule(&lcd);

	lcdCreateChar(&lcd, lcdCUSTOM_CHAR_1, lcd_degree_char);
}

void sysAdcInit(void) {
	portSetPinMux(pinPORT_E, 20, pinMUX_ALT_0);
	adcGetDefaultConfig(&humidity_sensor);
	humidity_sensor.channel = 0;
	adcInitModule(&humidity_sensor);

	portSetPinMux(pinPORT_B, 1, pinMUX_ALT_0);
	adcGetDefaultConfig(&temperature_sensor);
	temperature_sensor.channel = 9;
	adcInitModule(&temperature_sensor);

	while(!adcCalibration());
}

void taskControlLed(void *pvParameters){
	while(1){
		TPM2->CONTROLS[0].CnV = 655;
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void sysPwmInit(void) {
	SIM->SCGC5 |= (1 << 10); // Ativar clock porta B

	PORTB->PCR[PTB2_PIN] |= (1 << 24) |		// ISF=PORTB_PCR18[24]: w1c (limpa a pendência)
							(0b011 << 8);   // MUX=PORTB_PCR18[10:8]=0b011 (TPM2_CH0)
	GPIOB->PDDR |= (1 << 2);

	/*
	* 	System Clock Gating Control Register 6 (SIM_SCGC6)
	*|   31 | 30 |  29 | 28 |  27  |  26  |  25  |  24  |  23 | 22 -- 2 |    1   |  0  |
	*| DAC0 |  0 | RTC |  0 | ADC0 | TPM2 | TPM1 | TPM0 | PIT |    0    | DMAMUX | FTF |
	*/
	SIM->SCGC6 |= (1 << 26); // Habilitando o clock do TPM0

	SIM->SOPT2 &= ~(1 << 16);	//0 MCGFLLCLK clock

	SIM->SOPT2 |= (0b01 << 24);

	TPM2->MOD = 819;
//	TPM2_REG->mod = 819;

	TPM2->SC = 	(0 << 5)	|			// CPWMS=TPM2_SC[5]=0 (modo de contagem crescente)
					(0b01 << 3)	|           // CMOD=TPM2_SC[4:3]=0b01 (incrementa a cada pulso do LPTPM)
					(0b111 << 0);


	TPM2->CONTROLS[0].CnSC = (0 << 5) | // MSB =TPM2_C0SC[5]=0
								 (0 << 4) | // MSA =TPM2_C0SC[4]=0
								 (0 << 3) | // ELSB=TPM2_C0SC[3]=0
								 (0 << 2);  // ELSA=TPM2_C0SC[2]=0

	TPM2->CONTROLS[0].CnSC = (1 << 5) | // MSB =TPM2_C0SC[5]=0
								 (0 << 4) | // MSA =TPM2_C0SC[4]=0
								 (1 << 3) | // ELSB=TPM2_C0SC[3]=0
								 (0 << 2);  // ELSA=TPM2_C0SC[2]=0
}
//
//200Hz tem modulo de 819
//
//Para 6:00
//10% de duty cycle tem modulo de 81
//
//Para 8:00
//20% de duty cycle tem modulo de 163
//
//Para 9:00
//40% de duty cycle tem modulo de 327
//
//Para 10:00
//50% de duty cycle tem modulo de 409
//
//Para 11:00
//60% de duty cycle tem modulo de 491
//
//Para 12:00
//70% duty cycle tem modulo de 573
//
//Para 12:30
//90% de duty cycle tem modulo de 737
//
//Para 13:00
//80% de duty cycle tem modulo de 655
//
//Para 14:00
//60% de duty cycle tem modulo de 491
//
//Para 15:00
//50% de duty cycle tem modulo de 409
//
//Para 16:00
//40% de duty cycle tem modulo de 327
//
//Para 17:00
//20% de duty cycle tem modulo de 163
//
//Para 17:30
//5% de duty cycle tem modulo de 40
//
//Para 18:00
//0% de duty cycle tem modulo de 0
