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
#include <limits.h>
#include <stdexcept>

#include "Virtual Machine.h"

extern int registerFile[];
extern int programStack[];
extern int pcRegister;							//Program counter register
extern int srRegister;							//Status register
extern int spRegister;							//Stack pointer register

extern unsigned char *codeMemory;
extern unsigned char *dataMemory;

extern char *progName;
extern short int startAddress;
extern long checksum;

extern Boolean isLibraryProg;
extern long memoryPageOffset;
extern int stackFrameReg;

extern int IntVectorAddr[];
extern struct guardTableInfo *guardTable;
extern int guardInfoPtr;
extern programListInfo programList[];

extern int runResult;
extern Boolean stopProg;

extern int currentThreadNo;
extern int oldPCRegister;

void RunTimeError(const char *errMsg) {
	printf("Runtime error: %s at address location %u\n",errMsg, oldPCRegister);
	runResult = ERROR_RUNTIME;
	stopProg = TRUE;
}

void SaveThreadContext(int ThreadId){
	int i;
	
	programList[ThreadId].programState.PCRegister = pcRegister;
	programList[ThreadId].programState.SRRegister = srRegister;	
	programList[ThreadId].programState.SPRegister = spRegister;
	programList[ThreadId].stackFrameReg = stackFrameReg;

	//Save register file contents
	for (i = 0; i < REGISTER_FILE_SIZE; i++) {
		programList[ThreadId].programState.RegSet[i] = registerFile[i];
	}
	
	//Save stack contents
	for (i = 0; i < spRegister; i++) {
		programList[ThreadId].programState.StackState[i] = programStack[i];
	}	
}

void RestoreThreadContext(int ThreadId) {
	int i;
	
	pcRegister = programList[ThreadId].programState.PCRegister;
	srRegister = programList[ThreadId].programState.SRRegister;	
	spRegister = programList[ThreadId].programState.SPRegister;
	stackFrameReg = programList[ThreadId].stackFrameReg;

	//Restore register file contents
	for (i = 0; i < REGISTER_FILE_SIZE; i++) {
		registerFile[i] = programList[ThreadId].programState.RegSet[i];
	}
	
	//Restore stack contents
	for (i = 0; i < spRegister; i++) {
		programStack[i] = programList[ThreadId].programState.StackState[i];
	}
	
	programList[ThreadId].RRTickCount = 10;	
	currentThreadNo = ThreadId;
}

Boolean PushOntoStack(int Value) {
	if (spRegister < PROG_STACK_SIZE) {
		programStack[spRegister++] = Value;
		//printf("*** Stack: %d %d\n", spRegister-1,Value);
		return TRUE;
	}
	else {
		//Stack overflow
		RunTimeError("Stack overflow");
		return FALSE;
	}
}

Boolean PopFromStack(int *popVal) {
	if (spRegister > 0) {
		*popVal = programStack[--spRegister];
		return TRUE;
	}
	else {
		//Stack underflow
		RunTimeError("Stack underflow");
		return FALSE;
	}
}

void PutRegFileValue(unsigned char RegFileIndex, int Value) {
	registerFile[RegFileIndex] = Value;
}

int GetRegFileValue(unsigned char RegFileIndex) {
	return registerFile[RegFileIndex];
}

void set_Status_Flag(int res) {
	srRegister = 0;			//Z and N flags reset
	if (res == 0) {
		srRegister |= 1;	//Z flag is set
	}
	else
	if (res < 0) {
		srRegister |= 2;	//N flag is set
	}
}

void AddToRegFileValue(unsigned char RegFileIndex, int Value) {
	//Check for integer overflow
	/*if (registerFile[RegFileIndex] > INT_MAX - Value) {
		std::runtime_error OverflowException("0002Integer overflow");	
		throw(OverflowException);
	}*/	
	registerFile[RegFileIndex] += Value;
	//set_Status_Flag(registerFile[RegFileIndex]);
}

void SubFromRegFileValue(unsigned char RegFileIndex, int Value) {
	//Check for integer overflow
	/*if (registerFile[RegFileIndex] < INT_MIN + Value) {
		std::runtime_error OverflowException("0002Integer overflow");
		throw(OverflowException);
	}*/
	registerFile[RegFileIndex] -= Value;
	//set_Status_Flag(registerFile[RegFileIndex]);
}

void MulRegFileValue(unsigned char RegFileIndex, int Value) {
	//Check for integer overflow
	/*if (Value > 0 && registerFile[RegFileIndex] > INT_MAX / Value) {
		std::runtime_error OverflowException("0002Integer overflow");	
		throw(OverflowException);
	}*/
	registerFile[RegFileIndex] *= Value;
	//set_Status_Flag(registerFile[RegFileIndex]);
}

void DivRegFileValue(unsigned char RegFileIndex, int Value) {
	//Check for divide-by-zero
	if (Value == 0) {
		std::runtime_error DivideByZeroException("0001Divide by zero");	
		throw(DivideByZeroException);
	}
	registerFile[RegFileIndex] /= Value;
	//set_Status_Flag(registerFile[RegFileIndex]);
}

