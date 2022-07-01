// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------


void IncreaseProgramCounter()
{
    /*
	Doan code goc nam trong file machine/mipssim.cc
	
	
	registers[PrevPCReg] = registers[PCReg];	// for debugging, in case we
							// are jumping into lala-land
	registers[PCReg] = registers[NextPCReg];
	registers[NextPCReg] = pcAfter;
    */
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
}

// Input: Khong gian dia chi User(int) - gioi han cua buffer(int)
// Output: Bo nho dem Buffer(char*)
// Chuc nang: Sao chep vung nho User sang vung nho System
char* User2System(int virtAddr, int limit)
{
	int i; //chi so index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; //can cho chuoi terminal
	if (kernelBuf == NULL)
		return kernelBuf;
		
	memset(kernelBuf, 0, limit + 1);
	
	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Input: Khong gian vung nho User(int) - gioi han cua buffer(int) - bo nho dem buffer(char*)
// Output: So byte da sao chep(int)
// Chuc nang: Sao chep vung nho System sang vung nho User
int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void Exception_Halt()
{
    DEBUG('a', "\n Shutdown, initiated by user program.");
    printf("\n\n Shutdown, initiated by user program.");
    interrupt->Halt();
}

void Exception_ReadInt()
{
    /*int: [-2147483648 , 2147483647] --> max length = 11*/
    const int maxlen = 11;
    char num_string[maxlen] = {0};
    long long ret = 0;
    for (int i = 0; i < maxlen; i++) {
        char c = 0;
        gSynchConsole->Read(&c,1);
        if (c >= '0' && c <= '9') num_string[i] = c;
        else if (i == 0 && c == '-') num_string[i] = c;
        else break;
    }
    int i = (num_string[0] == '-') ? 1 : 0;
    while (i < maxlen && num_string[i] >= '0' && num_string[i] <= '9')
        ret = ret*10 + num_string[i++] - '0';
    ret = (num_string[0] == '-') ? (-ret) : ret;
    machine->WriteRegister(2, (int)ret);
}

void Exception_PrintInt()
{
    int n = machine->ReadRegister(4);
    /*int: [-2147483648 , 2147483647] --> max length = 11*/
    const int maxlen = 11;
    char num_string[maxlen] = {0};
    int tmp[maxlen] = {0}, i = 0, j = 0;
    if (n < 0) {
        n = -n;
        num_string[i++] = '-';
    }
    do {
        tmp[j++] = n%10;
        n /= 10;
    } while (n);
    while (j) 
    num_string[i++] = '0' + (char)tmp[--j];
    gSynchConsole->Write(num_string,i);
    machine->WriteRegister(2, 0);
}

void Exception_ReadChar()
{
    char c = 0;
    gSynchConsole->Read(&c,1);
    machine->WriteRegister(2, (int)c);
}

void Exception_PrintChar()
{
    char c = (char)machine->ReadRegister(4);
    gSynchConsole->Write(&c,1);
    machine->WriteRegister(2, 0);
}

void Exception_ReadString()
{
    int virtAddr, length;
    char* buffer;
    virtAddr = machine->ReadRegister(4); // Lay dia chi tham so buffer truyen vao tu thanh ghi so 4
    length = machine->ReadRegister(5); // Lay do dai toi da cua chuoi nhap vao tu thanh ghi so 5
    buffer = User2System(virtAddr, length); // Copy chuoi tu vung nho User Space sang System Space
    gSynchConsole->Read(buffer, length); // Goi ham Read cua SynchConsole de doc chuoi
    System2User(virtAddr, length, buffer); // Copy chuoi tu vung nho System Space sang vung nho User Space
    delete buffer; 
    return;
}

void Exception_PrintString()
{
    int virtAddr;
    char* buffer;
    virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
    buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255 ki tu
    int length = 0;
    while (buffer[length] != 0) length++; // Dem do dai that cua chuoi
    gSynchConsole->Write(buffer, length + 1); // Goi ham Write cua SynchConsole de in chuoi
    delete buffer;
    return;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which) {
    case NoException:
	return;

    case SyscallException:
	switch (type) {
	case SC_Halt:
	    Exception_Halt();
	    break;
	case SC_ReadInt:
	    Exception_ReadInt();
	    IncreaseProgramCounter();
	    break;
	case SC_PrintInt:
	    Exception_PrintInt();
	    IncreaseProgramCounter();
	    break;
	case SC_ReadChar:
	    Exception_ReadChar();
	    IncreaseProgramCounter();
	    break;
	case SC_PrintChar:
	    Exception_PrintChar();
	    IncreaseProgramCounter();
	    break;
	case SC_ReadString:
	    Exception_ReadString();
	    IncreaseProgramCounter();
	    break;
	case SC_PrintString:
	    Exception_PrintString();
	    IncreaseProgramCounter();
	    break;

	default:
	    IncreaseProgramCounter();
	}
	break;

    case PageFaultException:
	DEBUG('a', "\n No valid translation found");
	printf("\n\n No valid translation found");
	interrupt->Halt();
	break;

    case ReadOnlyException:
	DEBUG('a', "\n Write attempted to page marked read-only");
	printf("\n\n Write attempted to page marked read-only");
	interrupt->Halt();
	break;

    case BusErrorException:
	DEBUG('a', "\n Translation resulted invalid physical address");
	printf("\n\n Translation resulted invalid physical address");
	interrupt->Halt();
	break;

    case AddressErrorException:
	DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
	printf("\n\n Unaligned reference or one that was beyond the end of the address space");
	interrupt->Halt();
	break;

    case OverflowException:
	DEBUG('a', "\nInteger overflow in add or sub.");
	printf("\n\n Integer overflow in add or sub.");
	interrupt->Halt();
	break;

    case IllegalInstrException:
	DEBUG('a', "\n Unimplemented or reserved instr.");
	printf("\n\n Unimplemented or reserved instr.");
	interrupt->Halt();
	break;

    case NumExceptionTypes:
	DEBUG('a', "\n Num exception types");
	printf("\n\n Num exception types");
	interrupt->Halt();
	break;

    default:
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }
}








