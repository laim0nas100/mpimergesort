np=4
size=50000000
source=mergeSort.c
exe=mpiExe
print=0

clog:
	rm -f res_np*
clean:
	rm -f $(exe)
	rm -f gen
all: clean
	mpicc -o $(exe) $(source)
	gcc -o gen generate.c
run: all
	mpirun -np $(np) $(exe) $(size) $(print)

		
	