Boolean isZeroStatus() {
	if (srRegister & 1)
		return TRUE;
	return FALSE;
}

Boolean isNegativeStatus() {
	if (srRegister & 2)
		return TRUE;
	return FALSE;
}

void SaveContextOnStack(int Addr) {
	//Save the return address
	programStack[stackFrameReg + 1] = Addr;
}

void RestoreContextOnStack() {
	int retValue;

	if ((spRegister - stackFrameReg) == 3) {
		//This is a function return
		//Pop the function return value
		PopFromStack(&retValue);
		//Pop return address
		PopFromStack(&pcRegister);
		//Pop stack frame reg value
		PopFromStack(&stackFrameReg);
		//Push back the return value onto stack
		PushOntoStack(retValue);
	}
	else {
		//Pop return address
		PopFromStack(&pcRegister);
		//Pop stack frame reg value
		PopFromStack(&stackFrameReg);
	}
}

int ReadCodeWord() {
	unsigned int data;
	unsigned int d;
	int res;
	
	data = codeMemory[pcRegister++] << 8;
	data += codeMemory[pcRegister++];
	if (data & 0x8000) {
		//Negative number - convert to signed integer format
		d = (long)65536 - data;
		res = -1 * d;
	}
	else {
		//Not a negative number
		res = data;
	}
	
	return res;
}


Boolean WriteMemoryByte(short int offset, unsigned char byteVal) {
	if (offset < DATA_MEMORY_SIZE) {
		dataMemory[offset] = byteVal;
		return TRUE;
	}

	RunTimeError("Out of memory on write memory byte");

	return FALSE;
}

Boolean ReadMemoryByte(short int offset, unsigned char *byteVal) {
	if (offset < DATA_MEMORY_SIZE) {
		*byteVal = dataMemory[offset];
		return TRUE;
	}

	RunTimeError("Out of memory on read memory byte");

	return FALSE;
}

Boolean WriteMemoryWord(short int offset, int wordVal) {
	if (WriteMemoryByte(offset, (unsigned char)VAR_TYPE_INTEGER)) {
		if (WriteMemoryByte(offset + 1, (unsigned char)(wordVal >> 8))) {
			return WriteMemoryByte(offset + 2, (unsigned char)(wordVal & 0xFF));
		}
	} else {
		return FALSE;
	}
	
	return FALSE;
}

Boolean ReadMemoryWord(short int offset, int *wordVal) {
	unsigned char b0, b1;
	short int w;

	if (ReadMemoryByte(offset++, &b0)) {
		if (b0 == (unsigned char) VAR_TYPE_INTEGER || b0 == (unsigned char) VAR_TYPE_BYTE) {
			if (ReadMemoryByte(offset, &b1)) {
				if (ReadMemoryByte(offset + 1, &b0)) {
					w = (short int)((b1 << 8) + b0);
					*wordVal = (int)w;
					return TRUE;
				}
			}
		} else {
			RunTimeError("Data descriptor type integer was expected");
			return FALSE;
		}
	} else {
		return FALSE;
	}

	return FALSE;
}

Boolean WriteMemoryString(char *dp, int *offset, int size) {
	int i;
	Boolean writeRes = TRUE;

	if (WriteMemoryByte((*offset)++, (unsigned char)VAR_TYPE_STRING)) {
		for (i = 0; i < size; i++) {	
				if (!WriteMemoryByte((*offset)++, dp[i])) {
					writeRes = FALSE;
					break;
				}
		}
	} else {
			writeRes = FALSE;
	}

	return writeRes;
}

Boolean ReadMemoryString(char *dp, int *offset, int size) {
	int i;
	Boolean readRes = TRUE;
    unsigned char *uc;
    
	if (ReadMemoryByte((*offset)++, uc)) {
		if (*uc == (unsigned char)VAR_TYPE_STRING) {
			for (i = 0; i < size; i++) {
				uc = (unsigned char *)(dp + i);
				if (!ReadMemoryByte((*offset)++, uc)) {
					readRes = FALSE;
					break;
				}
			}
		} else {
			readRes = FALSE;
		}
	} else {
		readRes = FALSE;
	}

	return readRes;
}

Boolean ReadMemoryStringZ(char *dp, int *offset) {
	int i;
	Boolean readRes = TRUE;
    unsigned char *uc;
    
	if (ReadMemoryByte((*offset)++, uc)) {
		if (*uc == (unsigned char)VAR_TYPE_STRING) {
			i = 0;
			do {
				uc = (unsigned char *)(dp + i);
				if (!ReadMemoryByte((*offset)++, uc)) {
					readRes = FALSE;
					break;
				}
				i++;
			} while (uc);
		} else {
			readRes = FALSE;
		}
	} else {
		readRes = FALSE;
	}

	return readRes;
}

Boolean CopyMemoryStringZ(int *offsetSrc, int *offsetDest) {
	Boolean copyRes = TRUE;
    unsigned char uc;
	int offset1 = *offsetSrc;
	int offset2 = *offsetDest;

	do {
		if (!ReadMemoryByte(offset1, &uc)) {
			copyRes = FALSE;
			break;
		}

		if (!(uc)) break;

		if (!WriteMemoryByte(offset2, uc)) {
			copyRes = FALSE;
			break;
		}
		offset1++;
		offset2++;
	} while (TRUE);

	return copyRes;
}

