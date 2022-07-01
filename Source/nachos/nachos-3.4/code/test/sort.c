/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"
#include "copyright.h"

int A[100];

int
main()
{
    int n, i, j, tmp, check;

    // Khoi tao mang tu nguoi dung
    do {
	PrintString("Nhap so phan tu cua mang (n<=100): ");
	n = ReadInt();
    } while (n > 100 || n < 0);

    for(i = 0; i < n; i++)
    {
	PrintString("array[");
	PrintInt(i);
	PrintString("]=");
	A[i] = ReadInt();
    }

    // Sap xep bang bubble sort
    // Lap cho den khi cac phan tu dung thu tu
    do
    {
	check = 0;
	for (i = 0; i < n - 1; i++)
		if (A[i] > A[i + 1])
		{
			tmp = A[i];
			A[i] = A[i + 1];
			A[i + 1] = tmp;
			check = 1;
		}
    } while (check == 1);
    
    // Xuat mang sau khi sap xep
    PrintString("Mang sau khi sap xep la:\n");
    for(i = 0; i < n; i++)
    {
	PrintInt(A[i]);
	PrintString(", ");
    }

    Halt();
}
