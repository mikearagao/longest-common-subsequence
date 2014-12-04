gcc ../serial/lcs-serial.c -o ../serial/serial -lm -g -fopenmp
gcc ../serial/lcs-old.c -o ../serial/old -lm -g -fopenmp
gcc ../omp/lcs-omp.c -o ../omp/omp -lm -g -fopenmp
mpicc ../mpi/lcs-mpi.c -o ../mpi/mpi -lm -g 
