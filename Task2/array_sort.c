/*Complexity Analysis:
 * -----------------------
 * Time Complexity: O(n). 
 *      Only two traversals of the array is needed.
 * Space Complexity: O(1). 
 *      As no extra space is required.
 * Approach:-
 * ---------- 
 *	Count the number of 0s, 1s and 2s in the given array. 
 *	Then store all the 0s in the beginning followed by all the 1s then all the 2s.
*/


#include <stdio.h>
 
void printArray(int *data, int Size)
{
    int Index;
    for (Index = 0; Index < Size; Index++)
        printf("%d ", data[Index]);
    printf("\n");
}
 
void sort012(int *data, int Size)
{
    int Index;
    int cnt0 = 0;
    int	cnt1 = 0;
    int	cnt2 = 0;
 
    for (Index = 0; Index < Size; Index++) {
        switch (data[Index]) {
        case 0:
            cnt0++;
            break;
        case 1:
            cnt1++;
            break;
        case 2:
            cnt2++;
            break;
        }
    }
 
    Index = 0;
 
    while (cnt0 > 0) {
        data[Index++] = 0;
        cnt0--;
    }
 
    while (cnt1 > 0) {
        data[Index++] = 1;
        cnt1--;
    }
 
    while (cnt2 > 0) {
        data[Index++] = 2;
        cnt2--;
    }
 
}

int main()
{
    int foo[30] = { 2,2,1,0,1,0,1,1,0,2,2,1,1,1,2,0,0,1,1,0,1,0,1,2,0,0,0,1,1,2 };
    int Size = sizeof(foo) / sizeof(foo[0]);
 
    sort012(foo, Size);
    printArray(foo, Size);

    return 0;
}

