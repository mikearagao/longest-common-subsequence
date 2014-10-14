/*
 ============================================================================
 Name        : lcs-serial.c
 Author      : André Garção
 Version     :
 Copyright   :
 Description : Sequencial implementation of the Longest Common Subsequence algoritm
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

int max(int a, int b) {
  return (a > b) ? a : b;
}

short cost(int x) {
  int i, n_iter = 20;
	double dcost = 0;
	for(i = 0; i < n_iter; i++)
		dcost += pow(sin((double)x),2) + pow(cos((double)x),2);
	return (short) (dcost / n_iter + 0.1);
}

/* Returns length of LCS for X[0..m-1], Y[0..n-1] */
void lcs( char *X, char *Y, int m, int n ) {
  // miguel int L[m+1][n+1];
  int** L;
  int i, j;

  L = (int**) malloc(sizeof(int*) * (m + 1));
  for(i = 0; i <= m; i++) {
    L[i] = (int*) malloc(sizeof(int) * (n + 1));
  }
  /*
  printf("O tamanho da 1ª string é %d e da 2ª string é %d\n", m, n);
  printf("A primeira string é: %s\n", X);
  printf("A segunda string é: %s\n", Y);
  */

  /* Following steps build L[m+1][n+1] in bottom up fashion. Note
  that L[i][j] contains length of LCS of X[0..i-1] and Y[0..j-1] */
  for(i=0; i<=m; i++) {
    for(j=0; j<=n; j++) {
      if (i == 0 || j == 0) {
    	 L[i][j] = 0;
      }
      else {
    	 if (X[i-1] == Y[j-1]) {
    	   L[i][j] = L[i-1][j-1] + cost(i);
     	 }
    	 else {
    		 L[i][j] = max(L[i-1][j], L[i][j-1]);
    	 }
      }
    }
  }

  /*
  printf("::::::::::: LCS TABLE :::::::::::\n");
  for(i=0; i<=m; i++)
  {
	  printf("|");
	  for(j=0; j<=n; j++)
	   {
	   printf(" %d |", L[i][j]);
	   }
	  printf("\n");
  }
  */

  // Following code is used to print LCS
  int index = L[m][n];

  // Create a character array to store the lcs string
  char lcs[index+1];
  lcs[index] = '\0'; // Set the terminating character

  // Start from the right-most-bottom-most corner and
  // one by one store characters in lcs[]
  i = m;
  j = n;
  while (i > 0 && j > 0) {
    // If current character in X[] and Y are same, then
    // current character is part of LCS
    if (X[i-1] == Y[j-1]) {
      lcs[index-1] = X[i-1]; // Put current character in result
      i--;
      j--;
      index--;     // reduce values of i, j and index
    }

    // If not same, then find the larger of two and
    // go in the direction of larger value
    // If equal go left (as statement rule says so)
    else if (L[i-1][j] >= L[i][j-1])
      i--;
    else
      j--;
  }

  /* Print the lcs */
  printf("%d\n%s\n", L[m][n], lcs);
  /*printf("LCS of %s and %s is %s\n", X, Y, lcs);*/
  for(i = 0; i <= m; i++) {
    free(L[i]);
  }
  free(L);
}

int main(int argc, char *argv[]) {
	FILE *fp;
	char *firstString, *secondString;
	int firstStringSize = 0;
	int secondStringSize = 0;


	/* Checks if there is only one argument when calling the executable */
  if(argc != 2) {
	 printf("Number of parameters is incorrect\n");
	 return 1;
	}

	/* Opens the file passed as parameter */
	fp = fopen(argv[1],"r");

	if( fp == NULL ) {
	 printf("Error while opening the file.\n");
	 return 1;
	}

	/* Reads form the file the size of both sequences */
	fscanf(fp, "%d %d", &firstStringSize, &secondStringSize);

	/* Allocates space for the two sequences in two pointers to char */
	firstString = (char*) malloc(sizeof(char) * (firstStringSize + 1));
	secondString = (char*) malloc(sizeof(char) * (secondStringSize + 1));

	/* Reads form the file both sequences */
	fscanf(fp, "%s %s", firstString, secondString);

  /*
	printf("O tamanho da 1ª string é %d e da 2ª string é %d\n", firstStringSize, secondStringSize);
	printf("A primeira string é: %s\n", firstString);
	printf("A segunda string é: %s\n", secondString);
	*/

	/* Close the file */
	//fclose(fp);

	/* Calling the function that compute the LCS (In the statement says: "In order for the
	 * solution be unique you should follow these rules: associate the first string with the
	 * rows (and the second with the columns); when moving backwards in the matrix to determine
	 *  the subsequence, in case up and left have the same vaue always move left." */
	lcs(secondString, firstString, secondStringSize, firstStringSize);

  free(secondString);
  free(firstString);

	return EXIT_SUCCESS;
}