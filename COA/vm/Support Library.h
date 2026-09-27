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

#ifndef SUPP_LIB_H
#define SUPP_LIB_H

//Support Function Prototypes
void RunTimeError(const char *);
void SaveThreadContext(int);
void RestoreThreadContext(int);
Boolean PushOntoStack(int);
Boolean PopFromStack(int *);
void PutRegFileValue(unsigned char, int);
int GetRegFileValue(unsigned char);
void set_Status_Flag(int);
void AddToRegFileValue(unsigned char, int);
void SubFromRegFileValue(unsigned char, int);
void MulRegFileValue(unsigned char, int);
void DivRegFileValue(unsigned char, int);
Boolean isZeroStatus();
Boolean isNegativeStatus();
void SaveContextOnStack(int);
void RestoreContextOnStack();
Boolean LoadCodeFile(FILE *);
Boolean WriteMemoryByte(short int, unsigned char);
Boolean ReadMemoryByte(short int, unsigned char *);
Boolean WriteMemoryWord(short int, int);
Boolean ReadMemoryWord(short int, int *);
Boolean WriteMemoryString(char *, int *, int);
Boolean ReadMemoryString(char *, int *, int);
Boolean ReadMemoryStringZ(char *, int *);
int ReadCodeWord();
unsigned char ReadRegIndirect();
void WriteRegIndirect(unsigned char);
void WriteRegIndirectWithAutoInc(unsigned char);
unsigned char ReadMemDirect();
void WriteMemDirect(unsigned char);
unsigned char ReadMemIndirect();
void WriteMemIndirect(unsigned char);
void WriteMemIndirectWithAutoInc(unsigned char);
int ReadRegWordIndirect();
int ReadRegWordIndirectGetMemAddr(int *);
void WriteRegWordIndirect(int);
void WriteRegWordIndirectWithAutoInc(int);
int ReadMemWordDirect();
int ReadMemWordDirectGetMemAddr(int *);
void WriteMemWordDirect(int); 
int ReadMemWordIndirect();
int ReadMemWordIndirectGetMemAddr(int *);
void WriteMemWordIndirect(int);
void WriteMemWordIndirectWithAutoInc(int);
int ReadRegWordIndirectWithAutoInc();
int ReadMemWordIndirectWithAutoInc();
unsigned char ReadRegIndirectWithAutoInc();
unsigned char ReadMemIndirectWithAutoInc();
Boolean VerifyCodeFormat(int);
Boolean AnalyzeCodeFile(FILE *);
void sleep(int);

#endif
