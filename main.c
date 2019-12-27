#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

extern uint8_t data_c0[];

void voorbeeld();
void SystemClock_Config(void);

int main() {
	HAL_Init(); // Reset of all peripherals, Initializes the Flash interface and the Systick.
	SystemClock_Config(); // Configure the system clock to 72 MHz
	pwm_init();
	voorbeeld();
	while (1);
}

void voorbeeld() {
	//uint8_t b[8*3]; // 8 leds, 3 colours per led
	uint8_t b[32] = { 0 };

	b[1] = 16;
	b[0] = 0;
	b[2] = 0;

	b[4] = 16;
	b[3] = 8;
	b[5] = 0;

	b[7] = 8;
	b[6] = 8;
	b[8] = 0;

	b[10] = 0;
	b[9] = 8;
	b[11] = 0;

	b[13] = 0;
	b[12] = 8;
	b[14] = 16;

	b[16] = 8;
	b[15] = 0;
	b[17] = 8;

	b[19] = 16;
	b[18] = 0;
	b[20] = 16;

	b[22] = 8;
	b[21] = 8;
	b[23] = 8;

	while (is_busy());

	fill_buffer_decompress(0, 24, b);
	pwm_set_xchannels(TIM2, 0, 1);
	start_dma_transer2(data_c0, 8 * 3 * 8, DMA1_Channel2, TIM2);

}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 72000000
 *            HCLK(Hz)                       = 72000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            HSE PREDIV1                    = 1
 *            PLLMUL                         = 9
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_ClkInitTypeDef clkinitstruct = { 0 };
	RCC_OscInitTypeDef oscinitstruct = { 0 };
	RCC_PeriphCLKInitTypeDef rccperiphclkinit = { 0 };

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscinitstruct.HSEState = RCC_HSE_ON;
	oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;

	oscinitstruct.PLL.PLLState = RCC_PLL_ON;
	oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK) {
		/* Start Conversation Error */
		//Error_Handler();
	}

	/* USB clock selection */
	rccperiphclkinit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	rccperiphclkinit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	HAL_RCCEx_PeriphCLKConfig(&rccperiphclkinit);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
	clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK) {
		/* Start Conversation Error */
		//Error_Handler();
	}
}

void SysTick_Handler(void) {
	HAL_IncTick();
}