unsigned char ReadRegIndirect() {
	int data;
	unsigned char val;

	data = GetRegFileValue(codeMemory[pcRegister++]);
	ReadMemoryByte(data, &val);

	return val;
}

unsigned char ReadRegIndirectWithAutoInc() {
	int data;
	unsigned char val;
	unsigned char reg;

	reg = codeMemory[pcRegister++];
	data = GetRegFileValue(reg);
	ReadMemoryByte(data, &val);
	//Increment the indirect address
	PutRegFileValue(reg, data + 1);

	return val;
}

void WriteRegIndirect(unsigned char val) {
	int data;

	data = GetRegFileValue(codeMemory[pcRegister++]);
	WriteMemoryByte(data, val);
}

void WriteRegIndirectWithAutoInc(unsigned char val) {
	int data;
	unsigned char reg;

	reg = codeMemory[pcRegister++];
	data = GetRegFileValue(reg);
	WriteMemoryByte(data, val);
	//Increment the indirect address
	PutRegFileValue(reg, data + 1);
}

unsigned char ReadMemDirect() {
	int data;
	unsigned char val;

	data = ReadCodeWord();
	ReadMemoryByte(data, &val);

	return val;
}

void WriteMemDirect(unsigned char val) {
	int data;

	data = ReadCodeWord();
	WriteMemoryByte(data, val);
}

unsigned char ReadMemIndirect() {
	int data;
	unsigned char val;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data);
	ReadMemoryByte(data, &val);

	return val;
}

unsigned char ReadMemIndirectWithAutoInc() {
	int data;
	int data1;
	unsigned char val;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data1);
	ReadMemoryByte(data1, &val);
	//Increment the indirect mem address
	WriteMemoryWord(data, data1 + 1);

	return val;
}

void WriteMemIndirect(unsigned char val) {
	int data;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data);
	WriteMemoryByte(data, val);
}

void WriteMemIndirectWithAutoInc(unsigned char val) {
	int data;
	int data1;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data1);
	WriteMemoryByte(data1, val);
	//Increment the indirect mem address
	WriteMemoryWord(data, data1 + 1);
}

int ReadRegWordIndirect() {
	int data;
	int val;

	data = GetRegFileValue(codeMemory[pcRegister++]);
	ReadMemoryWord(data, &val);

	return val;
}

int ReadRegWordIndirectWithAutoInc() {
	int data;
	int val;
	unsigned char reg;

	reg = codeMemory[pcRegister++];
	data = GetRegFileValue(reg);
	ReadMemoryWord(data, &val);
	//Increment address in register
	PutRegFileValue(reg, data + 1);

	return val;
}

//Same as ReadRegWordIndirect but passes the memory address in the parameter
int ReadRegWordIndirectGetMemAddr(int *memAddr) {
	int data;
	int val;

	data = GetRegFileValue(codeMemory[pcRegister++]);
	*memAddr = data;
	ReadMemoryWord(data, &val);

	return val;
}

void WriteRegWordIndirect(int val) {
	int data;

	data = GetRegFileValue(codeMemory[pcRegister++]);
	WriteMemoryWord(data, val);
}

void WriteRegWordIndirectWithAutoInc(int val) {
	int data;
	unsigned char reg;

	reg = codeMemory[pcRegister++];
	data = GetRegFileValue(reg);
	WriteMemoryWord(data, val);
	//Increment the indirect address
	PutRegFileValue(reg, data + 1);
}

int ReadMemWordDirect() {
	int data;
	int val;

	data = ReadCodeWord();
	ReadMemoryWord(data, &val);

	return val;
}

//Same as ReadMemWordDirect but passes the memory address in the parameter
int ReadMemWordDirectGetMemAddr(int *memAddr) {
	int data;
	int val;

	data = ReadCodeWord();
	*memAddr = data;
	ReadMemoryWord(data, &val);

	return val;
}

void WriteMemWordDirect(int val) {
	int data;

	data = ReadCodeWord();
	WriteMemoryWord(data, val);
}

int ReadMemWordIndirect() {
	int data;
	int val;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data);
	ReadMemoryWord(data, &val);

	return val;
}

int ReadMemWordIndirectWithAutoInc() {
	int data;
	int data1;
	int val;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data1);
	ReadMemoryWord(data1, &val);
	//Increment memory location holding address
	WriteMemoryWord(data, data1 + 1);

	return val;
}

//Same as ReadMemWordIndirect but passes the memory address in the parameter
int ReadMemWordIndirectGetMemAddr(int *memAddr) {
	int data;
	int val;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data); 
	*memAddr = data;
	ReadMemoryWord(data, &val);

	return val;
}

void WriteMemWordIndirect(int val) {
	int data;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data);
	WriteMemoryWord(data, val);
}

