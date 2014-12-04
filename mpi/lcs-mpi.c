/*
 ============================================================================
 Name        : lcs_serial.c
 Authors     : André Garção, Miguel Aragão, Miguel Oliveira
 Description : Sequencial implementation of the longest common subsequence algorithm
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <mpi.h>

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
void lcs( char *X, char *Y, int m, int n, int id, int p) {
  unsigned short int* L;
  unsigned short int* ghost;
  unsigned short int* aux_ghost;
  int i, j, process_height;
  double start; // time before lcs algorithm
  double end; // time after lcs algorithm
  double time; // total execution time
  int size = 1;
  int inc = 1;
  int start_id;
  int aux_iter, iter, pos_to_store;
  int x, y;
  int init_x, init_y;
  MPI_Status status;
  MPI_Request request;

  process_height = 0;
  // printf("n: %d m %d\n", n, m);
  if ((n % p) != 0) {
    process_height = (n / p) + 1;
  } else {
    process_height = (n / p);
  }
  // for(i = id; i < n; i = i + p)
    // process_height++;
  //for(i = 0; i <= m; i++) {
    //L[i] = (unsigned short int*) malloc(sizeof(unsigned short int) * (process_height));
    //ghost[i] = (unsigned short int*) malloc(sizeof(unsigned short int) * (process_height));
  //}

  L = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * process_height );
  ghost = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * process_height);
  if (id == 0) {
    aux_ghost = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * process_height);
  }

  // printf("My ID is: %d | | Total number of PC's equal to: %d\n", id, p);

  // printf("Max height %d procc %d\n", process_height, id);fflush;

  for (j = 0; j < ((m + 1) * process_height); j++) {
    ghost[j] = 0;
    if(id == 0)
      aux_ghost[j] = 0;
    L[j] = 0;
  }

  aux_iter = 0;

  for (i = 1; i < m + n; i++) {
    if (i >= m) {
      x = m;
      y = 1 + i - m;
    } else {
      x = i;
      y = 1;
    }

    if (i == m || i == n) {
      inc--;
    }

    if (i == m && i == n) {
      inc--;
    }

    if(i > m) {
      if((id - ((i - m) % p)) < 0) {
        start_id = (id - ((i - m) % p)) + p;
      } else {
        start_id = (id - ((i - m) % p));
      }
    } else {
      start_id = id;
    }

    init_x = x;
    init_y = y;

    iter = aux_iter;
    for (j = start_id; j < size; j = j + p) {
      x = init_x - j;
      y = init_y + j;
      //printf("HEY! anti-diagonal: %d | | j: %d | | procc: %d\n", i, j, id);
      pos_to_store = ((m + 1) * iter) + i - id - (p * iter);
      if ((pos_to_store + 1) % (m + 1) == 0)
        aux_iter++;
      // printf("procc: %d | | anti-diagonal: %d | | j: %d | | pos_to_store: %d\n", id, i, j, pos_to_store);fflush;
      if (X[x-1] == Y[y-1]) {
       L[pos_to_store] = ghost[pos_to_store-1] + cost(i);
      } else {
        L[pos_to_store] = max(L[pos_to_store-1], ghost[pos_to_store]);
      }
      iter++;
    }

    MPI_Isend(L, (m + 1) * process_height, MPI_UNSIGNED_SHORT, ((id + 1) % p), id, MPI_COMM_WORLD, &request);

    if (id == 0) {
      MPI_Recv(aux_ghost, (m + 1) * process_height, MPI_UNSIGNED_SHORT, ((id - 1) % p), ((id - 1) % p), MPI_COMM_WORLD, &status);
      for (j = 0; j < (m + 1); j++) {
        ghost[j] = 0;
      }
      for (j = (m + 1); j < (m + 1) * process_height; j++) {
        ghost[j] = aux_ghost[j - (m + 1)];
      }
    } else {
      MPI_Recv(ghost, (m + 1) * process_height, MPI_UNSIGNED_SHORT, ((id - 1) % p), ((id - 1) % p), MPI_COMM_WORLD, &status);
    }
    
    // MPI_Isend(&L, (m + 1) * process_height, MPI_UNSIGNED_SHORT, ((id + 1) % p), id, MPI_COMM_WORLD);

    size += inc;
    MPI_Barrier (MPI_COMM_WORLD);
  }

  // start = omp_get_wtime();
  /* Following steps build L[m+1][n+1] in bottom up fashion. Note
  that L[i][j] contains length of LCS of X[0..i-1] and Y[0..j-1] */
  /*for(i = 0; i <= m; i++) {
    L[i][0] = 0;
  }

  for(i = 0; i <= n; i++) {
    L[0][i] = 0;
  }

  for(i=1; i<=m; i++) {
    for(j=1; j<=n; j++) {
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
  }*/
  // end = omp_get_wtime();
  // time = end - start;
  //printf("For Time: %f\n", time);

  // Following code is used to print LCS
  /*int index = L[m][n];

  // Create a character array to store the lcs string
  char lcs[index+1];
  lcs[index] = '\0'; // Set the terminating character

  // Start from the right-most-bottom-most corner and
  // one by one store characters in lcs[]
  i = m;
  j = n;
  // start = omp_get_wtime();
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
  }*/
  // end = omp_get_wtime();
  // time = end - start;
  //printf("While Time: %f\n", time);

  /* Print the lcs */
  //printf("%d\n%s\n", L[m][n], lcs);
  for (j = 0; j < (m+1) * process_height; j++) {
    // printf("id: %d index: %d val: %d\n", id, j, L[j]);fflush;
  }

  /*for(i = 0; i <= m; i++) {
    free(L[i]);
  }*/
  free(L);
  free(ghost);
  if (id == 0) {
    free(aux_ghost);
  }
}

int main(int argc, char *argv[]) {
	FILE *fp;
	char *firstString, *secondString;
	int firstStringSize = 0;
	int secondStringSize = 0;
  double start; // time before lcs algorithm
  double end; // time after lcs algorithm
  double time; // total execution time
  int id, p;

  MPI_Init (&argc, &argv);

  MPI_Comm_rank (MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &p);

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

	/* Close the file */
	fclose(fp);

	/* Calling the function that compute the LCS (In the statement says: "In order for the
	 * solution be unique you should follow these rules: associate the first string with the
	 * rows (and the second with the columns); when moving backwards in the matrix to determine
	 * the subsequence, in case up and left have the same vaue always move left." */
  start = MPI_Wtime();
	lcs(secondString, firstString, secondStringSize, firstStringSize, id, p);
  end = MPI_Wtime();

  time = end - start;
  MPI_Barrier (MPI_COMM_WORLD);
  printf("Total Time: %f\n", time);

  free(secondString);
  free(firstString);
  MPI_Finalize();

	return EXIT_SUCCESS;
}
