#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_tim.h"

#include "interruptHelper.h"

#include <stdbool.h>

extern bool buffer_state[];
extern bool timer_state[];

extern uint8_t data_c0[3072 * 4]; // 4 Clockless channels of either 96 RGBW or 128 RGB leds
extern uint8_t data_c1[512];  // 1 Clocked channels of 96 LEDS

void pins_init() {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable Timer 2 Clock
	__HAL_RCC_TIM2_CLK_ENABLE()
	;

	// Enable GPIO Port A Clock
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;

	// Common configuration for all channels
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	// Pins for timer 2

	// Apply pin configuration to PA0
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Apply pin configuration to PA1
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Apply pin configuration to PA2
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Apply pin configuration to PA3
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	// Pins for timer 3
	// Apply pin configuration to PA6
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Apply pin configuration to PA7
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Enable GPIO Port B Clock
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	// Apply pin configuration to PB0
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Apply pin configuration to PB1
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}



void pwm_set_xchannels(TIM_TypeDef *tim, int firstChannel, int nrChannels) {
	if (firstChannel + nrChannels <= 4 ) {
		tim->CR1 &= ~0x0001; // disable
		__DSB();
		tim->DCR &= ~TIM_DCR_DBA_Msk;
		tim->DCR |= ((13 + firstChannel) << TIM_DCR_DBA_Pos); // DMA Transfer Base address CCR1 + channel (0-3)
		tim->DCR &= ~TIM_DCR_DBL_Msk;
		tim->DCR |= ((nrChannels - 1) << TIM_DCR_DBL_Pos); // 4 Transfer at a time (CCR1 to CCR4)
		tim->CR1 |= 0x0001; // enable
		__DSB();
	}
}


void pwm_set_4channel(TIM_TypeDef *tim) {
	pwm_set_xchannels(tim,0,4);
}


void pwm_init2(DMA_Channel_TypeDef *dma, TIM_TypeDef *tim) {
	dma->CPAR = (uint32_t) &(tim->DMAR); // Link DMA channel to timer

	dma->CCR = 0x00;
	dma->CCR |= (0x01 << DMA_CCR_PSIZE_Pos); // Peripheral size 16 bit
	dma->CCR |= (0x00 << DMA_CCR_MSIZE_Pos); // Memory size 8 bit
	dma->CCR |= (0x1 << DMA_CCR_DIR_Pos);   // Memory to Peripheral
	dma->CCR |= (0x1 << DMA_CCR_MINC_Pos);   // Memory increasement
	dma->CCR |= (0x0 << DMA_CCR_PINC_Pos);  // Peripheral increasement
	dma->CCR |= (0x0 << DMA_CCR_CIRC_Pos);   // Circular mode
	dma->CCR |= DMA_CCR_TCIE; // Enable transfer complete interrupt

	dma->CCR |= DMA_CCR_TEIE; // Enable transfer error interrupt

	tim->DCR = 0;

	/*
	//  So we need to change this to also allow single channel transfers
	tim->DCR |= ((13) << TIM_DCR_DBA_Pos); // DMA Transfer Base address CCR1
	tim->DCR |= ((3) << TIM_DCR_DBL_Pos); // 4 Transfer at a time (CCR1 to CCR4)
	*/

	// For led strips
	tim->PSC = 9; // Prescaler
	tim->ARR = 8; // Reload Value

	/*
	// Testing for through hole
	tim->PSC = 9; // Prescaler
	tim->ARR = 16; // Reload Value
	*/


	tim->DIER = 0;
	//tim->DIER |= TIM_DIER_UDE; // Update DMA Request Enable

	tim->CCMR1 = 0;
	tim->CCMR1 |= (0b110 << TIM_CCMR1_OC1M_Pos); // pwm mode 1
	tim->CCMR1 |= (0b110 << TIM_CCMR1_OC2M_Pos); // pwm mode 1

	tim->CCMR2 = 0;
	tim->CCMR2 |= (0b110 << TIM_CCMR2_OC3M_Pos); // pwm mode 1
	tim->CCMR2 |= (0b110 << TIM_CCMR2_OC4M_Pos); // pwm mode 1

	tim->CCMR1 |= TIM_CCMR1_OC1PE; //(1 << TIM_CCMR1_OC1PE_Pos); // output compare preload enable
	tim->CCMR1 |= TIM_CCMR1_OC2PE; //(1 << TIM_CCMR1_OC2PE_Pos); // output compare preload enable

	tim->CCMR2 |= TIM_CCMR2_OC3PE; //(1 << TIM_CCMR2_OC3PE_Pos); // output compare preload enable
	tim->CCMR2 |= TIM_CCMR2_OC4PE; //(1 << TIM_CCMR2_OC4PE_Pos); // output compare preload enable

	// This I forgot
	tim->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;



	tim->CR1 |= 1 << 7; // auto reload enable
	tim->CR1 &= ~(0b1110000); // Edge aglined, upcounting
	tim->CR1 |= 0b100; // Event source, only over/underflow
	//tim->CR1 |= 0x0001; // enable

}

