/*
 * AM2302.c
 *  Created on: May 11, 2023
 *      Author: korea1028
 *       Board: NUCLEO-F429ZI
 */

#include "stm32f4xx_hal.h"
#include "main.h"
#include "AM2302.h"

void AM2302_Init(AM2302DATA *pData, GPIO_TypeDef *Port, uint16_t Pin, TIM_HandleTypeDef *htim) {
	pData->AM2302_Port = Port;
	pData->AM2302_Pin = Pin;
	pData->AM2302_Htim = htim;
}

uint8_t AM2302_ReadData(AM2302DATA *pData) {
	int8_t i = 0;
	uint16_t temperature = 0;
	uint16_t humidity = 0;
	uint8_t checksum = 0;
	uint8_t compare_checksum = 0;
	uint8_t data[40] = { 0 };
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// Set AM2302_DATA Pin for output
	GPIO_InitStruct.Pin = pData->AM2302_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(pData->AM2302_Port, &GPIO_InitStruct);

	// Send start signal
	pData->AM2302_Port->BSRR = pData->AM2302_Pin; //Set high
	HAL_Delay(5);
	pData->AM2302_Port->BSRR = pData->AM2302_Pin << 16; //Set low
	HAL_Delay(1); //Wait 1ms
	pData->AM2302_Port->BSRR = pData->AM2302_Pin; //Set high

	// Set AM2302_DATA Pin for input
	GPIO_InitStruct.Pin = pData->AM2302_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(pData->AM2302_Port, &GPIO_InitStruct);

	// Wait for AM2302's reply
	if (Wait_For_Low(pData, AM2302_MAX_REPLY_TIME) > AM2302_MAX_REPLY_TIME) //if sensor didn't send low, return
		return AM2302_NOT_REPLY;

	// AM2302's low signal
	if (Wait_For_High(pData, AM2302_SIGNAL_LENGTH) > AM2302_SIGNAL_LENGTH) //if sensor didn't send high, return
		return AM2302_SIGNAL_ERR;

	// Wait for data
	if (Wait_For_Low(pData, AM2302_SIGNAL_LENGTH) > AM2302_SIGNAL_LENGTH) //if sensor didn't send low, return
		return AM2302_DATA_NOT_START;

	// AM2302 send data
	for (i = 0; i < AM2302_DATA_BITS; i++) {
		Wait_For_High(pData, AM2302_DATA_LOW_LENGTH);
		data[i] = Wait_For_Low(pData, AM2302_DATA_HIGH_LENGTH_1);
		if (data[i] > AM2302_DATA_HIGH_LENGTH_1)
			return AM2302_DATA_HIGH_ERR;
	}
	for(i = 0; i < AM2302_DATA_BITS; i++)
		pData->buffer[i] = data[i];
	for (i = 0; i < 16; i++) {
		if (i)
			humidity <<= 1;
		if (data[i] > AM2302_DATA_HIGH_LENGTH_0)
			humidity |= 1;
	}
	for (; i < 32; i++) {
		if (i)
			temperature <<= 1;
		if (data[i] > AM2302_DATA_HIGH_LENGTH_0)
			temperature |= 1;
	}
	for (; i < 40; i++) {
		if (i)
			checksum <<= 1;
		if (data[i] > AM2302_DATA_HIGH_LENGTH_0)
			checksum |= 1;
	}

	compare_checksum += temperature & 0xFF;
	compare_checksum += (temperature >> 8) & 0xFF;
	compare_checksum += humidity & 0xFF;
	compare_checksum += (humidity >> 8) & 0xFF;
	if(compare_checksum != checksum)
		return AM2302_CHKSUM_ERR;

	pData->temp = temperature;
	pData->humid = humidity;

	return AM2302_SUCCESS;
}

uint32_t Wait_For_Low(AM2302DATA *pData, uint32_t max) {
	//Wait Pin State until Low, return time[us]
	volatile uint32_t time = 0;
	pData->AM2302_Htim->Instance->CNT = 0;
	while (((pData->AM2302_Port->IDR) & pData->AM2302_Pin) && (time = pData->AM2302_Htim->Instance->CNT <= max))
		time = pData->AM2302_Htim->Instance->CNT;
	return time;

}
uint32_t Wait_For_High(AM2302DATA *pData, uint32_t max) {
	//Wait Pin State until High, return time[us]
	volatile uint32_t time = 0;
	pData->AM2302_Htim->Instance->CNT = 0;
	while (!((pData->AM2302_Port->IDR) & pData->AM2302_Pin) && (time = pData->AM2302_Htim->Instance->CNT <= max))
		;
	return time;
}

float AM2302_Get_Temp(AM2302DATA *pData) {
	if(pData->temp >> 15 & 1)
		return ~((pData->temp & 0x7FFF) - 1) / 10.0;
	return pData->temp / 10.0;
}

float AM2302_Get_Humid(AM2302DATA *pData) {
	return pData->humid / 10.0;
}

