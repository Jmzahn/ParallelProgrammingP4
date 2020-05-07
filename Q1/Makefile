CC = mpicc
EXE0 = mpi_matrixmul.exe

%.exe : %.o
	$(CC) -o $@ $^

%.o : %.c
	$(CC) -c -o $@ $^

all: $(EXE0) 

clean:
	rm -f *.exe *.o
