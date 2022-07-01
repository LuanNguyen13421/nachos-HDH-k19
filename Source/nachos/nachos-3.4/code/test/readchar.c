#include "syscall.h"

int main() 
{ 
    char result;
    // Nhap ki tu tu nguoi dung
    PrintString("Nhap ki tu: ");
    result = ReadChar();
    // In ki tu vua nhap
    PrintString("Ki tu vua nhap la: ");
    PrintChar(result);
    Halt(); 
}
