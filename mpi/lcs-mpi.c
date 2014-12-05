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
  unsigned short int* ghost_to_send;
  unsigned short int* to_print;
  unsigned short int* line_chunks_sizes;
  int i, j, process_height, j_line, chunk_size_index;
  double start; // time before lcs algorithm
  double end; // time after lcs algorithm
  double start_intern; // time before lcs algorithm
  double end_intern; // time after lcs algorithm
  double time = 0.0; // total execution time
  double comm_time = 0.0;
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
  int last_m = m, mem_index, mem_index_src, mem_index_dest;

  process_height = 0;

  if ((n % p) != 0) {
    process_height = (n / p) + 1;
    if ((id + 1) <= (n % p)) {
      max_iter = process_height - 1;
    } else {
      max_iter = process_height - 2;
    }
  } else {
    process_height = (n / p);
    max_iter = process_height - 1;
  }

  L = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * process_height );
  ghost = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1));
  to_print = (unsigned short int*) malloc(sizeof(unsigned short int) * (m + 1) * 2);
  line_chunks_sizes = (unsigned short int*) malloc(sizeof(unsigned short int) * p);

  start = MPI_Wtime();
  for (j = 0; j < ((m + 1) * process_height); j++) {
    L[j] = 0;
  }
  for (j = 0; j < p; j++) {
    if ((m % p) != 0) {
      if ((j + 1) <= (m % p)) {
        line_chunks_sizes[j] = (m / p) + 1;
      } else {
        line_chunks_sizes[j] = (m / p);
      }
    } else {
      line_chunks_sizes[j] = (m / p);
    }
   // printf("procc: %d || %d line chunk size: %d\n", id, j, line_chunks_sizes[j]); // debugged
  }
  aux_ghost = (unsigned short int*) malloc(sizeof(unsigned short int) * line_chunks_sizes[0]);
  ghost_to_send = (unsigned short int*) malloc(sizeof(unsigned short int) * line_chunks_sizes[0]);
  for (j = 0; j < (m + 1); j++) {
    ghost[j] = 0;
  }

  for (j = 0; j < line_chunks_sizes[0]; j++) {
    aux_ghost[j] = 0;
  }

  end = MPI_Wtime();
  time = end - start;
  printf("procc: %d || Init Total Time: %f\n", id, time);

  aux_iter = 0;
  start = MPI_Wtime();
  time = 0;
  comm_time = 0;
  
  for (i = 0; i <= max_iter; i++) {

    if (id != 0) {
      // waits for ghost line from previous procc
      //printf("procc: %d || waiting...\n", id);fflush;
      MPI_Recv(aux_ghost, line_chunks_sizes[0], MPI_UNSIGNED_SHORT, (id - 1), (id - 1), MPI_COMM_WORLD, &status);
      //memmove(&ghost[1], &aux_ghost[0], line_chunks_sizes[0] * sizeof(unsigned short int));
      mem_index_src = 0;
      mem_index_dest = 1;
      for (mem_index = 0; mem_index < line_chunks_sizes[0]; mem_index++) {
        ghost[mem_index_dest] = aux_ghost[mem_index_src];
        mem_index_src++;
        mem_index_dest++;
      }
      //printf("procc: %d || first received!!!!\n", id);fflush;
    } else {
      if (i > 0) {
        // waits for ghost line from p procc
        //printf("procc: %d || waiting...\n", id);fflush;
        MPI_Recv(aux_ghost, line_chunks_sizes[0], MPI_UNSIGNED_SHORT, (p - 1), (p - 1), MPI_COMM_WORLD, &status);
        //memmove(&ghost[1], &aux_ghost[0], line_chunks_sizes[0] * sizeof(unsigned short int));
        mem_index_src = 0;
        mem_index_dest = 1;
        for (mem_index = 0; mem_index < line_chunks_sizes[0]; mem_index++) {
          ghost[mem_index_dest] = aux_ghost[mem_index_src];
          mem_index_src++;
          mem_index_dest++;
        }
        //printf("procc: %d || first received!!!!\n", id);fflush;
      } else {
        //printf("procc: %d || first no need to wait\n", id);fflush;
        // no need to wait (else to remove)
      }
    }

    j_line = 1;
    chunk_size_index = 0;
    time = 0;
    while (j_line <= m) {

      for (j = 0; j < line_chunks_sizes[chunk_size_index]; j++) {
        //printf("procc: %d || line: %d || column: %d || index on chunk: %d\n", id, i, j_line, j);
        if (X[j_line - 1] == Y[id + (p * i)]) {
         L[(i * (m + 1)) + j_line] = ghost[j_line - 1] + cost(i);
        } else {
          L[(i * (m + 1)) + j_line] = max(L[(i * (m + 1)) + (j_line-1)], ghost[j_line]);
        }
        j_line++;
      }

      if (id == ((n - 1) % p) && i == max_iter) {
      } else {
        // to send
        //printf("procc: %d || j_line: %d || sent!!!!\n", id, j_line);fflush;
        //memmove(&ghost_to_send[0], &L[(i * (m + 1)) + (j_line - line_chunks_sizes[chunk_size_index])], line_chunks_sizes[0] * sizeof(unsigned short int));
        mem_index_src = (i * (m + 1)) + (j_line - line_chunks_sizes[chunk_size_index]);
        mem_index_dest = 0;
        for (mem_index = 0; mem_index < line_chunks_sizes[0]; mem_index++) {
          ghost_to_send[mem_index_dest] = L[mem_index_src];
          mem_index_src++;
          mem_index_dest++;
        }
        MPI_Isend(ghost_to_send, line_chunks_sizes[0], MPI_UNSIGNED_SHORT, ((id + 1) % p), id, MPI_COMM_WORLD, &request);
      }

      if (j_line <= m) {
        if (id != 0) {
          //printf("procc: %d || j_line: %d || second waiting...\n", id, j_line);fflush;
          MPI_Recv(aux_ghost, line_chunks_sizes[0], MPI_UNSIGNED_SHORT, (id - 1), (id - 1), MPI_COMM_WORLD, &status);
          //memmove(&ghost[j_line], &aux_ghost[0], line_chunks_sizes[chunk_size_index + 1] * sizeof(unsigned short int));
          mem_index_src = 0;
          mem_index_dest = j_line;
          for (mem_index = 0; mem_index < line_chunks_sizes[chunk_size_index + 1]; mem_index++) {
            ghost[mem_index_dest] = aux_ghost[mem_index_src];
            mem_index_src++;
            mem_index_dest++;
          }
          //printf("procc: %d || received!!!!\n", id);fflush;
        } else {
          if (i > 0) {
            // waits for ghost line from p procc
            //printf("procc: %d || j_line: %d || second waiting...\n", id, j_line);fflush;
            MPI_Recv(aux_ghost, line_chunks_sizes[0], MPI_UNSIGNED_SHORT, (p - 1), (p - 1), MPI_COMM_WORLD, &status);
            //memmove(&ghost[j_line], &aux_ghost[0], line_chunks_sizes[chunk_size_index + 1] * sizeof(unsigned short int));
            mem_index_src = 0;
            mem_index_dest = j_line;
            for (mem_index = 0; mem_index < line_chunks_sizes[chunk_size_index + 1]; mem_index++) {
              ghost[mem_index_dest] = aux_ghost[mem_index_src];
              mem_index_src++;
              mem_index_dest++;
            }
            //printf("procc: %d || received!!!!\n", id);fflush;
          } else {
            // no need to wait (else to remove)
            //printf("procc: %d || second no need to wait\n", id);fflush;
          }
        }
      }      
      
      chunk_size_index++;
    }
    //printf("procc: %d || line: %d || column: %d || time: %d\n", id, i, j_line);
  }

  MPI_Barrier (MPI_COMM_WORLD);
  printf("procc: %d || Intern Computation Time: %f\n", id, time);
  printf("procc: %d || Intern Comm Time: %f\n", id, comm_time);
  end = MPI_Wtime();
  time = end - start;
  printf("procc: %d || Computation Total Time: %f\n", id, time);
  MPI_Barrier (MPI_COMM_WORLD);

  /*if (id == 0) {
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
    printf("%s\n", lcs);*/

  for (j = 0; j < (m+1) * process_height; j++) {
    if( j == (m+1) * process_height - 1)
    printf("id: %d index: %d val: %d\n", id, j, L[j]);fflush;
  }
  /*if(id == (n - 1) % p) {
    printf("RESULT: %d\n", L[(m+1) * (max_iter + 1)]);fflush;
  }*/

  free(L);
  free(ghost);
  free(aux_ghost);
  free(to_print);
  free(ghost_to_send);
  free(line_chunks_sizes);
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
