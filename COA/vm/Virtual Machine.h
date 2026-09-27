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

#ifndef VM_H
#define VM_H

#define FALSE	0
#define TRUE	1

#define CODE_SIGNATURE		-123

#define NO_OF_DATA_PAGES	10
#define DATA_PAGE_SIZE		1024

#define REGISTER_FILE_SIZE	64  //128
#define PROG_STACK_SIZE		256
#define DATA_MEMORY_SIZE	NO_OF_DATA_PAGES * DATA_PAGE_SIZE
#define MAX_NO_OF_THREADS	64

//Legal op codes
#define VM_INST_MOV		0
#define VM_INST_MVS		VM_INST_MOV + 1
#define VM_INST_CVS     VM_INST_MVS + 1
#define VM_INST_CVI     VM_INST_CVS + 1
#define VM_INST_LDB		VM_INST_CVI + 1
#define VM_INST_LDW		VM_INST_LDB + 1
#define VM_INST_LDX		VM_INST_LDW + 1
#define VM_INST_LNS		VM_INST_LDX + 1
#define VM_INST_LDBI	VM_INST_LNS + 1
#define VM_INST_LDWI	VM_INST_LDBI + 1
#define VM_INST_TAS		VM_INST_LDWI + 1
#define VM_INST_STB		VM_INST_TAS + 1
#define VM_INST_STW		VM_INST_STB + 1
#define VM_INST_STBI	VM_INST_STW + 1
#define VM_INST_STWI	VM_INST_STBI + 1
#define VM_INST_PSH		VM_INST_STWI + 1
#define VM_INST_POP		VM_INST_PSH + 1
#define VM_INST_PSHL	VM_INST_POP + 1
#define VM_INST_POPL	VM_INST_PSHL + 1
#define VM_INST_SWP		VM_INST_POPL + 1
#define VM_INST_ADD		VM_INST_SWP + 1
#define VM_INST_SUB		VM_INST_ADD + 1
#define VM_INST_SUBU	VM_INST_SUB + 1
#define VM_INST_MUL		VM_INST_SUBU + 1
#define VM_INST_DIV		VM_INST_MUL + 1

#define VM_INST_INC		VM_INST_DIV + 1
#define VM_INST_DEC		VM_INST_INC + 1
#define VM_INST_AND		VM_INST_DEC + 1

#define VM_INST_OR		VM_INST_AND + 1
#define VM_INST_NOT		VM_INST_OR + 1
#define VM_INST_SHL		VM_INST_NOT + 1
#define VM_INST_SHR		VM_INST_SHL + 1

#define VM_INST_ASL     VM_INST_SHR + 1
#define VM_INST_ASR		VM_INST_ASL + 1

#define VM_INST_JMP		VM_INST_ASR + 1

#define VM_INST_JEQ		VM_INST_JMP + 1
#define VM_INST_JNE		VM_INST_JEQ + 1
#define VM_INST_JGT		VM_INST_JNE + 1
#define VM_INST_JGE		VM_INST_JGT + 1
#define VM_INST_JLT		VM_INST_JGE + 1
#define VM_INST_JLE		VM_INST_JLT + 1
#define VM_INST_JNZ		VM_INST_JLE + 1

#define VM_INST_JZR		VM_INST_JNZ + 1
#define VM_INST_CAL		VM_INST_JZR + 1
#define VM_INST_LOOP	VM_INST_CAL + 1
#define VM_INST_JSEL	VM_INST_LOOP + 1

#define VM_INST_TABE	VM_INST_JSEL + 1
#define VM_INST_TABI	VM_INST_TABE + 1
#define VM_INST_MSF		VM_INST_TABI + 1

#define VM_INST_RET		VM_INST_MSF + 1
#define VM_INST_IRET	VM_INST_RET + 1
#define VM_INST_SWI		VM_INST_IRET + 1
#define VM_INST_HLT		VM_INST_SWI + 1

#define VM_INST_CMP		VM_INST_HLT + 1
#define VM_INST_CPS		VM_INST_CMP + 1

#define VM_INST_IN		VM_INST_CPS + 1
#define VM_INST_OUT		VM_INST_IN + 1
#define VM_INST_NOP		VM_INST_OUT + 1

//Addressing modes
#define ADDR_MODE_IMMEDIATE				0        //#4
#define ADDR_MODE_DIRECTREG				1        //R01
#define ADDR_MODE_DIRECTMEM				2        //100
#define ADDR_MODE_INDIRECTREG			3        //@R01
#define ADDR_MODE_INDIRECTMEM			4        //@100
#define ADDR_MODE_RELADDRESS			5        //+/-100
#define ADDR_MODE_RELREG_ADDRESS		6        //+/-R01
#define ADDR_MODE_RELINDREG_ADDRESS		7        //+/-@R01

#define MAXINTREQUESTS					6
#define INTR_CONSOLE_INPUT				1
#define INTR_TIMER						2
#define INT_EXCEPTION					3
#define INT_USER1						4
#define INT_USER2						5
#define INT_USER3						6

#define EXCEPTION_UNKNOWN				-1
#define EXCEPTION_BAD_SYSTEM_CALL		0
#define EXCEPTION_DIV_BY_ZERO			1
#define EXCEPTION_OVERFLOW				2
#define EXCEPTION_NO_MEMORY				3
#define EXCEPTION_BAD_IPC_RECEIVE_PARAM 4
#define EXCEPTION_IPC_SEND_OUT_FULL		5
#define EXCEPTION_BAD_IPC_SEND_PID		6
#define EXCEPTION_BAD_IPC_SEND_PARAM	7
#define EXCEPTION_BAD_IPC_COMMAND		8
#define EXCEPTION_NO_IPC_IN_PORT		9

//Error codes
#define ERROR_RUNTIME				 -1
#define ERROR_CODEFILE_LOAD          -2
#define ERROR_CODEFILE_ANALYSIS      -3
#define ERROR_ILLEGAL_PARAMETER      -4
#define ERROR_CODEFILE_UNSPECIFIED   -5

//Data types
#define VAR_TYPE_INTEGER	2
#define VAR_TYPE_BYTE		0
#define VAR_TYPE_BOOLEAN	4
#define VAR_TYPE_STRING		3
#define VAR_TYPE_OBJECT		5

//Maths instruction functions
#define MATH_FUNC_ADD		0
#define MATH_FUNC_SUB		1
#define MATH_FUNC_SUBU		2
#define MATH_FUNC_MUL		3
#define MATH_FUNC_DIV		4


typedef unsigned char Boolean;

struct guardTableInfo {
	int startAddr;
	int endAddr;	
};

//Used by the ready queue implementation
struct readyQInfo {
	Boolean Valid;
	int pid;
	int priority;
};

//Structures required for implementation of multi-threading
struct programStateInfo {
    int PCRegister;
    int SRRegister;
    int SPRegister;
    int StackState[PROG_STACK_SIZE];
    int RegSet[REGISTER_FILE_SIZE];
};

struct programListInfo {
	Boolean Valid;
	int startAddr;
	int priority;
	int parentThreadNo;
	programStateInfo programState;
	guardTableInfo *guardTable;
	int guardInfoPtr;
	int RRTickCount;
	Boolean waitForChild;
	int stackFrameReg;

	Boolean mutexFlag;
	Boolean inMutexWait;
	int semVarOffset;
	int mutexOwner;
	Boolean blockedWait;
};

void debugAid(unsigned char);

#endif
