/*
 * Error_handler.h
 *
 *  Created on: Oct 30, 2021
 *      Author: dang khoa
 */

#ifndef INC_ERROR_HANDLER_H_
#define INC_ERROR_HANDLER_H_



//#define SCH_REPORT_ERRORS

#ifdef SCH_REPORT_ERRORS

#define Error_tick_count 0

#define Error_port PORTA
#endif







typedef enum
{
	NO_ERROR,
	ERROR_SCH_TOO_MANY_TASKS,
	ERROR_SCH_CANNOT_DELETE_TASK,
	ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK,
	ERROR_SCH_WAITING_FOR_START_COMMAND_FROM_MASTER,
	ERROR_SCH_ONE_OR_MORE_SLAVE_DID_NOT_START,
	ERROR_SCH_LOST_SLAVE,
	ERROR_SCH_CAN_BUS_ERROR,
	ERROR_I2C_WRITE_BYTE_AT24C64
}Error_message;





typedef enum
{
	NORMAL,
	ERROR_CODE,
	MAX_TASK
}return_code;





#endif /* INC_ERROR_HANDLER_H_ */
