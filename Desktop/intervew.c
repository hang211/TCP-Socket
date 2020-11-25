#include <stdio.h>
int main()
{
float a=7;float b=6;
int result =compare(a,b);
printf("%d",result);

}
int compare(float a,float b)
{
if ((a >= b) && (a<=b))
return 1;
return 0;

}