void WriteMemWordIndirectWithAutoInc(int val) {
	int data;
	int data1;

	data = ReadCodeWord();
	ReadMemoryWord(data, &data1);
	WriteMemoryWord(data1, val);
	//Increment the indirect mem address
	WriteMemoryWord(data, data1 + 1);
}

//Suspend for wait seconds
void sleep(int wait){
   clock_t goal;
   goal = (clock_t)wait * CLOCKS_PER_SEC + clock();
   while(goal > clock()) {}
}

#ifdef EXECUTABLE_VERSION
Boolean VerifyCodeFormat(int size) {
	return TRUE;
}
#else
Boolean VerifyCodeFormat(int size) {
	Boolean badInst = FALSE;
	int pcRegister = 0;
	unsigned char opCode;
	unsigned char addrMode;

	while (pcRegister < size) {

		opCode = codeMemory[pcRegister++];

		//printf("--> %d@%d\n", opCode, pcRegister-1);

		switch (opCode) {
		case VM_INST_MOV:
			addrMode = codeMemory[pcRegister++];
			switch (addrMode) {
			case ADDR_MODE_IMMEDIATE:
				pcRegister += 2;
				break;
			case ADDR_MODE_DIRECTREG:
				pcRegister++;
				break;
			default:
				badInst = TRUE;
				break;
			}
			if (!badInst) {
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
				case ADDR_MODE_DIRECTREG:
					pcRegister++;
					break;
				default:
					badInst = TRUE;
					break;
				}
			}
			break;

		case VM_INST_PSHL:
		case VM_INST_POPL:
			addrMode = codeMemory[pcRegister++];
			switch (addrMode) {
				case ADDR_MODE_DIRECTMEM:
					pcRegister += 2;
					break;
				default:
					badInst = TRUE;
					break;
			}
			if (!badInst) {
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_IMMEDIATE:
						pcRegister += 2;
						break;
					default:
						badInst = TRUE;
						break;
				}
			}
			break;

			case VM_INST_SWP:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_DIRECTREG:
						pcRegister++;
						break;
					default:
						badInst = TRUE;
						break;
				}
				if (!badInst) {
					addrMode = codeMemory[pcRegister++];
					switch (addrMode) {
						case ADDR_MODE_DIRECTREG:
							pcRegister++;
							break;
						default:
							badInst = TRUE;
							break;
					}
				}
				break;

			case VM_INST_LDB:
			case VM_INST_LDW:
			case VM_INST_LNS:
			case VM_INST_LDX:
			case VM_INST_LDBI:
			case VM_INST_LDWI:
			case VM_INST_TAS:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_DIRECTMEM:
					case ADDR_MODE_INDIRECTMEM:
						pcRegister += 2;
						break;
					case ADDR_MODE_INDIRECTREG:
						pcRegister++;
						break;
					default:
						badInst = TRUE;
						break;
				}
				if (!badInst) {
					addrMode = codeMemory[pcRegister++];
					switch (addrMode) {
						case ADDR_MODE_DIRECTREG:
							pcRegister++;
							break;
						default:
							badInst = TRUE;
							break;
					}
				}
				break;

			case VM_INST_STB:
			case VM_INST_STW:
			case VM_INST_STBI:
			case VM_INST_STWI:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_IMMEDIATE:
						pcRegister += 2;
						break;
					case ADDR_MODE_DIRECTREG:
						pcRegister++;
						break;
					default:
						badInst = TRUE;
						break;
				}
				if (!badInst) {
					addrMode = codeMemory[pcRegister++];
					switch (addrMode) {
						case ADDR_MODE_DIRECTMEM:
						case ADDR_MODE_INDIRECTMEM:
							pcRegister += 2;
							break;
						case ADDR_MODE_INDIRECTREG:
							pcRegister++;
							break;
						default:
							badInst = TRUE;
							break;
					}
				}
				break;

			case VM_INST_PSH:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_IMMEDIATE:
						pcRegister += 2;
						break;
					case ADDR_MODE_DIRECTREG:
					case ADDR_MODE_INDIRECTREG:
						pcRegister++;
						break;
					default:
						badInst = TRUE;
						break;
				}
				break;

			case VM_INST_POP:
			case VM_INST_INC:
			case VM_INST_DEC:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_DIRECTREG:
						pcRegister++;
						break;
					default:
						badInst = TRUE;
						break;
				}
				break;

			case VM_INST_ADD:
			case VM_INST_SUB:
			case VM_INST_SUBU:
			case VM_INST_MUL:
			case VM_INST_DIV:
			case VM_INST_AND:
			case VM_INST_OR:
			case VM_INST_NOT:
			case VM_INST_SHL:
			case VM_INST_SHR:
			case VM_INST_ASR:
			case VM_INST_ASL:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_IMMEDIATE:
						pcRegister += 2;
						break;
					case ADDR_MODE_DIRECTREG:
						pcRegister++;
						break;
					default:
						badInst = TRUE;
						break;
				}
				if (!badInst) {
					addrMode = codeMemory[pcRegister++];
					switch (addrMode) {
						case ADDR_MODE_DIRECTREG:
							pcRegister++;
							break;
						default:
							badInst = TRUE;
							break;
					}
				}
				break;
	
			case VM_INST_JMP:
			case VM_INST_JEQ:
			case VM_INST_JZR:
			case VM_INST_JNE:
			case VM_INST_JNZ:
			case VM_INST_JGT:
			case VM_INST_JGE:
			case VM_INST_JLT:
			case VM_INST_JLE:
			case VM_INST_CAL:
			case VM_INST_LOOP:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_DIRECTMEM:
						pcRegister += 2;
						break;
					case ADDR_MODE_DIRECTREG:					//R03
					case ADDR_MODE_RELADDRESS:			        //+/-100
					case ADDR_MODE_RELREG_ADDRESS:		        //+/-R01
					case ADDR_MODE_RELINDREG_ADDRESS:		    //+/-@R01
						pcRegister++;		
						break;
					default:
						badInst = TRUE;
						break;
				}

				if (opCode == VM_INST_LOOP) {
					addrMode = codeMemory[pcRegister++];
					switch (addrMode) {
						case ADDR_MODE_DIRECTREG:
							pcRegister++;
							break;
						default:
							badInst = TRUE;
							break;
					}
				}
				break;

			case VM_INST_RET:
			case VM_INST_MSF:
			case VM_INST_IRET:
			case VM_INST_HLT:
				break;

			case VM_INST_SWI:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_DIRECTMEM:
						pcRegister += 2;
						break;
				}
				break;

			case VM_INST_MVS:
			case VM_INST_CPS:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_DIRECTMEM:
					case ADDR_MODE_INDIRECTMEM:
						pcRegister += 2;
						break;
					case ADDR_MODE_INDIRECTREG:
						pcRegister++;
						break;
				}
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_DIRECTMEM:
					case ADDR_MODE_INDIRECTMEM:
						pcRegister += 2;
						break;
					case ADDR_MODE_INDIRECTREG:
						pcRegister++;
						break;
				}
				break;

			case VM_INST_CMP:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_IMMEDIATE:
						pcRegister += 2;
						break;
					case ADDR_MODE_DIRECTREG:
						pcRegister++;
						break;
					default:
						badInst = TRUE;
						break;
				}
				if (!badInst) {
					addrMode = codeMemory[pcRegister++];
					switch (addrMode) {
						case ADDR_MODE_DIRECTREG:
							pcRegister++;
							break;
						default:
							badInst = TRUE;
							break;
					}
				}
				break;

			case VM_INST_IN	:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_DIRECTMEM:
						pcRegister += 2;
						break;
					default:
						badInst = TRUE;
						break;
				}
				if (!badInst) {
					addrMode = codeMemory[pcRegister++];
					switch (addrMode) {
						case ADDR_MODE_DIRECTREG:
							pcRegister++;
							break;
						case ADDR_MODE_DIRECTMEM:
							pcRegister += 2;
							break;
						default:
							badInst = TRUE;
							break;
					}
				}
				break;

			case VM_INST_OUT:
				addrMode = codeMemory[pcRegister++];
				switch (addrMode) {
					case ADDR_MODE_IMMEDIATE:
					case ADDR_MODE_DIRECTMEM:
					case ADDR_MODE_INDIRECTMEM:
						pcRegister += 2;
						break;
					case ADDR_MODE_DIRECTREG:
					case ADDR_MODE_INDIRECTREG:
						pcRegister++;
						break;
					default:
						badInst = TRUE;
						break;
				}
				if (!badInst) {
					addrMode = codeMemory[pcRegister++];
					switch (addrMode) {
						case ADDR_MODE_DIRECTMEM:
							pcRegister += 2;
							break;
						default:
							badInst = TRUE;
							break;
					}
				}
				break;

			default:
				badInst = TRUE;
				break;
		}

		if (badInst) {
			printf("Illegal instruction %u at address %u\n", opCode, pcRegister-1);
			return FALSE;
		}
	}

	return TRUE;
}
#endif

