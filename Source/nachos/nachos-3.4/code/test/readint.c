#include "syscall.h"

int main() 
{ 
    int result;
    // Nhap so tu nguoi dung
    PrintString("Nhap so: ");
    result = ReadInt();
    // In so vua nhap
    PrintString("So vua nhap la: ");
    PrintInt(result);
    Halt(); 
}
