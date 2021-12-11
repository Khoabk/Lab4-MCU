/*
 * Task.c
 *
 *  Created on: Nov 2, 2021
 *      Author: dang khoa
 */
#include "Task.h"
#include "stm32f103x6.h"
#include "main.h"




uint8_t task1[]="Task1\r";

uint8_t task2[]="Task2\r";

uint8_t task3[]="Task3\r";

uint8_t task4[]="Task4\r";

uint8_t task5[]="Task5\r";


void Led_blinking1(void)
{
	GPIOA->ODR ^=(1<<4);
	HAL_UART_Transmit(&huart2, task1,sizeof(task1),1000);
}


void Led_blinking2(void)
{
	GPIOA->ODR ^=(1<<5);
	HAL_UART_Transmit(&huart2, task2,sizeof(task2),1000);
}

void Led_blinking3(void)
{
	GPIOA->ODR ^= (1<<6);
	HAL_UART_Transmit(&huart2, task3,sizeof(task3),1000);
}

void Led_blinking4(void)
{
	GPIOA->ODR ^= (1<<7);
	HAL_UART_Transmit(&huart2, task4,sizeof(task4),1000);
}


void Led_blinking5(void)
{
	GPIOA->ODR ^= (1<<8);
	HAL_UART_Transmit(&huart2, task5,sizeof(task5),1000);
}
