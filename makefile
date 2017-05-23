np=4
size=10
source=mergeSort.c
exe=mpiExe
print=1

clog:
	rm -f res_np*
clean:
	rm -f $(exe)
	
all: clean
	mpicc -o $(exe) $(source)
	
run: all
	mpirun -np $(np) $(exe) $(size) $(print)
	

		
	
