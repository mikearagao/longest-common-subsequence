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
#include <string.h>

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
  unsigned short int* to_print;
  int i, j, process_height;
  double start; // time before lcs algorithm
  double end; // time after lcs algorithm
  double start_intern; // time before lcs algorithm
  double end_intern; // time after lcs algorithm
  double time; // total execution time
  double comm_time;
  int size = 1;
  int inc = 1;
  int start_id;
  int max_iter = 0;
  int aux_iter, iter, pos_to_store;
  int x, y;
  int init_x, init_y, procc_to_print;
  MPI_Status status;
  MPI_Request request;
  int is_first = 1;
  int max_index;
  char *lcs;
  int last_m = m;

  process_height = 0;

  if ((n % p) != 0) {
    process_height = (n / p) + 1;
  } else {
    process_height = (n / p);
  }

  L = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * process_height );
  ghost = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * process_height);
  to_print = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * 2);
  if (id == 0) {
    aux_ghost = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * process_height);
  }

  start = MPI_Wtime();
  for (j = 0; j < ((m + 1) * process_height); j++) {
    ghost[j] = 0;
    if(id == 0)
      aux_ghost[j] = 0;
    L[j] = 0;
  }
  end = MPI_Wtime();
  time = end - start;
  printf("procc: %d || Init Total Time: %f\n", id, time);

  aux_iter = 0;
  start = MPI_Wtime();
  time = 0;
  comm_time = 0;
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
    start_intern = MPI_Wtime();
    for (j = start_id; j < size; j = j + p) {
      x = init_x - j;
      y = init_y + j;
      pos_to_store = ((m + 1) * iter) + i - id - (p * iter);
      if ((pos_to_store + 1) % (m + 1) == 0)
        aux_iter++;
      if (X[x-1] == Y[y-1]) {
       L[pos_to_store] = ghost[pos_to_store-1] + cost(i);
      } else {
        L[pos_to_store] = max(L[pos_to_store-1], ghost[pos_to_store]);
      }
      iter++;
    }
    end_intern = MPI_Wtime();
    time = time + (end_intern - start_intern);
    if ((iter - 1) >= max_iter) {
      max_iter = (iter - 1);
    }
    
    start_intern = MPI_Wtime();
    MPI_Isend(L, (m + 1) * process_height, MPI_UNSIGNED_SHORT, ((id + 1) % p), id, MPI_COMM_WORLD, &request);

    if (id == 0) {
       MPI_Recv(aux_ghost, (m + 1) * process_height, MPI_UNSIGNED_SHORT, ((id - 1) % p), ((id - 1) % p), MPI_COMM_WORLD, &status);
       for (j = 0; j < (m + 1); j++)
        ghost[j] = 0;
       memcpy(&ghost[(m + 1)], &aux_ghost[0], (m + 1) * (process_height - 1) * sizeof(unsigned short int));
    } else {
      MPI_Recv(ghost, (m + 1) * process_height, MPI_UNSIGNED_SHORT, ((id - 1) % p), ((id - 1) % p), MPI_COMM_WORLD, &status);
    }
    

    size += inc;
    end_intern = MPI_Wtime();
    comm_time = comm_time + (end_intern - start_intern);
    //MPI_Barrier (MPI_COMM_WORLD);
  }
  printf("procc: %d || Intern Computation Time: %f\n", id, time);
  printf("procc: %d || Intern Comm Time: %f\n", id, comm_time);
  end = MPI_Wtime();
  time = end - start;
  printf("procc: %d || Computation Total Time: %f\n", id, time);
  MPI_Barrier (MPI_COMM_WORLD);

  if (id == 0) {
    procc_to_print = (n - 1) % p;
    start = MPI_Wtime();
    for (i = n - 1; i >= 0; i--) {
      if (procc_to_print == 0) {
        memcpy(&to_print[0], &ghost[(m + 1) * max_iter], (m + 1) * sizeof(unsigned short int));
        memcpy(&to_print[(m + 1)], &L[(m + 1) * max_iter], (m + 1) * sizeof(unsigned short int));
        max_iter--;
      } else {
        MPI_Recv(to_print, (m + 1) * 2, MPI_UNSIGNED_SHORT, procc_to_print, procc_to_print, MPI_COMM_WORLD, &status);
        // compute lines
      }

      if (is_first > 0) {
        max_index = to_print[((m + 1) * 2) - 1];
        lcs = (char*) malloc(sizeof(char) * (max_index + 1));
        lcs[max_index] = '\0'; // Set the terminating character
        printf("%d\n", max_index);
        max_index--;
        is_first = -1;
      }

      for (j = last_m; j > 0; j--) {
        last_m--;
        if (X[j - 1] == Y[i]) {
          lcs[max_index] = X[j - 1];
          max_index--;
          break;
        } else if (to_print[j] > to_print[(m + 1) + j - 1]) {
          last_m++;
          break;
        }
      }
      procc_to_print = (procc_to_print - 1);
      if (procc_to_print < 0) {
        procc_to_print = p - 1;
      }
    }
    end = MPI_Wtime();
    time = end - start;
    printf("procc: %d || Print Total Time: %f\n", id, time);
  } else {
    for (i = max_iter; i >= 0; i--) {
      memcpy(&to_print[0], &ghost[(m + 1) * i], (m + 1) * sizeof(unsigned short int));
      memcpy(&to_print[(m + 1)], &L[(m + 1) * i], (m + 1) * sizeof(unsigned short int));

      MPI_Ssend(to_print, (m + 1) * 2, MPI_UNSIGNED_SHORT, 0, id, MPI_COMM_WORLD);
    }
  }

  if(id == 0)
    printf("%s\n", lcs);

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
  if(id == 0)
    printf("Total Time: %f\n", time);

  free(secondString);
  free(firstString);
  MPI_Finalize();

	return EXIT_SUCCESS;
}
