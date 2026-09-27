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
#include <stdexcept>
#include <string.h>
//#include <iostream>

#ifdef LINUX_PLATFORM
#include "Linux Support.h"
#else
#include <conio.h>
#endif

#include "Virtual Machine.h"
#include "Instruction Library.h"
#include "Support Library.h"

int registerFile[REGISTER_FILE_SIZE];
int programStack[PROG_STACK_SIZE];
struct readyQInfo threadReadyQueue[MAX_NO_OF_THREADS];

int pcRegister;							//Program counter register
int srRegister = 0;						//Status register
int spRegister = 0;						//Stack pointer register

unsigned char *dataMemory;
unsigned char *codeMemory;

char *progName;
short int startAddress = 0;
struct stringDataInfo *stringData;
struct guardTableInfo *guardTable;
int guardInfoPtr = 0;
long checksum = 0;

Boolean isLibraryProg;
long memoryPageOffset;

int IntVectorAddr[MAXINTREQUESTS];

int oldPCRegister;
unsigned char opCode;
int runResult = 0;
Boolean stopProg = FALSE;
int inputDataMemOffset = -1;

programListInfo programList[MAX_NO_OF_THREADS];
int currentThreadNo = 0;
int numberOfActiveThreads = 1;   //Includes the main thread
int stackFrameReg = -1;

//--------The following code is for debugging purposes only ----------
void displayRegFileValues(int start, int length) {
	for (int n = start; n <= start + length; n++) {
		printf("R%02d:\t%d\n", n, GetRegFileValue(n));
	}
}

void displayMemory(int start, int length) {
	unsigned char b;

	for (int n = start; n <= start + length; n++) {
		ReadMemoryByte(n, &b);
		printf("%02x", b);
	}
	printf("\n");
}

void debugAid(unsigned char opCode) {
	unsigned char c;
	int start;
	int length;

	//Used for debuging!
	printf("%d;%d: opCode %d @ %d\n", currentThreadNo, programList[currentThreadNo].RRTickCount, opCode, oldPCRegister);

	while (TRUE) {
		printf(">> ");
		while (!_kbhit()) {}
		c = _getch();
		if (c == '\r') break;
		printf("%c\n", c);
		switch (c) {
			case 'r':
				printf(": ");
				scanf("%d %d", &start, &length);
				//Display ALL register values
				displayRegFileValues(start, length);
				break;
			case 'm':
				printf(": ");
				scanf("%d %d", &start, &length);
				//Display the selected memory locations
				displayMemory(start, length);
				break;
			default:
				printf("Only r, m or q are allowed\n");
				fflush(stdin);
				break;
		}
	}
}
//-------------------------------------------------------------------------

