//#ifndef SYSTEM_H
//#define SYSTEM_H
//
//// ================================================================================
//// FUNCTION PROTOTYPES
//// ================================================================================
//
//void sysComponentsInit(void);
//
//void sysStartupScreen(void);
//
//void sysLcdInit(void);
//
//void sysAdcInit(void);
//
//void sysPwmInit(void);
//
//// ================================================================================
//// ADC AND SENSOR CONTROL
//// ================================================================================
//
//#define SENSOR_QUANTITY 2
//
//typedef enum {
//	sensorHUMIDITY,
//	sensorTEMPERATURE
//} sensor_id;
//
//typedef struct {
//	sensor_id id;
//	uint32_t value;
//} sensor_handle_t;
//
//static adc_config_t temperature_sensor;
//
//static adc_config_t humidity_sensor;
//
//// ================================================================================
//// LCD CONTROL
//// ================================================================================
//
//#define lcdDEGREE_CHAR 0x00
//
//const static uint8_t lcd_degree_char[] = {
//											0b00110,
//											0b01001,
//											0b00110,
//											0b00000,
//											0b00000,
//											0b00000,
//											0b00000,
//											0b00000
//										};
//
//static lcd_handler_t lcd;
//
//#endif
