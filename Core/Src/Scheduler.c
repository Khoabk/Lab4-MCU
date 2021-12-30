/*
 * Scheduler.c
 *
 *  Created on: Nov 2, 2021
 *      Author: dang khoa
 */


/*
 * CPU_scheduling.c
 *
 *  Created on: Oct 30, 2021
 *      Author: dang khoa
 */


#include "Watchdog.h"
#include "Lowpower_mode.h"
#include "Error_handler.h"
#include "Timer.h"
#include "stm32f1xx_hal.h"
#include "Task.h"
#include "string.h"


Error_message Errorcode= NO_ERROR;
Error_message Last_error_code = NO_ERROR ;


typedef struct{


	void (* pTask)(void);

	uint32_t Delay;

	uint32_t Period;

	uint8_t RunMe;

	uint32_t TaskID;

	uint32_t Prev_ID;

	uint32_t Next_ID;

}sTask;



#define MAX_SIZE  40
#define NO_TASK_ID 0

int Task_count=0;
int Head=0;
int Tail=0;



sTask Task_Array[MAX_SIZE];




static unsigned char SCH_Reset_Task(const int TASK_IDX)
{
	unsigned char Return_code;

	if(Task_Array[TASK_IDX].pTask==0)
	{
		Errorcode= ERROR_SCH_CANNOT_DELETE_TASK;
		return ERROR;
	}
	else{
		Return_code= NORMAL;
	}

	Task_Array[TASK_IDX].pTask = 0x0000;
	Task_Array[TASK_IDX].Delay = 0;
	Task_Array[TASK_IDX].Period = 0;
	Task_Array[TASK_IDX].RunMe = 0;

	return Return_code;
}



void SCH_Init(void)
{



	Head=0;
	Tail=0;
	Task_count=0;

	unsigned char i;

	for(i=0; i < MAX_SIZE ; i++)
	{
		SCH_Reset_Task(i);
		Task_Array[i].TaskID=i;
		Task_Array[i].Prev_ID = i-1;
		Task_Array[i].Next_ID= i+1;
	}

	Errorcode =0;

	Timer_init();
	Watchdog_init();
	//Configure normal sleep mode
	SCB->SCR &= ~(1<<2);

}



unsigned char SCH_Delete_Task(const int TASK_IDX)
{

	unsigned char Return_code;

	Return_code = SCH_Reset_Task(TASK_IDX);

	if(Return_code==NORMAL)
	{
		Task_count--;

		if(Tail==Head)return Return_code;

		if(Tail==TASK_IDX)
		{
			Tail=Task_Array[TASK_IDX].Prev_ID;
			return Return_code;
		}

		if(Head==TASK_IDX)
		{
			Head=Task_Array[TASK_IDX].Next_ID;
			Task_Array[Head].Prev_ID=-1;
		}
		else
		{
			Task_Array[Task_Array[TASK_IDX].Prev_ID].Next_ID=Task_Array[TASK_IDX].Next_ID;
			Task_Array[Task_Array[TASK_IDX].Next_ID].Prev_ID=Task_Array[TASK_IDX].Prev_ID;
		}

		Task_Array[TASK_IDX].Next_ID=Task_Array[Tail].Next_ID;
		Task_Array[Task_Array[TASK_IDX].Next_ID].Prev_ID=TASK_IDX;
		Task_Array[TASK_IDX].Prev_ID=Tail;
		Task_Array[Tail].Next_ID=TASK_IDX;
	}


	return Return_code;

}





static void SCH_Tail_Handling(unsigned int addpos)
{

	Task_Array[addpos].Next_ID = Task_Array[Tail].Next_ID;

	if(Task_count < MAX_SIZE){ Task_Array[Task_Array[Tail].Next_ID].Prev_ID = addpos;}

	Task_Array[addpos].Prev_ID = Tail;

	Task_Array[Tail].Next_ID = addpos;

	Tail = addpos;

}







