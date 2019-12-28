/*

File: 		pwmdma_stm32.h
Author:		André van Schoubroeck
License:	MIT


MIT License

Copyright (c) 2017, 2018, 2019 André van Schoubroeck

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef INC_PWMDMA_H_
#define INC_PWMDMA_H_

#include <stdbool.h>
#include <stddef.h>

extern uint8_t pwmdma_data_c0[3072 * 4]; // 4 Clockless channels of either 96 RGBW or 128 RGB leds

void pwmdma_init();
void pwmdma_start_dma_transer(void* memory, size_t size);
void pwmdma_apply(size_t size);
bool pwmdma_is_busy();

#endif /* INC_PWMDMA_H_ */
