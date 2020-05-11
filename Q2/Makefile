CC = mpicc
EXE0 = partA.exe
EXE1 = partB.exe

%.exe : %.o
	$(CC) -o $@ $^

%.o : %.c
	$(CC) -c -o $@ $^

all: $(EXE0) $(EXE1)

clean:
	rm -f *.exe *.o
