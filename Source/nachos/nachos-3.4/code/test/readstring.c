#include "syscall.h"
#include "copyright.h"
#define LENGTH 255

int main() 
{ 
    char string[LENGTH];
    // Nhap chuoi ki tu tu nguoi dung
    PrintString("Nhap chuoi ki tu: ");
    ReadString(string, LENGTH);
    // In chuoi ki tu vua nhap
    PrintString("Chuoi vua nhap la: ");
    PrintString(string);
    Halt(); 
}
