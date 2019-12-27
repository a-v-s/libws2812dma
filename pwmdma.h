/*
 * pwmdma.h
 *
 *  Created on: 22 apr. 2017
 *      Author: andre
 */

#ifndef INC_PWMDMA_H_
#define INC_PWMDMA_H_

void pwm_init();
void start_dma_transer(void* memory, size_t size);
bool is_busy();

#endif /* INC_PWMDMA_H_ */