int RunProgram() {
    //unsigned char opCode;
    //int oldPCRegister;
    int i;
	unsigned char c;
	//unsigned char d;

	//Main Fetch, Decode, Execute cyle
	while (!stopProg) {

		oldPCRegister = pcRegister;
		opCode = codeMemory[pcRegister++];

		//Used for debuging!
		/*if (oldPCRegister >= 622) {
			debugAid(opCode);
		}*/

        try {
		    switch (opCode) {

    			case VM_INST_MOV:
    				do_MOV_Instruction();
    				break;
    
                case VM_INST_MVS:
    				do_MVS_Instruction();
    				break;
    
//    			case VM_INST_CVS:
//                    //Add code here
//                    break;
                     
//    			case VM_INST_CVI:
//                    //Add code here
//                    break;
                                      
                case VM_INST_LDB:
  				    do_LDB_Instruction(FALSE);
  				    break;

				case VM_INST_LDBI:
  				    do_LDB_Instruction(TRUE);
  				    break;

                case VM_INST_LDW:
				case VM_INST_LNS:
				    do_LDW_Instruction(FALSE);
				    break;

				case VM_INST_LDWI:
				    do_LDW_Instruction(TRUE);
				    break;

				case VM_INST_LDX:
        			do_LDX_Instruction();
        			break;

				case VM_INST_TAS:
					do_TAS_Instruction();
					break;
					
        		case VM_INST_STB:
        			do_STB_Instruction(FALSE);
        			break;

				case VM_INST_STBI:
        			do_STB_Instruction(TRUE);
        			break;

        		case VM_INST_STW:
        			do_STW_Instruction(FALSE);
        			break;

				case VM_INST_STWI:
        			do_STW_Instruction(TRUE);
        			break;

        		case VM_INST_PSH:
        			do_PSH_Instruction();
        			break;
        
        		case VM_INST_POP:
        			do_POP_Instruction();
        			break;
        
				case VM_INST_PSHL:
					do_PSHL_Instruction();
					break;
				
				case VM_INST_POPL:
					do_POPL_Instruction();
					break;

        		case VM_INST_ADD:
        			do_ADD_Instruction();
        			break;
        
        		case VM_INST_SUB:
        			do_SUB_Instruction();
        			break;

				case VM_INST_SUBU:
        			do_SUBU_Instruction();
        			break;

				case VM_INST_MUL:
					do_MUL_Instruction();
					break;
	
				case VM_INST_DIV:
					do_DIV_Instruction();
					break;

				case VM_INST_INC:
					do_INC_Instruction();
					break;

				case VM_INST_DEC:
					do_DEC_Instruction();
					break;

				case VM_INST_AND:
					do_AND_Instruction();
					break;

				case VM_INST_OR:
					do_OR_Instruction();
					break;

				case VM_INST_NOT:
					do_NOT_Instruction();
					break;

				case VM_INST_SHL:
					do_SHL_Instruction();
					break;

				case VM_INST_SHR:
					do_SHR_Instruction();
					break;

				case VM_INST_ASL:
					do_ASL_Instruction();
					break;

				case VM_INST_ASR:
					do_ASR_Instruction();
					break;

				case VM_INST_JMP:
					do_JMP_Instruction();
					break;
	
				case VM_INST_JEQ:
					do_JEQ_Instruction();
					break;

				case VM_INST_JZR:
					do_JZR_Instruction();
					break;

				case VM_INST_JNE:
					do_JNE_Instruction();
					break;

				case VM_INST_JNZ:
					do_JNZ_Instruction();
					break;

				case VM_INST_JGT:
					do_JGT_Instruction();
					break;
	
				case VM_INST_JGE:
					do_JGE_Instruction();
					break;
	
				case VM_INST_JLT:
					do_JLT_Instruction();
					break;
	
				case VM_INST_JLE:
					do_JLE_Instruction();
					break;

				case VM_INST_LOOP:
        			do_LOOP_Instruction();
        			break;

				case VM_INST_JSEL:
					do_JSEL_Instruction();
					break;
	
				case VM_INST_MSF:
					do_MSF_Instruction();
					break;

				case VM_INST_CAL:
					do_CAL_Instruction();
					break;
	
				case VM_INST_RET:
					do_RET_Instruction();
					break;
	
				case VM_INST_IRET:
					do_IRET_Instruction();
					break;
	
				case VM_INST_SWI:
					do_SWI_Instruction();
					break;
	
				case VM_INST_HLT:
					stopProg = TRUE;
					break;
	
				case VM_INST_CMP:
					do_CMP_Instruction();
					break;
	
				case VM_INST_CPS:
					do_CPS_Instruction();
					break;
	
				case VM_INST_IN	:
					do_IN_Instruction();
					break;
	
				case VM_INST_OUT:
					do_OUT_Instruction();
					break;
				
				case VM_INST_SWP:
					do_SWP_Instruction();
					break;

				case VM_INST_NOP:
					//Do nothing
					break;

				default:
					RunTimeError("Illegal instruction");
					break;
			}
		
        	if (IsTimerInterrupt()) {
           		//Need to handle Timer interrupt routine if one is available
           		if (IntVectorAddr[INTR_TIMER] != -1) {
                 	SaveContextOnStack(pcRegister);
                 	pcRegister = IntVectorAddr[INTR_TIMER];               
           		}
           		ResetTimerInterrupt();             
        	}
			
			if (threadReadyQueue[0].Valid) {
				if (--programList[currentThreadNo].RRTickCount == 0) {
					programList[currentThreadNo].RRTickCount=10;

					queueToReadyQ(currentThreadNo);
					SaveThreadContext(currentThreadNo);

					//Dequeue until a live thread is found (e.g. a queued thread's parent may have terminated)
					do {
						i = dequeueFromReadyQ();
						RestoreThreadContext(i);
					} while (!programList[currentThreadNo].Valid);
				}
			}

        } catch (const std::exception &X) {
			const char *e = X.what();
			char errNoStr[5];
			int errCode;

			//The first 4 characters are the error code number string
			//Extract the error no string
			for (i = 0; i < 4; i++) {
				errNoStr[i] = e[i];
			}
			errNoStr[i] = '\0';
			//Convert to integer error number
			errCode = atoi(errNoStr);

			//printf("Exception: %s, errCode = %d\n", e, errCode);

			//The first character indicates which exception type
			/*switch (errCode) {
				case EXCEPTION_OVERFLOW:
					//Integer overflow
					PushOntoStack(EXCEPTION_OVERFLOW);
					break;
				case EXCEPTION_DIV_BY_ZERO:
					//Divide by zero
					PushOntoStack(EXCEPTION_DIV_BY_ZERO);
					break;	
				default:
					//PushOntoStack(EXCEPTION_UNKNOWN);
					//PushOntoStack(errCode);
					break;						
			}*/
			
			if (IntVectorAddr[INT_EXCEPTION] != -1) {
				//printf("Is -1\n");

				//Exception handler available
				PushOntoStack(stackFrameReg);
				PushOntoStack(pcRegister);
				//SaveContextOnStack(pcRegister);
				//Make exception error number available
				PushOntoStack(errCode);
				//Invoke the exception handler
				pcRegister = IntVectorAddr[INT_EXCEPTION];
				//printf("Exception pcRegister = %d\n", pcRegister);
			}
            else { 				
				if (programList[currentThreadNo].guardInfoPtr > 0) {
					//Guard information available
					int jmpAddr;
					int exAddr;
					int n;
					
					//printf("guardInfoPtr = %d\n", programList[currentThreadNo].guardInfoPtr);

					//Make exception number available
					PushOntoStack(errCode);

					//Get exception instruction address
					exAddr = oldPCRegister;
						
        			jmpAddr = 32767; //Max +ve integer 			
        			//Scan the guard block address table
        			for (n = 0; n < programList[currentThreadNo].guardInfoPtr; n++) {
            			if (exAddr >= programList[currentThreadNo].guardTable[n].startAddr &&
                			exAddr < programList[currentThreadNo].guardTable[n].endAddr) {
                			//Find the innermost guard block
                			if (programList[currentThreadNo].guardTable[n].endAddr < jmpAddr) {
                    			jmpAddr = programList[currentThreadNo].guardTable[n].endAddr;
                			}
            			}
        			}	
            		//Now, jump to exception handler address - this is not a sub call!
            		//Stack will be adjusted by a planted instruction at that location - see generated code
            		pcRegister = jmpAddr;					
			    }
			    else {
					//No exception handler or guard info available, so just display runtime error message               
                	RunTimeError(&e[4]);  
				}
            }			
        }
        
	}
	

	//The following line is for testing only.
	//Suspends the console until a key is pressed.
	//printf("Please press the RETURN key to exit");
	//getchar();

	return runResult;
}

