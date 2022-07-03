#include <kl46z/delay.h>

/**!
 * @brief Makes the CPU wait for a given time in
 * 		  milliseconds (occupies the CPU during
 * 		  that time).
 * @parameters The delay time in milliseconds.
 */
void delay_ms(uint32_t ms) {
	for(int i = 0; i < ms; i++) {
		for(int j = 0; j < 7000; j++);
	}
}

/**!
 * @brief Makes the CPU wait for a given time in
 * 		  microseconds (occupies the CPU during
 * 		  that time).
 * @parameters The delay time in microseconds.
 */
void delay_us(uint32_t us) {

	for(int i = 0; i < us; i++) {
		for(int j = 0; j < 7; j++);
	}
}