Boolean LoadCodeFile(FILE *fp) {
	short int n = 0, p = 0;
	unsigned char i;
	short int size = 0;
	int offset = 0;
	int dataType = 0;
	int dataValue = 0;
	unsigned char isArray = '\0';
	char *data;
	unsigned char intNo;
	short int intAddr = 0;
	Boolean loadRes = TRUE;

    int multiDim = 0;
    int arrayAddr = 0;
    int arraySize = 0;
    int colSize = 0;
    int rowSize = 0;

    //Code file format:
	//     <Code file signature>
    //     <Prog name size in bytes>
    //     <Prog name bytes>

	//		***<Library flag byte>
	//		***<Data byte offset>

	//     <Start Address>
	//     0 <Code Size in Bytes>
    //     <Code Bytes>
    //     <Code Checksum>
    //     <Intr vector entry count>
    //     2 <Intr No> <Intr Addr>
    //     ...

    //     ***<Initialisation Data Entry Count>
    //     ***5 <Data offset> <Data Type> <Data Value> <IsArray>
    //     ***...
    //     ***<Array Initialisation Data Entry Count>
    //     ***6 <MultiDim> <Array Addr> <Array Size> <Array Col Size> <Array Row Size>
    //     ***...

	//     <String Data Entry Count>
    //     1 <String Size in Bytes> <String Byte Offset> <String Data Bytes>
    //		...
    //     <Guard table size>
    //     3 <Start address> <End address>
    //     ...

	//     ***<Library subroutine list Size>
    //     ***4 <Export flag> <Name> <Address>
    //     ***...

	try {
		//Used only for debugging!
		//printf("***Loading code file\n");

		//Get code file signature
		fread(&n, sizeof(short int), 1, fp);

		if (n == CODE_SIGNATURE) {
			//Get prog name size
			fread(&n, sizeof(short int), 1, fp);
			//Get space for prog name
			progName = (char *)malloc(n + 1);
			//Get prog name
			fread(progName, n, 1, fp);
			progName[n] = '\0';

			//Get library flag value
			fread(&isLibraryProg, sizeof(unsigned char), 1, fp);
			//Get data byte offset
			//fread(&memoryPageOffset, sizeof(long), 1, fp);
			fread(&memoryPageOffset, sizeof(short int), 1, fp);

			//Get start address
			fread(&startAddress, sizeof(short int), 1, fp);

			//Get code flag value
			fread(&i, sizeof(unsigned char), 1, fp);
			if (i == 0) {
				//Get code size in bytes
				fread(&n, sizeof(short int), 1, fp);
				//Get space for code data
				codeMemory = (unsigned char *)malloc(n);
				//Get code data
				fread(codeMemory, n, 1, fp);

				//Check code validity
				if (VerifyCodeFormat(n)) {
					//Get checksum value
					fread(&checksum, sizeof(int), 1, fp);

					//Allocate space for data memory area
					dataMemory = (unsigned char *)malloc(DATA_MEMORY_SIZE);

					//Intialise the memory data area
					for (p = 0; p < DATA_MEMORY_SIZE; p++) {
						dataMemory[p] = 0;
					}
					//Initialise the semaphore lock memory word - lock is initially free!
					WriteMemoryWord(0, 0);

					//Initialise int vector table
					for (n = 0; n < MAXINTREQUESTS; n++) {
						IntVectorAddr[n] = -1;
					}

					//Get interrupt vector entry count
					fread(&n, sizeof(short int), 1, fp);
					if (n > 0) {
						for (p = 0; p < n; p++) {
							//Get interrupt flag value
							fread(&i, sizeof(unsigned char), 1, fp);
							if (i == 2) {
								//Get data size in bytes
								fread(&intNo, sizeof(unsigned char), 1, fp);
								//Get data byte offset
								fread(&intAddr, sizeof(short int), 1, fp);
								//Store in intr vector table
								IntVectorAddr[intNo] = intAddr;
							}
						}
					}

					//Get string data entry count
					fread(&n, sizeof(short int), 1, fp);
					if (n > 0) {
						for (p = 0; p < n; p++) {
							//Get data flag value
							fread(&i, sizeof(unsigned char), 1, fp);
							if (i == 1) {
								//Get data size in bytes
								fread(&size, sizeof(short int), 1, fp);
								//Get data byte offset
								fread(&offset, sizeof(short int), 1, fp);
								//Make room for string data
								data = (char *)malloc(size);
								//Get string data bytes
								fread(data, size, 1, fp);
								//Need to skip the 0/ at the end of the string
								fread(&i, sizeof(unsigned char), 1, fp);

								//printf("***Offset: %d, Size: %d, Data: %s\n", (short int)offset, size, data);

								//Store in memory
								if (!WriteMemoryString(data, &offset, size)) {
									//printf("Write to memory failed at %d\n", offset);
									free(data);
									//printf("Data: %s; Offset: %d, Size: %d\n", data, offset, size);
									throw("Write to memory failed [0]");
									break;
								}
								free(data);
							}
						}
					}

					//Get initialization data entry count
					fread(&n, sizeof(short int), 1, fp);
					if (n > 0) {
						for (p = 0; p < n; p++) {
							//Get initialization data flag value
							fread(&i, sizeof(unsigned char), 1, fp);
							if (i == 5) {
								//Get data offset in bytes
								fread(&offset, sizeof(short int), 1, fp);
								//Get data type
								fread(&dataType, sizeof(short int), 1, fp);
								//Get data value
								fread(&dataValue, sizeof(short int), 1, fp);
								//Get data array flag
								fread(&isArray, sizeof(unsigned char), 1, fp);
								//printf("offset = %d, dataType = %d, dataValue = %d, isArray = %d\n", offset, dataType, dataValue, isArray);

								//printf("***Offset: %d, Type: %d, Value: %d\n", (short int)offset, dataType, dataValue);

								switch (dataType) {
									case VAR_TYPE_INTEGER:
										//Write it in destination location
										if (!WriteMemoryWord(offset, dataValue)) {
											//printf("Write to memory failed at %d\n", offset);
											throw("Write to memory failed [1]");
											//loadRes = FALSE;
											break;
										}
										break;
									case VAR_TYPE_BYTE:
									case VAR_TYPE_BOOLEAN:
										//Get data value
										//Write it in destination location
										if (!WriteMemoryByte(offset, dataValue)) {
											//printf("Write to memory failed at %d\n", offset);
											throw("Write to memory failed [3]");
											break;
										}
										break;
									case VAR_TYPE_STRING:
										//Get source data offset
										if (isArray) {
											//Pointers in array to string data
											WriteMemoryWord(offset, dataValue);
										}
										else {
											//Copy null terminated string from source location to destination location in memory
											if (!CopyMemoryStringZ(&dataValue, &offset)) {
												//printf("Copy to memory failed at %d\n", offset);
												throw("Copy to memory failed");
												break;
											}
										}
										break;
									default:
										throw("Bad memory initialization data type");
										break;
								}
							}
						}
					}

					//Get array data initialization entry count
					fread(&n, sizeof(short int), 1, fp);
					if (n > 0) {
						for (p = 0; p < n; p++) {
							//Get array data flag value
							fread(&i, sizeof(unsigned char), 1, fp);
							if (i == 6) {
								//<MultiDim> <Array Addr> <Array Size> <Array Col Size> <Array Row Size>
								//Get multi dimensional array flag
								fread(&multiDim, sizeof(short int), 1, fp);
								//Get array start of data address
								fread(&arrayAddr, sizeof(short int), 1, fp);

								if (multiDim) {
									if (!WriteMemoryWord((short int)arrayAddr, (short int)arrayAddr + 12)) {
										//printf("Write to memory failed at %d\n", offset);
										throw("Write to memory failed [4]");
									}
								}
								else {
									if (!WriteMemoryWord((short int)arrayAddr, (short int)arrayAddr + 6)) {
										//printf("Write to memory failed at %d\n", offset);
										throw("Write to memory failed [5]");
									}
								}

								//Get array size
								fread(&arraySize, sizeof(short int), 1, fp);
								if (!WriteMemoryWord((short int)arrayAddr + 3, (short int)arraySize)) {
									//printf("Write to memory failed at %d\n", offset);
									throw("Write to memory failed [6]");
								}

								//printf("***Addr: %d, Size: %d\n", (short int)arrayAddr, (short int)arraySize);

								//Get array column size
								fread(&colSize, sizeof(short int), 1, fp);
								//Get array row size
								fread(&rowSize, sizeof(short int), 1, fp);

								if (multiDim) {
									if (!WriteMemoryWord((short int)arrayAddr + 6, (short int)colSize)) {
										//printf("Write to memory failed at %d\n", offset);
										throw("Write to memory failed [7]");
									}
									if (!WriteMemoryWord((short int)arrayAddr + 9, (short int)rowSize)) {
										//printf("Write to memory failed at %d\n", offset);
										throw("Write to memory failed [8]");
									}
								}
							}
						}
					}

					//Get guard table entry count
					fread(&n, sizeof(short int), 1, fp);
					guardInfoPtr = n;

					if (n > 0) {
						//Allocate space for guard table
						guardTable = (struct guardTableInfo *) malloc(sizeof(guardTableInfo));

						for (p = 0; p < n; p++) {
							//Get data flag value
							fread(&i, sizeof(unsigned char), 1, fp);
							if (i == 3) {
								//Get guard entry start address
								fread(&guardTable[p].startAddr, sizeof(short int), 1, fp);
								//Get guard entry end address
								fread(&guardTable[p].endAddr, sizeof(short int), 1, fp);
							}
						}
					}

					if (isLibraryProg) {
						//Get Library subroutine list entry count
						fread(&n, sizeof(short int), 1, fp);
						if (n > 0) {
							for (p = 0; p < n; p++) {
								//Get subroutine list entry flag value
								fread(&i, sizeof(unsigned char), 1, fp);
								if (i == 4) {
									//<Export flag> <Name> <Address>
									//Get subroutine export flag
									//fread(&exportFlag, sizeof(short int), 1, fp);
									//Get subroutine name
									//fread(&subName, sizeof(short int), 1, fp);
									//Get subroutine address
									//fread(&subAddr, sizeof(short int), 1, fp);

								}
							}
						}
					}

				}
				else {
					throw("Bad code file [0]");    
				}
			}
		}
		else {
			throw("Bad code file [1]");
		}

	} catch (char * str) {
		printf("Code file load error: %s\n",str);
		loadRes = FALSE;
	}

	fclose(fp);

	//Used only for debugging!
	//printf("***Loading code file completed\n");

	return loadRes;
}

