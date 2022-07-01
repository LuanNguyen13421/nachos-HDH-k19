#include "syscall.h"
#include "copyright.h"

int main()
{
    int i;
    char _char;
    PrintString("\n-------------ASCII---------------\n");
    for(i = 0; i < 128; i++)
    {
	PrintChar('|');
	_char = (char)i;
	PrintInt(i);
	PrintString(" = ");
	PrintChar(_char);
	PrintString("|\n");
    }
    PrintString("\n-------------------------------------\n");
    Halt(); 
}