void start_dma_transer2(char* memory, size_t size, DMA_Channel_TypeDef *dma,
		TIM_TypeDef *tim) {

	DisableInterrupts;
	buffer_state[0] = true;
	EnableInterrupts;

	switch ((uint32_t) tim) {
	case (uint32_t) TIM2:
		if (timer_state[0]) {
			return; // BUSY // TODO GENERATE ERROR MESSAGE
		} else
			timer_state[0] = true;
		break;
	case (uint32_t) TIM3:
		if (timer_state[1]) {
			return; // BUSY // TODO GENERATE ERROR MESSAGE
		} else
			timer_state[1] = true;
		break;
	case (uint32_t) TIM4:
		if (timer_state[2]) {
			return; // BUSY // TODO GENERATE ERROR MESSAGE
		} else
			timer_state[2] = true;
	}

	tim->DIER = TIM_DIER_UDE; // Update DMA Request Enable
	dma->CNDTR = size;
	dma->CMAR = (uint32_t) memory;
	tim->CCMR1 |= 1; // enable timer
	dma->CCR |= 1; // Enable DMA
	tim->EGR = TIM_EGR_UG; // Trigger update event, starting the transfer
}
void start_dma_transer(char* memory, size_t size) {
	start_dma_transer2(memory, size, DMA1_Channel2, TIM2);
}

void pwm_init() {

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // enable timer2 clock
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; // enable dma1 clock


	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // enable timer3 clock


	pins_init();

	// Datasheet page 281
	// TIM1_UP --> DMA 1 Channel 5
	// TIM2_UP --> DMA 1 Channel 2
	// TIM3_UP --> DMA 1 Channel 3
	// TIM4_UP --> DMA 1 Channel 7
	pwm_init2(DMA1_Channel2, TIM2);
	pwm_init2(DMA1_Channel3, TIM3);
	// We could also add Timer 4, then we have 4 more channels but no I²C
	// For Timer 1, we could add 2 channels, the other channels are in use by USB

	NVIC_ClearPendingIRQ(DMA1_Channel2_IRQn);
	NVIC_EnableIRQ(DMA1_Channel2_IRQn);

	NVIC_ClearPendingIRQ(DMA1_Channel3_IRQn);
	NVIC_EnableIRQ(DMA1_Channel3_IRQn);

	NVIC_ClearPendingIRQ(TIM2_IRQn);
	NVIC_EnableIRQ(TIM2_IRQn);

	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);

	memset(data_c0, 2, sizeof(data_c0));

	pwm_set_4channel(TIM2);
	pwm_set_4channel(TIM3);
	start_dma_transer2(data_c0, sizeof(data_c0), DMA1_Channel2, TIM2);
	start_dma_transer2(data_c0, sizeof(data_c0), DMA1_Channel3, TIM3);

}

bool is_busy2(DMA_Channel_TypeDef *dma) {
	return dma->CCR & 1;
}

bool is_busy() {

	//return is_busy2(DMA1_Channel2) || is_busy2(DMA1_Channel3) ;

	return timer_state[0] || timer_state[1] || buffer_state[0];
}

void DMA1_Channel2_IRQHandler(void) {
	DMA1->IFCR = DMA1->ISR;
	DMA1_Channel2->CCR &= ~1; // Disable DMA

	// Resetting channel state
	TIM2->CCR1 = 0;
	TIM2->CCR2 = 0;
	TIM2->CCR3 = 0;
	TIM2->CCR4 = 0;

	TIM2->RCR = 32; // Keep reset time for 32 bits (RGBW)
	TIM2->DIER = TIM_DIER_UIE; // Update Interrupt Enable

	DisableInterrupts;
	buffer_state[0] = false;
	EnableInterrupts;
}

void DMA1_Channel3_IRQHandler(void) {
	DMA1->IFCR = DMA1->ISR;
	DMA1_Channel3->CCR &= ~1; // Disable DMA

	// Resetting channel state
	TIM3->CCR1 = 0;
	TIM3->CCR2 = 0;
	TIM3->CCR3 = 0;
	TIM3->CCR4 = 0;

	TIM3->RCR = 32; // Keep reset time for 32 bits (RGBW)
	TIM3->DIER = TIM_DIER_UIE; // Update Interrupt Enable

	DisableInterrupts;
	buffer_state[0] = false;
	EnableInterrupts;

}

void DMA1_Channel7_IRQHandler(void) {
	DMA1->IFCR = DMA1->ISR;
	DMA1_Channel7->CCR &= ~1; // Disable DMA

	// Resetting channel state
	TIM4->CCR1 = 0;
	TIM4->CCR2 = 0;
	TIM4->CCR3 = 0;
	TIM4->CCR4 = 0;

	DisableInterrupts;
	buffer_state[0] = false;
	EnableInterrupts;

}

void TIM2_IRQHandler(void) {
	timer_state[0] = false;
	TIM2->DIER = 0;
}

void TIM3_IRQHandler(void) {
	timer_state[1] = false;
	TIM3->DIER = 0;
}

void TIM4_IRQHandler(void) {
	timer_state[2] = false;
	TIM4->DIER = 0;
}