static unsigned char SCH_Reallocation(unsigned int add_pos, char head_mode)
{

	int Traverse = Head;


	if(head_mode)
	{
		if(Tail == Head)return add_pos;
		Head= Task_Array[Head].Next_ID;
		Task_Array[Head].Prev_ID = -1;
		Traverse= Head;
	}



	while(1)
	{

		if(Task_Array[Traverse].Delay < Task_Array[add_pos].Delay)
		{
			Task_Array[add_pos].Delay -= Task_Array[Traverse].Delay;
			if(Traverse == Tail){

			if(head_mode)SCH_Tail_Handling(add_pos);

			else Tail=add_pos;

			break;
			}
			Traverse = Task_Array[Traverse].Next_ID;
			continue;
		}

		Task_Array[Traverse].Delay -= Task_Array[add_pos].Delay;

		if(!head_mode)
		{
			Task_Array[Tail].Next_ID = Task_Array[add_pos].Next_ID;

			if(Task_count < MAX_SIZE){ Task_Array[Task_Array[Tail].Next_ID].Prev_ID = Tail;}
		}

		Task_Array[add_pos].Next_ID = Traverse;

		Task_Array[add_pos].Prev_ID = Task_Array[Traverse].Prev_ID;

		Task_Array[Traverse].Prev_ID = add_pos;

		if(Traverse == Head){Head = add_pos;  break;}

		Task_Array[Task_Array[add_pos].Prev_ID].Next_ID = add_pos;

		break;
	}



	return add_pos;



}






unsigned char SCH_Add_Task(void(* pFunction)(), unsigned int DELAY, unsigned int PERIOD)
{
	
	int add_pos;

	if(Task_count==MAX_SIZE)
	{
		Errorcode= ERROR_SCH_TOO_MANY_TASKS;

		return MAX_TASK;
	}

	add_pos = (Task_count==0)?Tail:Task_Array[Tail].Next_ID;

	Task_count++;

	Task_Array[add_pos].pTask= pFunction;

	Task_Array[add_pos].Delay= DELAY;

	Task_Array[add_pos].Period= PERIOD;

	Task_Array[add_pos].RunMe =0;

	if(Task_count==1)return add_pos;

	return SCH_Reallocation(add_pos, 0);
	
}




void SCH_Update(void)
{

	if(Task_count == 0)return;

	int in_loop=0;

	if(Task_Array[Head].Delay == 0)
	{
		if(in_loop==0)in_loop=1;

		Task_Array[Head].RunMe++;

		if(Task_Array[Head].Period)
		{
			Task_Array[Head].Delay = Task_Array[Head].Period;

		}

	}
	if(in_loop==0)Task_Array[Head].Delay --;

}




static void SCH_Go_To_Sleep(void)
{
  __WFI();
}




void SCH_Report_Status(void)
{
#ifdef SCH_REPORT_ERRORS

	if(Errorcode != Last_error_code)
	{
		Error_port = 255 - Errorcode;
		Last_error_code = Errorcode;

		if(Errorcode != 0)
		{
			Error_tick_count = 60000;
		}
		else
		{
			Error_tick)count = 0;
		}
	}
	else{

		if(Error_tick_count != 0)
		{
			if(--Error_tick_count == 0)
			{
				Errorcode =0;
			}
		}

	}


#endif
}




void SCH_Dispatch_Task(void)
{

	while(Task_Array[Head].RunMe > 0||Task_Array[Head].Delay==0)
	{
		 (*Task_Array[Head].pTask)();

		 if(Task_Array[Head].RunMe > 0)Task_Array[Head].RunMe--;

		 if(Task_Array[Head].Period==0)SCH_Delete_Task(Head);

		 else
		 {
			 Task_Array[Head].Delay = Task_Array[Head].Period;

			 SCH_Reallocation(Head, 1);
		 }


	}

	SCH_Report_Status();

	SCH_Go_To_Sleep();

}














