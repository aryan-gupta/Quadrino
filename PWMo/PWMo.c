


//#include "FlightCtrl.h"
//#include "PWMo.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>


void main() {
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_TIM1);
	gpio_set_mode(
		GPIOA,
		GPIO_MODE_OUTPUT_50_MHZ, 
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, 
		GPIO8 | GPIO9 | GPIO10 | GPIO11
	);
	timer_set_mode(
		TIM1, 
		TIM_CR1_CKD_CK_INT, 
		TIM_CR1_CMS_EDGE, 
		TIM_CR1_DIR_DOWN
	);
	timer_set_oc_mode(
		TIM1, 
		TIM_OC1 | TIM_OC2 | TIM_OC3 | TIM_OC4,  
		TIM_OCM_TOGGLE
	);
	timer_enable_oc_output(TIM1, TIM_OC1 | TIM_OC2 | TIM_OC3 | TIM_OC4);
	timer_enable_break_main_output(TIM1);
	timer_set_oc_value(TIM1, TIM_OC1, 200);
	timer_set_oc_value(TIM1, TIM_OC2, 200);
	timer_set_oc_value(TIM1, TIM_OC3, 200);
	timer_set_oc_value(TIM1, TIM_OC4, 200);
	timer_set_period(TIM1, 1000);
	timer_enable_counter(TIM1);

	while (true);
}