#ifndef EXECUTABLE_VERSION
Boolean AnalyzeCodeFile(FILE *fp) {
	FILE *ap;
	short int n, p;
	unsigned char i;
	short int size;
	short int offset;
	unsigned char *data;
	unsigned char intNo;
	short int intAddr;
	Boolean loadRes = TRUE;
    short codeSize;
    short b;
    int addr1, addr2;
    
    //Code file format:
	//     <Code file signature>
    //     <Prog name size in bytes>
    //     <Prog name bytes>

	//		***<Library flag byte>
	//		***<Data byte offset>

	//     <Start Address>
	//     0 <Code Size in Bytes>
    //     <Code Bytes>
    //     <Code Checksum>
    //     <Intr vector entry count>
    //     2 <Intr No> <Intr Addr>
    //     ...

    //     ***<Initialisation Data Entry Count>
    //     ***5 <Data offset> <Data Type> <Data Value> <IsArray>
    //     ***...
    //     ***<Array Initialisation Data Entry Count>
    //     ***6 <MultiDim> <Array Addr> <Array Size> <Array Col Size> <Array Row Size>
    //     ***...

	//     <String Data Entry Count>
    //     1 <String Size in Bytes> <String Byte Offset> <String Data Bytes>
    //		...
    //     <Guard table size>
    //     3 <Start address> <End address>
    //     ...

	//     ***<Library subroutine list Size>
    //     ***4 <Export flag> <Name> <Address>
    //     ***...

	/*if ((fp = fopen(fileName, "rb")) == NULL) {
        printf("Unable to open file %s\n", fileName);
		return FALSE;
	}*/
    
    if ((ap = fopen("Analysis.txt", "w")) == NULL) {
        printf("Unable to open file Analysis.txt for output\n");
        fclose(fp);
        return FALSE;  
    }
    
	//Get code file signature
	fread(&n, sizeof(short int), 1, fp);

	if (n == CODE_SIGNATURE) {
		//Get prog name size
		fread(&n, sizeof(short int), 1, fp);
		//Get space for prog name
		progName = (char *)malloc(n + 1);
		//Get prog name
		fread(progName, n, 1, fp);
		progName[n] = '\0';
        
		//Get start address
		fread(&startAddress, sizeof(short int), 1, fp);

		//Get code flag value
		fread(&i, sizeof(unsigned char), 1, fp);
		if (i == 0) {
			//Get code size in bytes
			fread(&n, sizeof(short int), 1, fp);
			codeSize = n;
			
			//Get space for code data
			codeMemory = (unsigned char *)malloc(n);
			//Get code data
			fread(codeMemory, n, 1, fp);

			//Check code validity
			if (VerifyCodeFormat(n)) {
                fprintf(ap, "Program name: %s\n", progName); 
                fprintf(ap, "Start address: %u\n", startAddress);
                fprintf(ap, "Code size: %u bytes\n\n", codeSize);
                             
                i = 0;
                //Display code data here
                fprintf(ap,"*** CODE:");
                for (b = 0; b < codeSize; b++) {
                    if (b % 16 == 0) {
                       fprintf(ap, "\n%.4u: ", b);  
                    }
                    
                    if (b % 2 == 0) {
                       fprintf(ap," ");
                    }
                    fprintf(ap,"%.2x", codeMemory[b]);    
                }
                              
                free(codeMemory);
                                     
				//Get checksum value - this is not displayed
				fread(&checksum, sizeof(long), 1, fp);

				//Get interrupt vector entry count
				fread(&n, sizeof(short int), 1, fp);

				if (n > 0) {
                     fprintf(ap, "\n\n*** Interrupt vectors:\n");
					//Get interrupt flag value
					fread(&i, sizeof(unsigned char), 1, fp);		
					if (i == 2) {
						//Get data size in bytes
						fread(&intNo, sizeof(unsigned char), 1, fp);
						//Get data byte offset
						fread(&intAddr, sizeof(short int), 1, fp);
						fprintf(ap, "Int No: %u  Int Addr: %u\n", intNo,intAddr);
					}
				}

				//Get data entry count
				fread(&n, sizeof(short int), 1, fp);

				if (n > 0) {
                    fprintf(ap, "\n\n*** DATA:\n");
					for (p = 0; p < n; p++) {
						//Get data flag value
						fread(&i, sizeof(unsigned char), 1, fp);		
						if (i == 1) {
							//Get data size in bytes
							fread(&size, sizeof(short int), 1, fp);
							//Get data byte offset
							fread(&offset, sizeof(short int), 1, fp);
							//Make room for string data
							data = (unsigned char *)malloc(size);
							//Get string data bytes
							fread(data, size, 1, fp);

                            fprintf(ap, "Offset: %u, Size: %u, Data: '%s'\n", offset, size - 1, data);
						}
					}
				}
				
				//Get guard table entry count
				fread(&n, sizeof(short int), 1, fp);
				
				if (n > 0) {
					fprintf(ap, "\n\n*** GUARD DATA:\n");		
					for (p=0; p < n; p++) {
						//Get data flag value
						fread(&i, sizeof(unsigned char), 1, fp);		
						if (i == 3) {
							//Get guard entry start address
							fread(&addr1, sizeof(int), 1, fp);
							//Get guard entry end address
							fread(&addr2, sizeof(int), 1, fp);
							
							fprintf(ap, "Start addr: %u, End addr: %u\n", addr1, addr2);
						}
					}					
				}
				
			}
			else {
                 printf("Bad code file\n");
                 loadRes = FALSE;
            }    
		}
	}
	else {
		printf("Bad code file\n");
		loadRes = FALSE;
	}

	fclose(fp);
    fclose(ap);
    
	return loadRes;
}
#endif