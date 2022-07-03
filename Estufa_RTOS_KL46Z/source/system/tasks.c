//#include <stdio.h>
//
//#include "MKL46Z4.h"
//
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//
//#include "kl46z/lcd.h"
//#include "kl46z/adc.h"
//#include "kl46z/delay.h"
//#include "kl46z/gpio.h"
//
//#include "system/system.h"
//#include "system/tasks.h"
//
//// ================================================================================
//// TASKS IMPLEMENTATION
//// ================================================================================
//
//void taskTemperature(void *pvParameters) {
//	QueueHandle_t sensor_queue = (QueueHandle_t) pvParameters;
//
//	sensor_handle_t sensor;
//	sensor.id = sensorTEMPERATURE;
//
//	while(1) {
//		sensor.value = (int) ((adcReadInput(temperature_sensor.channel)*330.0)/65536.0);
//		PORTE->PCR[31] |= (1 << 8);
//		GPIOE->PDDR |= (1 << 31);
//		if(sensor.value > 40) {
//			GPIOE->PSOR |= (1 << 31);
//		} else{
//			GPIOE->PCOR |= (1 << 31);
//		}
//		xQueueSendToBack(sensor_queue, &sensor, portMAX_DELAY);
//		vTaskDelay(100/portTICK_RATE_MS);
//	}
//}
//
//void taskHumidity(void *pvParameters) {
//	QueueHandle_t sensor_queue = (QueueHandle_t) pvParameters;
//
//	sensor_handle_t sensor;
//	sensor.id = sensorHUMIDITY;
//
//	while(1) {
//		sensor.value = (int) (100 - (adcReadInput(humidity_sensor.channel)*100.0)/65536.0);
//		xQueueSendToBack(sensor_queue, &sensor, portMAX_DELAY);
//		vTaskDelay(100/portTICK_RATE_MS);
//	}
//}
//
//void taskLcd(void *pvParameters) {
//	QueueHandle_t sensor_queue = (QueueHandle_t) pvParameters;
//
//	sensor_handle_t sensors[SENSOR_QUANTITY];
//
//	char sensor_string[10];
//
//	while(1) {
//		xQueueReceive(sensor_queue, &sensors[0], portMAX_DELAY);
//		xQueueReceive(sensor_queue, &sensors[1], portMAX_DELAY);
//
//		for(int i = 0; i < SENSOR_QUANTITY; i++) {
//			itoa(sensors[i].value, sensor_string, 10);
//
//			switch(sensors[i].id) {
//				case sensorHUMIDITY:
//					lcdClearDisplay(&lcd);
//					lcdSetCursor(&lcd, 0, 0);
//					lcdWriteString(&lcd, "Umidade: ");
//					lcdWriteString(&lcd, sensor_string);
//					lcdWriteString(&lcd, "%");
//					break;
//				case sensorTEMPERATURE:
//					lcdClearDisplay(&lcd);
//					lcdSetCursor(&lcd, 0, 0);
//					lcdWriteString(&lcd, "Temp. : ");
//					lcdWriteString(&lcd, sensor_string);
//					lcdWriteChar(&lcd, lcdDEGREE_CHAR);
//					lcdWriteString(&lcd, "C");
//					break;
//			}
//			delay_ms(500);
//		}
//		vTaskDelay(1000/portTICK_RATE_MS);
//	}
//}
//
//void taskMenu(void *pvParameters) {
//	while(1);
//}
