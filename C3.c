#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n3,num;
    printf("Enter value of n3\n");
    scanf("%d", &n3);
    int sum = 0;
    FILE *fptr;

    // use appropriate location if you are using MacOS or Linux
    if ((fptr = fopen("C:\\Users\\Deepanshu Mody\\projects\\helloworld\\Test\\a.txt", "r")) == NULL)
    {
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    for (int i = 0; i < n3; i++)
    {
        fscanf(fptr, "%d", &num);
        sum += num;
    }

    printf("Value of sum=%d", sum);
    fclose(fptr);

    return 0;
}