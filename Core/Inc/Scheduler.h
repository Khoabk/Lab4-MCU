/*
 * Scheduler.h
 *
 *  Created on: Nov 2, 2021
 *      Author: dang khoa
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_



void SCH_Init(void);

unsigned char SCH_Delete_Task(const int TASK_IDX);

unsigned char SCH_Add_Task(void(* pFunction)(), unsigned int DELAY, unsigned int PERIOD);

void SCH_Update(void);

void SCH_Dispatch_Task(void);


#endif /* INC_SCHEDULER_H_ */
