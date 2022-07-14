// ================================================================================
// LIBRARIES
// ================================================================================

#include <stdio.h>

#include "MKL46Z4.h"
#include "clock_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

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
// CONTROL GLOBAL VARIABLES
// ================================================================================

int32_t pwm = 0;

typedef enum {
	INCREASE,
	DECREASE
} orientation;

orientation pwm_orientation;

xSemaphoreHandle buttonSemaphore;

pin_handler_t relay;

pin_handler_t sw1;

pin_handler_t sw2;

// ================================================================================
// ADC AND SENSOR CONTROL
// ================================================================================

#define SENSOR_QUANTITY 2


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
// SYSTEM FUNCTION PROTOTYPES
// ================================================================================

void sysComponentsInit(void);

void sysStartupScreen(void);

void sysLcdInit(void);

void sysAdcInit(void);

void sysPwmInit(void);

void sysButtonsInit(void);

void sysRelayInit(void);

// ================================================================================
// TASKS PROTOTYPES
// ================================================================================

void vTaskHumidity(void *pvParameters);

void vTaskTemperature(void *pvParameters);

void vTaskLcd(void *pvParameters);

void vTaskControllLed(void *pvParameters);

// ================================================================================
// PWM CONTROL BUTTONS ISR
// ================================================================================

void PORTA_IRQHandler() {
	delay_ms(30);

	UBaseType_t uxSavedInterruptStatus;

	if(portCheckInterrupt(&sw1)) {
		uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
		pwm_orientation = INCREASE;
		taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	}

	if(portCheckInterrupt(&sw2)) {
		uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
		pwm_orientation = DECREASE;
		taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	}

	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

}

// ================================================================================
// MAIN CODE
// ================================================================================

int main(void) {

	sysComponentsInit();
	
	sysStartupScreen();

	QueueHandle_t sensor_queue = xQueueCreate(SENSOR_QUANTITY, sizeof(sensor_handle_t));
	if(buttonSemaphore != NULL) {

		xTaskCreate(vTaskTemperature,
				"vTaskTemperature",
				configMINIMAL_STACK_SIZE,
				sensor_queue,
				1,
				&temperature_task);

		xTaskCreate(vTaskHumidity,
					"vTaskHumidity",
					configMINIMAL_STACK_SIZE,
					sensor_queue,
					1,
					&humidity_task);

		xTaskCreate(vTaskLcd,
				"vTaskLcd",
				configMINIMAL_STACK_SIZE * 4,
				sensor_queue,
				1,
				&lcd_task);

		xTaskCreate(vTaskControllLed,
				"TaskPWM",
				configMINIMAL_STACK_SIZE,
				NULL,
				1,
				NULL);

		vTaskStartScheduler();

		while(1) {}
	}
	return 0;
}

// ================================================================================
// TASKS IMPLEMENTATION
// ================================================================================

void vTaskTemperature(void *pvParameters) {
	QueueHandle_t sensor_queue = (QueueHandle_t) pvParameters;

	long temperature;
	sensor_handle_t sensor;
	sensor.id = sensorTEMPERATURE;

	while(1) {
		while(!adcCalibration());

		temperature = (adcReadInput(temperature_sensor.channel)*330.0)/65536.0;

		if(temperature > 30) {
			gpioPinWrite(&relay, gpioHIGH);
		}
		else{
			gpioPinWrite(&relay, gpioLOW);
		}

		sensor.value = (int) temperature;
		xQueueSendToBack(sensor_queue, &sensor, portMAX_DELAY);

		vTaskDelay(100/portTICK_RATE_MS);
	}
}

void vTaskHumidity(void *pvParameters) {
	QueueHandle_t sensor_queue = (QueueHandle_t) pvParameters;

	sensor_handle_t sensor;
	sensor.id = sensorHUMIDITY;

	while(1) {
		sensor.value = (int) (100 - (adcReadInput(humidity_sensor.channel)*100.0)/65536.0);
		xQueueSendToBack(sensor_queue, &sensor, portMAX_DELAY);
		vTaskDelay(100/portTICK_RATE_MS);
	}
}

void vTaskLcd(void *pvParameters) {
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
				PRINTF("TEMPERATURE: %d\n\r", sensors[i].value);
				lcdClearDisplay(&lcd);
				lcdSetCursor(&lcd, 0, 0);
				lcdWriteString(&lcd, "Temp. : ");
				lcdWriteString(&lcd, sensor_string);
				lcdWriteChar(&lcd, lcdDEGREE_CHAR);
				lcdWriteString(&lcd, "C");
				break;
			}
			delay_ms(250);
		}
		vTaskDelay(500/portTICK_RATE_MS);
	}
}

void vTaskControllLed(void *pvParameters){
	while(1){
		xSemaphoreTake(buttonSemaphore, portMAX_DELAY);

		taskENTER_CRITICAL();

		if(pwm_orientation == INCREASE) {
			pwm += 81;
			if(pwm > 819) {
				pwm = 819;
			}
			TPM2->CONTROLS[0].CnV = pwm;
		}
		else {
			pwm -= 81;
			if(pwm < 0) {
				pwm = 0;
			}
			TPM2->CONTROLS[0].CnV = pwm;
		}

		taskEXIT_CRITICAL();

		vTaskDelay(100/portTICK_RATE_MS);
	}
}

// ================================================================================
// SYSTEM FUNCTION IMPLEMENTATION
// ================================================================================

void sysStartupScreen(void) {
	lcdClearDisplay(&lcd);
	lcdSetCursor(&lcd, 0, 0);
	lcdWriteString(&lcd, "   Controle de");
	lcdSetCursor(&lcd, 1, 0);
	lcdWriteString(&lcd, "   Estufa v1.5");
	delay_ms(1000);
}

void sysComponentsInit(void) {
	sysLcdInit();
	sysAdcInit();
	sysPwmInit();
	sysButtonsInit();
	sysRelayInit();
}

void sysButtonsInit(void) {
	sw1.port = pinPORT_A;
	sw1.pin = 5;
	gpioPinInit(&sw1, gpioINPUT);

	sw2.port = pinPORT_A;
	sw2.pin = 12;
	gpioPinInit(&sw2, gpioINPUT);

	portConfigInterrupt(&sw1, portINT_RISING_EDGE);
	portConfigInterrupt(&sw2, portINT_RISING_EDGE);
}

void sysRelayInit(void) {
	relay.port = pinPORT_E;
	relay.pin = 31;
	gpioPinInit(&relay, gpioOUTPUT);
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

void sysPwmInit(void) {
	// Create Semaphore
	vSemaphoreCreateBinary(buttonSemaphore);
	xSemaphoreTake(buttonSemaphore, 0);

	pwm_orientation = INCREASE;

	SIM->SCGC5 |= (1 << 10);

	PORTB->PCR[2] |= (0b011 << 8);
	
	GPIOB->PDDR |= (1 << 2);

	SIM->SCGC6 |= (1 << 26);

	SIM->SOPT2 &= ~(1 << 16);

	SIM->SOPT2 |= (0b01 << 24);

	TPM2->MOD = 819;

	TPM2->SC |= (0b111 << 0);

	TPM2->CONTROLS[0].CnSC = 0;

	TPM2->CONTROLS[0].CnSC |= (0b10 << 4) | (0b10 << 2);
	
	TPM2->SC |= (0b01 << 3);
}
