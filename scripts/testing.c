#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ()
{
   unsigned short int* src;
   unsigned short int*  dest;
   int i;

   src = (unsigned short int*) malloc(sizeof(unsigned short int) * 100);
   dest = (unsigned short int*) malloc(sizeof(unsigned short int) * 25);

   for (i = 0; i < 100; i++) {
   	src[i] = i + 1;
   }
   for (i = 0; i < 100; i++) {
   	dest[i] = i + 1;
   }

   printf("src: ");
   for (i = 0; i < 100; i++) {
   	printf("%d ", src[i]);
   }
   printf("\n");

   memcpy(&src[25], &src[0], 25 * sizeof(unsigned short int));

   printf("dest: ");
   for (i = 0; i < 100; i++) {
   	printf("%d ", src[i]);
   }
   printf("\n");
   
   return(0);
}