#ifdef EXECUTABLE_VERSION
Boolean AnalyseProgram(FILE *fp){
     return TRUE;
}
#else
Boolean AnalyseProgram(FILE *fp){
     return AnalyzeCodeFile(fp);
}
#endif

void InitializeMainThread(void) {
	pcRegister = startAddress;

    //Initialize rest of program list
    for (int i = 1; i < MAX_NO_OF_THREADS; i++) {
        programList[i].Valid = FALSE;
	}
	//Initialise thread ready Q
	for (int i = 0; i < MAX_NO_OF_THREADS; i++) {
		threadReadyQueue[i].Valid = FALSE;
	}

	programList[0].Valid = TRUE;
	programList[0].startAddr = startAddress;
	programList[0].priority = -1;
	programList[0].parentThreadNo = -1;
	programList[0].RRTickCount = 10;
	programList[0].waitForChild = FALSE;
	programList[0].guardTable = guardTable;
	programList[0].guardInfoPtr= guardInfoPtr;
	programList[0].stackFrameReg = -1;
	programList[0].inMutexWait = FALSE;
	programList[0].mutexFlag = FALSE;
	programList[0].semVarOffset = -1;
	programList[0].blockedWait = FALSE;
}

int main(int argc, char *argv[]) {
	char *codeFileName;
    char *exeFileName;
	unsigned char b;
	int n;
	int found;
	unsigned char marker[5];
	FILE *fp;

	//Used only for debugging!
	//printf("***Start of program\n");

	switch (argc) {
	case 1:
		exeFileName = argv[0];
		if ((fp = fopen(exeFileName, "rb")) == NULL) {
			printf("Unable to run program %s [0]\n", exeFileName);
			return ERROR_CODEFILE_LOAD;
		}

		found = FALSE;

		//Used only for debugging!
		//printf("***Locating the marker\n");

		while(!feof(fp)) {
			fread(&b, sizeof(unsigned char), 1, fp);
			if (b == 'v') {
				marker[0] = 'v';
				n = 1;
				while(!feof(fp)) {
					fread(&b, sizeof(unsigned char), 1, fp);
					marker[n] = b;
					if (n++ == 4) break;
				}
				if (marker[0] == 'v' && marker[1] == 'm' && marker[2] == 'e' && marker[3] == 'x' && marker[4] == 'e'){

					//Used only for debugging!
					//printf("***Found the marker\n");

					found = TRUE;
					if (LoadCodeFile(fp)) {
#ifdef LINUX_PLATFORM
						set_tty_raw();				/* Set up character-at-a-time */
#endif
						InitializeMainThread();
                   		runResult = RunProgram();
						break;
#ifdef LINUX_PLATFORM
						set_tty_cooked();			/* Restore normal TTY mode */
#endif
					}
					else {
						printf("Unable to run program %s [1]\n", exeFileName);
						return ERROR_CODEFILE_LOAD;
					}               
				}
			}
		}

		if (!found) {
			fclose(fp);
			printf("Unable to run program %s [2]\n", exeFileName);
			return ERROR_CODEFILE_LOAD;
		}
		break;

	case 3:
        switch (argv[1][0]) {
            case 'r':
				codeFileName = argv[2];
               //First, load the code
				if ((fp = fopen(codeFileName, "rb")) == NULL) {
					printf("Unable to open file %s\n", codeFileName);
					return FALSE;
				}
				if (LoadCodeFile(fp)) {
#ifdef LINUX_PLATFORM
					set_tty_raw();				/* Set up character-at-a-time */
#endif
					InitializeMainThread();
                   	runResult = RunProgram();
					break;
#ifdef LINUX_PLATFORM
					set_tty_cooked();			/* Restore normal TTY mode */
#endif
               }
		       else {
			       printf("Could not load the code from file %s\n", codeFileName);
                   return ERROR_CODEFILE_LOAD;
               }               
               break;
            case 'a':
				if ((fp = fopen(codeFileName, "rb")) == NULL) {
					printf("Unable to open file %s\n", codeFileName);
					return FALSE;
				}
				if (!AnalyseProgram(fp)) {
			       printf("Could not analyse the code from file %s\n", codeFileName);
                   return ERROR_CODEFILE_ANALYSIS;
               }                                                  
               break;
            default:
			     printf("Illegal parameter %s used\n", argv[1]);
			     printf("Usage: %s {r | a} <code file name>\n", argv[0]);
			     return ERROR_ILLEGAL_PARAMETER;                 
		}
		break;

	default:
		printf("Illegal parameter %s used\n", argv[1]);
		printf("Usage: %s {r | a} <code file name>\n", argv[0]);
		return ERROR_ILLEGAL_PARAMETER;                 
	}

	return runResult;
}
