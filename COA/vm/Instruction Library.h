/*Copyright (c) 2018 - 2020 Besim Mustafa

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef INST_LIB_H
#define INST_LIB_H

//System call values
#define SYSCALL_PROG_EXIT       	1
#define SYSCALL_PROG_SYNCEXIT		2
//#define SYSCALL_WRITE_CONSOLE_DATA	3
//#define SYSCALL_READ_CONSOLE_DATA	4
#define SYSCALL_THREAD_CREATE		5
#define SYSCALL_WAIT				6
#define SYSCALL_CHILD_WAIT			7
#define SYSCALL_GET_DATE			8
#define SYSCALL_BLOCKED_WAIT 		10
#define SYSCALL_TIMER           	11
#define SYSCALL_IPC_COMMAND			15
#define SYSCALL_CR_ENTER			18
#define SYSCALL_CR_LEAVE			19
#define SYSCALL_SEMAPHORE_WAIT		21
#define SYSCALL_SEMAPHORE_SIGNAL	22
#define SYSCALL_RUNTIME_EXCEPTION	24

//IPC commands
#define IPC_COMMAND_OPEN			0
#define IPC_COMMAND_CLOSE			1
#define IPC_COMMAND_SEND			2
#define IPC_COMMAND_RECEIVE			3

#define IPC_DIRECTION_IN			0
#define IPC_DIRECTION_OUT			1

#define IS_LOGIC_OP_AND				0
#define IS_LOGIC_OP_OR				1
#define IS_LOGIC_OP_NOT				2
#define IS_LOGIC_OP_SHL				3
#define IS_LOGIC_OP_SHR				4
#define IS_LOGIC_OP_ASR				5
#define IS_LOGIC_OP_ASL				6

//Thread queue functions
Boolean queueToReadyQ(int);
int dequeueFromReadyQ();

//Timer functions
void SetTimerInterval(long);
void ResetTimerInterrupt();
void StartTimer();
void StopTimer();
Boolean IsTimerInterrupt();

//Instruction Function Prototypes
void do_MOV_Instruction();
void do_MVS_Instruction();
void do_LDB_Instruction(int);
void do_LDW_Instruction(int);
void do_LDX_Instruction();
void do_TAS_Instruction();
void do_STB_Instruction(int);
void do_STW_Instruction(int);
void do_PSH_Instruction();
void do_POP_Instruction();
void do_PSHL_Instruction();
void do_POPL_Instruction();
void do_ADD_Instruction();
void do_SUB_Instruction();
void do_SUBU_Instruction();
void do_MUL_Instruction();
void do_DIV_Instruction();
void do_AND_Instruction();
void do_OR_Instruction();
void do_NOT_Instruction();
void do_SHL_Instruction();
void do_SHR_Instruction();
void do_ASR_Instruction();
void do_ASL_Instruction();
void do_INC_Instruction();
void do_DEC_Instruction();
void do_JMP_Instruction();
void do_JEQ_Instruction();
void do_JZR_Instruction();
void do_JNE_Instruction();
void do_JNZ_Instruction();
void do_JGT_Instruction();
void do_JLT_Instruction();
void do_JGE_Instruction();
void do_JLE_Instruction();
void do_LOOP_Instruction();
void do_JSEL_Instruction();
void do_MSF_Instruction();
void do_CAL_Instruction();
void do_RET_Instruction();
void do_IRET_Instruction();
void do_SWI_Instruction();
void do_CMP_Instruction();
void do_CPS_Instruction();
void do_IN_Instruction();
void do_OUT_Instruction();
void do_SWP_Instruction();

#endif
