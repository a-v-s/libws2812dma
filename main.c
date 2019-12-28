#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

#pragma pack(push,1)

typedef struct {
	uint8_t g;
	uint8_t r;
	uint8_t b;
} rgb_t;

rgb_t leds[8];

#pragma pack(pop)

void SystemClock_Config(void);

int main() {
	HAL_Init(); // Reset of all peripherals, Initializes the Flash interface and the Systick.
	SystemClock_Config(); // Configure the system clock to 72 MHz
	pwmdma_init();

	leds[0].r = 16;
	leds[0].g = 0;
	leds[0].b = 0;

	leds[1].r = 16;
	leds[1].g = 8;
	leds[1].b = 0;

	leds[2].r = 8;
	leds[2].g = 8;
	leds[2].b = 0;

	leds[3].r = 0;
	leds[3].g = 8;
	leds[3].b = 0;

	leds[4].r = 0;
	leds[4].g = 8;
	leds[4].b = 16;

	leds[5].r = 8;
	leds[5].g = 0;
	leds[5].b = 8;

	leds[6].r = 16;
	leds[6].g = 0;
	leds[6].b= 16;

	leds[7].r = 8;
	leds[7].g = 8;
	leds[7].b = 8;

	while (pwmdma_is_busy());
	pwmdma_fill_buffer_decompress(0, sizeof(leds), &leds);
	pwmdma_apply(sizeof(leds));

	while (1) {
		HAL_Delay(250);
		rgb_t tmp = leds[0];
		for (int i = 0 ; i <7; i++) {
			leds[i]=leds[i+1];
		}
		leds[7]=tmp;
		while (pwmdma_is_busy());
		pwmdma_fill_buffer_decompress(0, sizeof(leds), &leds);
		pwmdma_apply(sizeof(leds));
	}
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
