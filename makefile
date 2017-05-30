np=1
size=50000000
source=mergeSortNew.c
exe=mpiExe
print=0

clog:
	rm -f res_np*
	rm -f slurm*
clean:
	rm -f $(exe)
	rm -f gen
all: clean
	mpicc -o $(exe) $(source) -lm
	gcc -o gen generate.c
run: all
	mpirun -np $(np) $(exe) $(size) $(print)


	
cleanSingle:
	rm -f exeSingle

single: cleanSingle
	gcc -o exeSingle mergeSortSingle.c

runSingle:single 
	./exeSingle 10
	
