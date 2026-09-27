/*Copyright (c) 2018 - 2019 Besim Mustafa

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>

#ifdef LINUX_PLATFORM
#include "Linux Support.h"
#else
#include <conio.h>
#endif

#include "Virtual Machine.h"
#include "Support Library.h"
#include "Instruction Library.h"

Boolean timerIsActive = FALSE;
long timerIntervalValue = -1;
clock_t lastTimeValue;

int Inst_IN_Num_Data_Cnt = 0;
int Inst_IN_Num_Sign_Entered = 0;
int Inst_IN_Num_Sign = 1;
unsigned char Inst_IN_Num_Data[4];

extern int registerFile[];
extern int programStack[];
extern readyQInfo threadReadyQueue[];

extern unsigned char opCode;					//Instruction opcode
extern int oldPCRegister;						//Holds address of current instruction
extern int pcRegister;							//Program counter register
extern int srRegister;							//Status register
extern int spRegister;							//Stack pointer register

extern int currentThreadNo;
extern int numberOfActiveThreads;

extern unsigned char *codeMemory;
extern unsigned char *dataMemory;

extern int IntVectorAddr[];

extern programListInfo programList[];

extern int inputDataMemOffset;
extern long memoryPageOffset;
extern int stackFrameReg;

void SetTimerInterval(long tInterval) {
     timerIntervalValue = tInterval;
}

void ResetTimerInterrupt() {
     lastTimeValue = clock();     
}
     
void StartTimer() {
     timerIsActive = TRUE;
     ResetTimerInterrupt();
}

void StopTimer() {
     timerIsActive = FALSE;
}

Boolean IsTimerInterrupt() {
     if (timerIsActive) {  
        if ((clock() - lastTimeValue) >= timerIntervalValue) {
           lastTimeValue = clock();
           return TRUE;       
        }
     }
     
     return FALSE;               
}

void do_MOV_Instruction() {
	unsigned char addrMode;
	int opnd1;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			PutRegFileValue(codeMemory[pcRegister++], opnd1);
			set_Status_Flag(opnd1);
			break;
	}
}

void do_MVS_Instruction() {
	unsigned char addrMode;
	int opnd1;
	int opnd2;
	unsigned char memByte;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_INDIRECTMEM:
			opnd1 = ReadMemDirect();
			break;
		case ADDR_MODE_INDIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd2 = ReadCodeWord();
			break;
		case ADDR_MODE_INDIRECTMEM:
			opnd2 = ReadMemDirect();
			break;
		case ADDR_MODE_INDIRECTREG:
			opnd2 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
				
	while (TRUE) {
		memByte = dataMemory[opnd1++];
		if (memByte == 0) break;
		dataMemory[opnd2++] = memByte;
	}
	dataMemory[opnd2++] = memByte;
}

void do_LDB_Instruction(int autoIncrement) {
	unsigned char addrMode;
	int opnd1;
	unsigned char reg;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadMemDirect();
			break;
		case ADDR_MODE_INDIRECTMEM:
			if (autoIncrement) {
				opnd1 = ReadMemIndirectWithAutoInc();
			} else {
				opnd1 = ReadMemIndirect();
			}
			break;
		case ADDR_MODE_INDIRECTREG:
			if (autoIncrement) {
				opnd1 = ReadRegIndirectWithAutoInc();
			} else {
				opnd1 = ReadRegIndirect();
			}
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			reg = codeMemory[pcRegister++];
			PutRegFileValue(reg, opnd1);
			set_Status_Flag(GetRegFileValue(reg));
			break;
	}
}

void do_LDW_Instruction(int autoIncrement) {
	unsigned char addrMode;
	int opnd1;
	unsigned char reg;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadMemWordDirect();
			break;
		case ADDR_MODE_INDIRECTMEM:
			if (autoIncrement) {
				opnd1 = ReadMemWordIndirectWithAutoInc();
			} else {
				opnd1 = ReadMemWordIndirect();
			}
			break;
		case ADDR_MODE_INDIRECTREG:
			if (autoIncrement) {
				opnd1 = ReadRegWordIndirectWithAutoInc();
			} else {
				opnd1 = ReadRegWordIndirect();
			}
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			reg =codeMemory[pcRegister++]; 
			PutRegFileValue(reg, opnd1);
			set_Status_Flag(GetRegFileValue(reg));
			//printf("+++ R%d = %d\n", reg, opnd1);
			break;
	}
}

//Same as LDW except the memory location is not accessed
void do_LDX_Instruction() {
	unsigned char addrMode;
	int opnd1;
	
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_INDIRECTMEM:
			opnd1 = ReadCodeWord();
			ReadMemoryWord(opnd1, &opnd1);
			break;
		case ADDR_MODE_INDIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			PutRegFileValue(codeMemory[pcRegister++],opnd1);
			break;
	}
}

void do_TAS_Instruction() {
	unsigned char addrMode;
	int memAddr;
	int opnd1;
	
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadMemWordDirectGetMemAddr(&memAddr);
			break;
		case ADDR_MODE_INDIRECTMEM:
			opnd1 = ReadMemWordIndirectGetMemAddr(&memAddr);
			break;
		case ADDR_MODE_INDIRECTREG:
			opnd1 = ReadRegWordIndirectGetMemAddr(&memAddr);
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			//Put memory location value in register
			PutRegFileValue(codeMemory[pcRegister++],opnd1);
			//Store value 1 in referenced memory location
			WriteMemoryWord(memAddr,1);
			set_Status_Flag(GetRegFileValue(opnd1));
			break;
	}	
}

void do_STB_Instruction(int autoIncrement) {
	unsigned char addrMode;
	int opnd1;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			WriteMemDirect(opnd1);
			break;
		case ADDR_MODE_INDIRECTMEM:
			if (autoIncrement) {
				WriteMemIndirectWithAutoInc(opnd1);
			} else {
				WriteMemIndirect(opnd1);
			}
			break;
		case ADDR_MODE_INDIRECTREG:
			if (autoIncrement) {
				WriteRegIndirectWithAutoInc(opnd1);
			} else {
				WriteRegIndirect(opnd1);
			}
			break;
	}
}

void do_STW_Instruction(int autoIncrement) {
	unsigned char addrMode;
	int opnd1;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			WriteMemWordDirect(opnd1);
			break;
		case ADDR_MODE_INDIRECTMEM:
			if (autoIncrement) {
				WriteMemWordIndirectWithAutoInc(opnd1);
			} else {
				WriteMemWordIndirect(opnd1);
			}
			break;
		case ADDR_MODE_INDIRECTREG:
			if (autoIncrement) {
				WriteRegWordIndirectWithAutoInc(opnd1);
			} else {
				WriteRegWordIndirect(opnd1);
			}
			break;
	}
}

void do_PSH_Instruction() {
	unsigned char addrMode;
	int opnd1;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	PushOntoStack(opnd1);
}

void do_POP_Instruction() {
	unsigned char addrMode;
	int opnd1;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			if (PopFromStack(&opnd1)) {
				PutRegFileValue(codeMemory[pcRegister++],opnd1);
			}
			break;
	}
}

void do_PSHL_Instruction() {
	unsigned char addrMode;
	int opnd1;
	int opnd2;
	int n;

	addrMode = codeMemory[pcRegister++];
	opnd1 = ReadCodeWord();           //codeMemory[pcRegister++];
	addrMode = codeMemory[pcRegister++];
	opnd2 = ReadCodeWord();           //codeMemory[pcRegister++];

	for (n = 0; n < 16; n++) {
		if (opnd1 & (1 << n)) {
			PushOntoStack(GetRegFileValue(n));
		}
	}
	for (n = 0; n < 16; n++) {
		if (opnd2 & (1 << n)) {
			PushOntoStack(GetRegFileValue(n + 16));
		}
	}
}

void do_POPL_Instruction() {
	unsigned char addrMode;
	int opnd1;
	int opnd2;
	int val;
	int n;

	addrMode = codeMemory[pcRegister++];
	opnd1 = ReadCodeWord();       //codeMemory[pcRegister++];
	addrMode = codeMemory[pcRegister++];
	opnd2 = ReadCodeWord();       //codeMemory[pcRegister++];

	for (n = 15; n >= 0 ; n--) {
		if (opnd2 & (1 << n)) {
			if (PopFromStack(&val)) {
				PutRegFileValue(n + 16, val);
			}
		}
	}
	for (n = 15; n >= 0; n--) {
		if (opnd1 & (1 << n)) {
			if (PopFromStack(&val)) {
				PutRegFileValue(n, val);
			}
		}
	}
}

void do_SWP_Instruction() {
	unsigned char addrMode;
	int opnd1;
	int reg1;
	int opnd2;
	int reg2;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			reg1 = codeMemory[pcRegister++];
			opnd1 = GetRegFileValue(reg1);
			break;
	}

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			reg2 = codeMemory[pcRegister++];
			opnd2 = GetRegFileValue(reg2);
			break;
	}

	PutRegFileValue(reg1,opnd2);
	PutRegFileValue(reg2,opnd1);
}

void do_MATH_Instruction(int mathFunc){
	unsigned char addrMode;
	unsigned char reg;
	int opnd1;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			reg = codeMemory[pcRegister++];
			switch (mathFunc) {
				case MATH_FUNC_ADD:
					AddToRegFileValue(reg, opnd1);
					break;
				case MATH_FUNC_SUB:
					SubFromRegFileValue(reg, opnd1);
					break;
				case MATH_FUNC_SUBU:
					SubFromRegFileValue(reg, opnd1);
					registerFile[reg] = abs(registerFile[reg]);
					break;
				case MATH_FUNC_MUL:
					MulRegFileValue(reg, opnd1);
					break;
				case MATH_FUNC_DIV:
					DivRegFileValue(reg, opnd1);
					break;
			}
			set_Status_Flag(GetRegFileValue(reg));
			break;
	}
}

void do_ADD_Instruction() {
	do_MATH_Instruction(MATH_FUNC_ADD);
}

void do_SUB_Instruction() {
	do_MATH_Instruction(MATH_FUNC_SUB);
}

void do_SUBU_Instruction() {
	do_MATH_Instruction(MATH_FUNC_SUBU);
}

void do_MUL_Instruction() {
	do_MATH_Instruction(MATH_FUNC_MUL);
}

void do_DIV_Instruction() {
	do_MATH_Instruction(MATH_FUNC_DIV);
}

void do_INC_DEC_Instruction(int isINC) {
	unsigned char addrMode;
	int opnd1;
	unsigned char reg;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			reg = codeMemory[pcRegister++];
			opnd1 = GetRegFileValue(reg);
			break;
	}

	if (isINC) {
		PutRegFileValue(reg, opnd1 + 1);
	} else {
		PutRegFileValue(reg, opnd1 - 1);
	}
	set_Status_Flag(GetRegFileValue(reg));
}

void do_INC_Instruction() {
	do_INC_DEC_Instruction(TRUE);
}

void do_DEC_Instruction() {
	do_INC_DEC_Instruction(FALSE);
}

void do_LOGIC_Instruction(int isLogicOp) {
	unsigned char addrMode;
	int opnd1;
	int opnd2;
	int opnd2x;
	int reg;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			reg = codeMemory[pcRegister++];
			opnd2 = GetRegFileValue(reg);
			break;
	}

	switch (isLogicOp) {
		case IS_LOGIC_OP_AND:
			PutRegFileValue(reg, opnd1 & opnd2);
			break;
		case IS_LOGIC_OP_OR:
			PutRegFileValue(reg, opnd1 | opnd2);
			break;
		case IS_LOGIC_OP_NOT:
			PutRegFileValue(reg, ~opnd1);
			break;
		case IS_LOGIC_OP_SHL:
		case IS_LOGIC_OP_ASL:
			PutRegFileValue(reg, opnd2 << opnd1);
			break;
		case IS_LOGIC_OP_SHR:
			PutRegFileValue(reg, opnd2 >> opnd1);
			break;
		case IS_LOGIC_OP_ASR:
			opnd2x = opnd2;
			PutRegFileValue(reg, opnd2 >> opnd1);
			if (opnd2x < 0) {
				//Preserve the sign bit if negative number operand
				opnd2 |= 1 << (sizeof(opnd1) - 1);
			}
			break;

	}

	set_Status_Flag(GetRegFileValue(reg));
}

void do_AND_Instruction() {
	do_LOGIC_Instruction(IS_LOGIC_OP_AND);
}

void do_OR_Instruction() {
	do_LOGIC_Instruction(IS_LOGIC_OP_OR);
}

void do_NOT_Instruction() {
	do_LOGIC_Instruction(IS_LOGIC_OP_NOT);
}

void do_SHL_Instruction() {
	do_LOGIC_Instruction(IS_LOGIC_OP_SHL);
}

void do_SHR_Instruction() {
	do_LOGIC_Instruction(IS_LOGIC_OP_SHR);
}

void do_ASL_Instruction() {
	do_LOGIC_Instruction(IS_LOGIC_OP_ASL);
}

void do_ASR_Instruction() {
	do_LOGIC_Instruction(IS_LOGIC_OP_ASR);
}

int resolve_Jump_Addr() {
	unsigned char addrMode;
	char relAddr;
	int opnd1;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
		case ADDR_MODE_RELADDRESS:			        //+/-100
			relAddr = codeMemory[pcRegister++];		//Needs to be signed byte value, so convert.
			opnd1 = oldPCRegister + relAddr;
			break;
		case ADDR_MODE_RELREG_ADDRESS:		        //+/-R01
			opnd1 = oldPCRegister + GetRegFileValue(codeMemory[pcRegister++]);
			break;
		case ADDR_MODE_RELINDREG_ADDRESS:		    //+/-@R01
			opnd1 = oldPCRegister + ReadRegWordIndirect();
			break;
	}

	return opnd1;
}

void do_JMP_Instruction() {
	pcRegister = resolve_Jump_Addr();
}

void do_JEQ_Instruction() {
	int opnd1;

	opnd1 = resolve_Jump_Addr();
	if (isZeroStatus()) {
		pcRegister = opnd1;
	}
}

void do_JZR_Instruction(){
	do_JEQ_Instruction();
}

void do_JNE_Instruction() {
	int opnd1;

	opnd1 = resolve_Jump_Addr();
	if (!isZeroStatus()) {
		pcRegister = opnd1;
	}
}

void do_JNZ_Instruction(){
	do_JNE_Instruction();
}

void do_JGT_Instruction() {
	int opnd1;

	opnd1 = resolve_Jump_Addr();
	if (!isNegativeStatus() && !isZeroStatus()) {
		pcRegister = opnd1;
	}
}

void do_JGE_Instruction() {
	int opnd1;

	opnd1 = resolve_Jump_Addr();
	if (!isNegativeStatus()) {
		pcRegister = opnd1;
	}
}

void do_JLT_Instruction() {
	int opnd1;

	opnd1 = resolve_Jump_Addr();
	if (isNegativeStatus() && !isZeroStatus()) {
		pcRegister = opnd1;
	}
}

void do_JLE_Instruction() {
	int opnd1;

	opnd1 = resolve_Jump_Addr();
	if (isNegativeStatus() != isZeroStatus()) {
		pcRegister = opnd1;
	}
}

void do_LOOP_Instruction() {
	unsigned char addrMode;
	int opnd1;
	int opnd2;
	unsigned char regNo;

	opnd1 = resolve_Jump_Addr();
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			regNo = codeMemory[pcRegister++];
			opnd2 = GetRegFileValue(regNo) - 1;
			PutRegFileValue(regNo, opnd2);
			if (opnd2 > 0) {
				pcRegister = opnd1;
			}
			break;
	}
}

int getSelectCaseAddress(int caseVal, int jumpTableAddr) {
    unsigned char opCode;
    int opnd1;
    int opnd2;
        
    pcRegister = jumpTableAddr;
    
    //Process the jump table
    while(TRUE) {
        opCode = codeMemory[pcRegister++];
        
        if (opCode == VM_INST_TABI) {
           //Should use IMMEDIATE addressing mode
           opnd1 = ReadCodeWord();
           if (opnd1 == caseVal) {
              //Get case jump address 
              opnd2 = ReadCodeWord();
              break;          
           }
           //Skip this table entry
           pcRegister += 2;           
        }
        else
        if (opCode == VM_INST_TABE) {
           //Get CASE ELSE jump address
           //Should use MEMORYDIRECT addressing mode
           opnd2 = ReadCodeWord();
           break;              
        }
    }
    
    //Return the selected case jump address
    return opnd2;
}

void do_JSEL_Instruction() {
	unsigned char addrMode;
	int opnd1;             //Holds case index value
	int opnd2;             //Holds case jump address
	
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;     
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	} 
   
	addrMode = codeMemory[pcRegister++];
    switch (addrMode) {
    	case ADDR_MODE_DIRECTMEM:
    		opnd2 = ReadCodeWord();
    		break;
    }
    
    //Get the case jump address
    pcRegister = getSelectCaseAddress(opnd1, opnd2);      
}

void do_MSF_Instruction() {
	PushOntoStack(stackFrameReg);
    stackFrameReg = spRegister - 1;
    //Reserve return address position on stack
    PushOntoStack(0);
}

void do_CAL_Instruction() {
	int opnd1;

	opnd1 = resolve_Jump_Addr();
	SaveContextOnStack(pcRegister);
	pcRegister = opnd1;
}

void do_RET_Instruction() {
	RestoreContextOnStack();
}

void do_IRET_Instruction() {
	RestoreContextOnStack();
}

//Alternative to orphaning all children
void stopChildren(int parentPid) {
	int i;
	int parent;
	
	//Search for children of this thread
	for (i = 1; i < MAX_NO_OF_THREADS; i++) {
		if (programList[i].Valid) {
			parent = programList[i].parentThreadNo;
			if (parent == parentPid) {
				//Stop the child
				programList[i].Valid = FALSE;
				numberOfActiveThreads--;
				//Stop its children too, if any
				stopChildren(i);
			}	
		}
	}	
}

//Alternative to stopping all children
void orphanChildren(int parentPid) {
	int i;
	int parent;
	
	//Search for children of this thread
	for (i = 1; i < MAX_NO_OF_THREADS; i++) {
		if (programList[i].Valid) {
			parent = programList[i].parentThreadNo;
			if (parent == parentPid) {
				//Assign child its grandparent as its parent!
				programList[i].parentThreadNo = programList[parentPid].parentThreadNo;
			}	
		}
	}	
}

Boolean queueToReadyQ(int i) {
	int n, p;
	Boolean t = FALSE;

	//This thread scheduling uses priorit based round robin scheduling method

	//It is assumed that the queue is already sorted in the priority order
	for (n = 0; n < MAX_NO_OF_THREADS; n++) {
		if (threadReadyQueue[n].Valid) {
			if (programList[i].priority < threadReadyQueue[n].priority) {
				//Shift Q to make room for this thread of higher priority
				for (p = n; threadReadyQueue[p].Valid; p++) {
					threadReadyQueue[p + 1] = threadReadyQueue[p];
				}
				//Insert the higher priority thread in the Q 
				threadReadyQueue[n].pid = i;
				threadReadyQueue[n].priority = programList[i].priority;
				t = TRUE;
				//printf("--- Queued thread %d\n", i);
				break;
			}
		} else {
			//Add the thread to the tail of the Q
			threadReadyQueue[n].Valid = TRUE;
			threadReadyQueue[n].pid = i;
			threadReadyQueue[n].priority = programList[i].priority;
			t = TRUE;
			//printf("--- Queued thread %d\n", i);
			break;			
		}
	}

	return t;
}

int dequeueFromReadyQ() {
	int n;
	int i = -1;

	//Check to see if a valid thread at the head of the Q
	if (threadReadyQueue[0].Valid) {
		//Select the thread to dequeue from the head of the Q
		i = threadReadyQueue[0].pid;
		threadReadyQueue[0].Valid = FALSE;

		//Move the threads one place up towards the head of the queue
		for (n = 1; n < MAX_NO_OF_THREADS; n++) {
			if (threadReadyQueue[n].Valid) {
				threadReadyQueue[n - 1] = threadReadyQueue[n];
				threadReadyQueue[n].Valid = FALSE;
			}
			else break;
		}

		//printf("--- Dequeued thread %d\n", i);
	}

	return i;
}

void do_SWI_Instruction() {
	unsigned char addrMode;
	//int opnd1;
	int exNo;
	int srvcNo;
	int waitTime;
	int pageAddr;
	time_t ltime;
	struct tm *today;
	char dateBuffer[11];
	int ipcCommand;
	int semVarOffset;
	int lockFlagAddr;
	int lockAddr;
	char exMsg[60];

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			srvcNo = ReadCodeWord();
			break;
	}

	//PopFromStack(&srvcNo);

	//System call functions
	switch (srvcNo) {
		case SYSCALL_PROG_EXIT:
		case SYSCALL_PROG_SYNCEXIT:
			//Stop the program
            if (currentThreadNo == 0) {
				//Stop the main thread
				//Stop the children too if they are not already stopped
				exit(0);
			}
			else {
				if (srvcNo == SYSCALL_PROG_SYNCEXIT) {
					//Reset the lock word
					WriteMemoryWord(0, 0);
					for (int i = 0; i < MAX_NO_OF_THREADS; i++) {
						if (programList[i].Valid && programList[i].blockedWait) {
							programList[i].blockedWait = FALSE;
							//Thread is not blocked so schedule it to run
							queueToReadyQ(i);
						}
					}
				}

				//Stop currently running child thread
				int parentThreadNo = programList[currentThreadNo].parentThreadNo;
				programList[currentThreadNo].Valid = FALSE;
				numberOfActiveThreads--;
				//printf("Thread %d ending [%d]\n", currentThreadNo, numberOfActiveThreads);

				//Stop its children too
				stopChildren(currentThreadNo);

				//Check to see if current thread's parent is waiting for its children
				if (programList[parentThreadNo].waitForChild) {
					//This parent is set to wait for its children
					//Initially assume this parent has no children
					programList[parentThreadNo].waitForChild = FALSE;
					//printf("Thread %d parent %d is waiting\n", currentThreadNo, parentThreadNo);

					//Check to see if this parent has any children
					for (int i = 0; i < MAX_NO_OF_THREADS; i++) {
						if (programList[i].Valid) {
							if (programList[i].parentThreadNo == parentThreadNo) {
								//printf("Parent thread %d has active child %d\n", parentThreadNo, i);
								//Yes, this parent has live children so continue waiting
            					programList[parentThreadNo].waitForChild = TRUE;
            					break;
							}
						}
					}

					if (!programList[parentThreadNo].waitForChild) {
						//Not waiting anymore so Add to the ready Q
						queueToReadyQ(parentThreadNo);
					}
				} 

				if (threadReadyQueue[0].Valid) {
					//There is a thread in the head of the ready Q so schedule it to run
					int i = dequeueFromReadyQ();
					RestoreThreadContext(i);
				}
			}
			break;

		case SYSCALL_THREAD_CREATE:
			int addr;
			int pr;
			int i;

			//Initialize the child thread
			for (i = 1; i < MAX_NO_OF_THREADS; i++) {
				if (!programList[i].Valid) {
            		programList[i].Valid = TRUE;
					//Get thread start address
            		PopFromStack(&addr);
            		programList[i].startAddr = addr;
            		programList[i].programState.PCRegister = addr;
					//Get thread priority
            		PopFromStack(&pr);
            		programList[i].priority = pr;
            		programList[i].parentThreadNo = currentThreadNo;
            		programList[i].RRTickCount = 10;
            		programList[i].waitForChild = FALSE;
            		//Inherit parent's guard table data
            		programList[i].guardTable = programList[currentThreadNo].guardTable;
            		programList[i].guardInfoPtr = programList[currentThreadNo].guardInfoPtr;
					programList[i].stackFrameReg = stackFrameReg;
					programList[i].inMutexWait = FALSE;
					programList[i].semVarOffset = -1;
					programList[i].mutexFlag = FALSE;
					programList[i].blockedWait = FALSE;

					//printf("=== New thread %d is created\n", i);

					queueToReadyQ(i);

					numberOfActiveThreads++;

            		break;
				}
			} 
			if (i == MAX_NO_OF_THREADS) {
				printf("Maximum number of threads (%d) is reached; no new thread is created!\n", MAX_NO_OF_THREADS);
			}
		    break;

        case SYSCALL_TIMER:
			int timerFunc;
			int timeInterval;
			
			PopFromStack(&timerFunc);
			if (timerFunc == 0) {
            	//Stop timer
            	StopTimer();
			}
			else { 
            	//Start timer
            	//Get time interval for timer interrupts
            	PopFromStack(&timeInterval);
            	//Initialize the interval
            	SetTimerInterval(timeInterval);
            	StartTimer();
			} 	
			break;

		case SYSCALL_WAIT:
			if (PopFromStack(&waitTime)) {
				sleep(waitTime);
			}
			break;

		case SYSCALL_CHILD_WAIT:
			//Check to see if this thread has any children
			for (int i = 0; i < MAX_NO_OF_THREADS; i++) {
				if (programList[i].Valid) {
					if (programList[i].parentThreadNo == currentThreadNo) {
						//Found at least one child, so set wait-for-child flag
            			programList[currentThreadNo].waitForChild = TRUE;
						//printf("=== Thread %d is waiting for children\n", currentThreadNo);

						SaveThreadContext(currentThreadNo);

						if (threadReadyQueue[0].Valid) {
							i = dequeueFromReadyQ();
							//There is a queued thread so this can be switched in (context switching)
							RestoreThreadContext(i);
						}

            			break;
					}
				}
			}			
			break;

		case SYSCALL_CR_ENTER:
			if (programList[programList[currentThreadNo].parentThreadNo].mutexFlag) {
				//Critical region is entered by another thread
				//Put this thread in waiting state
				programList[currentThreadNo].inMutexWait = TRUE;

				SaveThreadContext(currentThreadNo);

				if (threadReadyQueue[0].Valid) {
					int i = dequeueFromReadyQ();
					//There is a queued thread so this can be switched in (context switching)
					RestoreThreadContext(i);
				}
			} else {
				//Critical region is not entered, so flag entered and identify owner of mutex
				programList[programList[currentThreadNo].parentThreadNo].mutexFlag = TRUE;
				programList[programList[currentThreadNo].parentThreadNo].mutexOwner = currentThreadNo;
			}
			break;

		case SYSCALL_CR_LEAVE:
			if (programList[programList[currentThreadNo].parentThreadNo].mutexFlag) {
				if (programList[programList[currentThreadNo].parentThreadNo].mutexOwner == currentThreadNo) {
					//This is the owner of the mutex lock
					//Reset the mutex flag
					programList[programList[currentThreadNo].parentThreadNo].mutexFlag = FALSE;

					for (int i = 0; i < MAX_NO_OF_THREADS; i++) {
						if (programList[i].Valid) {
							if (programList[i].inMutexWait) {
								if (programList[i].parentThreadNo == programList[currentThreadNo].parentThreadNo) {
									programList[i].inMutexWait = FALSE;
									queueToReadyQ(i);
								}
							}
						}
					}
				}
			}
			break;

		case SYSCALL_BLOCKED_WAIT:
			PopFromStack(&lockAddr);
			SaveThreadContext(currentThreadNo);
			programList[currentThreadNo].programState.PCRegister = lockAddr;
			programList[currentThreadNo].blockedWait = TRUE;

			SaveThreadContext(currentThreadNo);

			if (threadReadyQueue[0].Valid) {
				int i = dequeueFromReadyQ();
				//There is a queued thread so this can be switched in (context switching)
				RestoreThreadContext(i);
			}

			break;

		case SYSCALL_SEMAPHORE_WAIT:
			PopFromStack(&semVarOffset);
			PopFromStack(&lockFlagAddr);
			ReadMemoryWord(semVarOffset, &i);

			if (i > 0) {
				WriteMemoryWord(semVarOffset, i-1);
				//Unlock
				WriteMemoryWord(lockFlagAddr, 0);
			} else {
				//Unlock
				WriteMemoryWord(lockFlagAddr, 0);
				//Critical region is locked
				//Put this thread to waiting state
				programList[currentThreadNo].inMutexWait = TRUE;
				programList[currentThreadNo].semVarOffset = semVarOffset;

				SaveThreadContext(currentThreadNo);

				if (threadReadyQueue[0].Valid) {
					i = dequeueFromReadyQ();
					//There is a queued thread so this can be switched in (context switching)
					RestoreThreadContext(i);
				}
			}
			break;

		case SYSCALL_SEMAPHORE_SIGNAL:
			unsigned char found;

			PopFromStack(&semVarOffset);
			PopFromStack(&lockFlagAddr);

			//Find a thread suspended on the locked critical region
			found = 0;
			for (i = 0; i < MAX_NO_OF_THREADS; i++) {
				if (programList[i].Valid) {
					if (programList[i].inMutexWait) {
						if (programList[i].semVarOffset == semVarOffset) {
							//Found, so take this thread out of waiting state
							found = 1;
							programList[i].inMutexWait = FALSE;
							programList[i].semVarOffset = -1;
							queueToReadyQ(i);
							break;
						}
					}
				}
			}

			if (!found) {
				ReadMemoryWord(semVarOffset, &i);
				WriteMemoryWord(semVarOffset, i + 1);
			}
			//Unlock
			WriteMemoryWord(lockFlagAddr, 0);
			break;

		case SYSCALL_GET_DATE:
			if (PopFromStack(&pageAddr)) {
				time(&ltime);
				today = localtime( &ltime );
				strftime(dateBuffer, 11, "%d/%m/%Y", today);
				WriteMemoryString(dateBuffer, &pageAddr, 11);
			}
			break;
			
		case SYSCALL_IPC_COMMAND:
			PopFromStack(&ipcCommand);
			//Currently IPC commands are not implemented!
			switch (ipcCommand) {
				case IPC_COMMAND_OPEN:
					break;
				case IPC_COMMAND_CLOSE:
					break;
				case IPC_COMMAND_SEND:
					break;
				case IPC_COMMAND_RECEIVE:
					break;
				default:
					break;					
			}
			break;

		case SYSCALL_RUNTIME_EXCEPTION:
			//Get exception number
			PopFromStack(&exNo);
			sprintf(exMsg, "%.4dRuntime exception", exNo);
			throw std::runtime_error(exMsg);
			break;

		default:
			//Unknown system call
			sprintf(exMsg, "%.4dBad runtime service request", srvcNo);
			//printf("%s\n", exMsg);
			throw std::runtime_error(exMsg);	
			break;
	}
}

void do_CMP_Instruction() {
	unsigned char addrMode;
	int opnd1;
	int opnd2;
	int cmpRes;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			opnd2 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	cmpRes = opnd2 - opnd1;
	set_Status_Flag(cmpRes);
}

void do_CPS_Instruction() {
	unsigned char addrMode;
	int opnd1;
	int opnd2;
	unsigned char memByte;
	unsigned char memByte2;
	int cmpRes;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_INDIRECTMEM:
			opnd1 = ReadMemDirect();
			break;
		case ADDR_MODE_INDIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd2 = ReadCodeWord();
			break;
		case ADDR_MODE_INDIRECTMEM:
			opnd2 = ReadMemDirect();
			break;
		case ADDR_MODE_INDIRECTREG:
			opnd2 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
	}
	while (TRUE) {
		memByte = dataMemory[opnd1++];
		memByte2 = dataMemory[opnd2++];
		if (memByte == 0 && memByte2 == 0) break;
		cmpRes = memByte2 - memByte;
		if (cmpRes != 0) break;
	}
	set_Status_Flag(cmpRes);
}

void do_IN_Instruction() {
	unsigned char addrMode;
	int opnd1;
	int opnd2;
	int i;
	int n;
	int value = 0;
	unsigned char input;
	int thisPcAddress = pcRegister - 1;
	char exMsg[60];

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadCodeWord();
			break;
	}
	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_DIRECTREG:
			opnd2 = codeMemory[pcRegister++];
			break;
		case ADDR_MODE_DIRECTMEM:
			opnd2 = ReadCodeWord();
			break;
	}
	if (opnd1 == 0 || opnd1 == 1 || opnd1 == 2) {
		if (opnd1 == 1 || opnd1 == 2) {
			//Blocking I/O
#ifdef LINUX_PLATFORM
			while (TRUE) {
				input = kb_getc();
				if (input != 0) break;
			}
#else
			while (!_kbhit()) {}
			input = _getch();
			//printf("%d", input);
#endif
			if (addrMode == ADDR_MODE_DIRECTMEM) {
				//String input
				if (inputDataMemOffset == -1) {
					//Set the string start address
					inputDataMemOffset = opnd2;
					//First, put the string tag code
					WriteMemoryByte(inputDataMemOffset, VAR_TYPE_STRING);
					inputDataMemOffset++;
				}
				if (input == '\r') {
					//Line feed ("Return" key) terminates the input entry
					//Finish off with the null delimiter
					WriteMemoryByte(inputDataMemOffset, 0);
					inputDataMemOffset = -1;
				}
				else {
					putchar(input);
					fflush(stdout);

					//Store string character in memory
					WriteMemoryByte(inputDataMemOffset, input);
					inputDataMemOffset++;
					//Make sure we execute this instruction again
					pcRegister = thisPcAddress;
				}
			}
			else {
				if (addrMode == ADDR_MODE_DIRECTREG) {
					if (opnd1 == 2) {
						if (input != '\r') {
							//Char input
							putchar(input);
							fflush(stdout);
						}
						PutRegFileValue(opnd2, input);
					}
					else {
						if (input == '\b') {
							//Delete last digit
							if (Inst_IN_Num_Data_Cnt > 0) {
								putchar('\b');
								putchar(' ');
								putchar('\b');
								fflush(stdout);

								Inst_IN_Num_Data_Cnt -= 1;
							}
							else {
								if (Inst_IN_Num_Sign_Entered) {
									putchar('\b');
									putchar(' ');
									putchar('\b');
									fflush(stdout);

									Inst_IN_Num_Sign_Entered = 0;
									Inst_IN_Num_Sign = 1;
								}
							}
							//Make sure we execute this instruction again
							pcRegister = thisPcAddress;
						}
						else {
							if (input != '\r') {
								//Number input - allowed if: digits 0 to 9 or '-', upto size of array digits!
								if (((input >= '0' && input <= '9') || input == '-' || input == '+') && Inst_IN_Num_Data_Cnt < sizeof(Inst_IN_Num_Data)) {
									if ((Inst_IN_Num_Data_Cnt == 0) && ((input == '-') || (input == '+')) && (!Inst_IN_Num_Sign_Entered)) {
										putchar(input);
										fflush(stdout);
										if (input == '-') {
											Inst_IN_Num_Sign = -1;
										}
										else {
											Inst_IN_Num_Sign = 1;
										}
										Inst_IN_Num_Sign_Entered = 1;
									}
									else {
										if (input != '-') {
											putchar(input);
											fflush(stdout);
											//printf("(%d)\n", Inst_IN_Num_Data_Cnt);
											Inst_IN_Num_Data[Inst_IN_Num_Data_Cnt++] = input;
										}
									}
								}
								//Make sure we execute this instruction again
								pcRegister = thisPcAddress;
							}
							else {
								//End of number entry - return the number entered
								//Inst_IN_Num_Data_Val = 0;
								for (n = Inst_IN_Num_Data_Cnt - 1, i = 1; n >= 0; n--) {
									value += (Inst_IN_Num_Data[n] - '0') * i;
									i *= 10;
								}

								//printf("%d\n", Inst_IN_Num_Data_Val * Inst_IN_Num_Sign);

								PutRegFileValue(opnd2, value * Inst_IN_Num_Sign);
								Inst_IN_Num_Data_Cnt = 0;
								Inst_IN_Num_Sign = 1;
							}
						}
					}
				}
			}

			if (IntVectorAddr[INTR_CONSOLE_INPUT] != -1) {
				//Invoke key input interrupt
				SaveContextOnStack(pcRegister);
				pcRegister = IntVectorAddr[INTR_CONSOLE_INPUT];
			}
		}
		else {
			//Non-blocking I/O - char input only
#ifdef LINUX_PLATFORM
			input = kb_getc();
			if (input != 0) {
#else
			if (_kbhit()) {
				input = _getch();
#endif
				//printf(">> Input = %c\n", input);

				PutRegFileValue(opnd2, input);

				if (IntVectorAddr[INTR_CONSOLE_INPUT] != -1) {
					//Invoke key input interrupt
					SaveContextOnStack(pcRegister);
					pcRegister = IntVectorAddr[INTR_CONSOLE_INPUT];
				}
			}
		}
	}
}

void do_OUT_Instruction() {
	unsigned char addrMode;
	unsigned char addrMode2;
	int opnd1;
	int opnd2;
	unsigned char data;
	int val;
	unsigned char bval;

	addrMode = codeMemory[pcRegister++];
	switch (addrMode) {
		case ADDR_MODE_IMMEDIATE:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_DIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
		case ADDR_MODE_DIRECTMEM:
			opnd1 = ReadCodeWord();
			break;
		case ADDR_MODE_INDIRECTREG:
			opnd1 = GetRegFileValue(codeMemory[pcRegister++]);
			break;
		case ADDR_MODE_INDIRECTMEM:
			opnd1 = ReadMemIndirect();
			break;
	}
		
	addrMode2 = codeMemory[pcRegister++];
	switch (addrMode2) {
		case ADDR_MODE_DIRECTMEM:
			opnd2 = ReadCodeWord();
			break;
	}

	if (opnd2 == 0) {
		//Stream output
		if (addrMode == ADDR_MODE_IMMEDIATE || addrMode == ADDR_MODE_DIRECTREG) {
            printf("%d",opnd1);
		    fflush(stdout);				
		}
		else {
			ReadMemoryByte(opnd1, &data);
			switch (data) {
				case VAR_TYPE_BYTE:
					ReadMemoryByte(opnd1, &bval); 
					printf("%d",bval);
					fflush(stdout);
				case VAR_TYPE_BOOLEAN:
				case VAR_TYPE_INTEGER:
					ReadMemoryWord(opnd1, &val); 
					printf("%d",val);
					fflush(stdout);	
					break;
				case VAR_TYPE_STRING:
					printf("%s",&dataMemory[opnd1 + 1]);
					fflush(stdout);
					break;
				default:
					break;
			}
		}
	}
	else {
		if (opnd2 == 1) {
			//Character output
			if (opnd1 == 10) {
				//Newline
				putchar('\n');
				putchar('\r');
				fflush(stdout);
			}
			else {
				//Part of new line used in development environment - not needed here.
				putchar(opnd1);
				fflush(stdout);
			}
		}
	}
}
