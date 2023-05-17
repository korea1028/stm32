/*
 * AM2302.h
 *
 *  Created on: May 11, 2023
 *      Author: user
 */

#ifndef INC_AM2302_H_
#define INC_AM2302_H_

#define AM2302_SUCCESS                0    // return value when reading was success
#define AM2302_NOT_REPLY              1    // return value when AM2302 did not send low signal
#define AM2302_SIGNAL_ERR             2    // return value when AM2302 did not send high signal
#define AM2302_DATA_NOT_START         3    // return value when AM2302 did not send
#define AM2302_CHKSUM_ERR             4    // return value when checksum is not correct
#define AM2302_DATA_HIGH_ERR          5    // return value when AM2302 signal high time was not proper

#define AM2302_MAX_REPLY_TIME         100  // max repeat times to wait AM2302 to reply
#define AM2302_SIGNAL_LENGTH          100  // max repeat times for AM2302 reply 80uS
#define AM2302_DATA_LOW_LENGTH        55   // max repeat times for AM2302 low signal 50uS
#define AM2302_DATA_HIGH_LENGTH_1     80   // max repeat times for AM2302 high signal 1, < 80 uS
#define AM2302_DATA_HIGH_LENGTH_0     30   // max repeat times for AM2302 high signal 1, < 30 uS

#define AM2302_DATA_BITS              40   // Humidity Data 16bit + Temperature Data 16bit + checksum 8bit

typedef struct {
	uint16_t AM2302_Pin;
	GPIO_TypeDef *AM2302_Port;
	TIM_HandleTypeDef *AM2302_Htim;
	uint8_t buffer[40];
	uint16_t humid;
	uint16_t temp;
	uint8_t checksum;
} AM2302DATA;

void AM2302_Init(AM2302DATA *AM2302, GPIO_TypeDef *Port, uint16_t Pin, TIM_HandleTypeDef *htim);
uint8_t AM2302_ReadData(AM2302DATA *pData);
uint32_t Wait_For_Low(AM2302DATA *pData, uint32_t max);
uint32_t Wait_For_High(AM2302DATA *pData, uint32_t max);
float AM2302_Get_Temp(AM2302DATA *pData);
float AM2302_Get_Humid(AM2302DATA *pData);

#endif /* INC_AM2302_H_ */
