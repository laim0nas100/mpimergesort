#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int fullSize;
int slice;
// Merge Function 
void merge(int *a, int *b, int l, int m, int r) {
	
  int h, i, j, k;
  h = l;
  i = l;
  j = m + 1;
  while((h <= m) && (j <= r)) {
    if(a[h] <= a[j]) {
      b[i] = a[h++];
    }
    else {
      b[i] = a[j++];
    }
    i++;
  }
  
  //whats left
  k = h;
  if(m < h){
    m = r;
    k = j;
  }
  while(k <= m){
    b[i++] = a[k++];
  }
  
  for(k = l; k <= r; k++) {
    a[k] = b[k];
  }
		
}

void insertionSort (int a[],int start, int end){
  int i;
  if(start <0 || end<0){
    puts("Negative index");
    MPI_Abort (MPI_COMM_WORLD, 1);
  }
  for (i = start; i < end; i++){
      int j, v = a[i];
      for (j = i - 1; j >= start; j--){
        if (a[j] <= v)
          break;
	  a[j + 1] = a[j];
      }
      a[j + 1] = v;
    }
}

// Recursive Merge Function
void mergeSortInsertion(int *a, int *b, int l, int r) {
  int m;
  if(r - l <= 16 && r > l){
    insertionSort(a,l,r+1);
  }else 
  if(l < r){
    m = (l + r)/2;
    
    mergeSortInsertion(a, b, l, m);
    mergeSortInsertion(a, b, (m + 1), r);
    merge(a, b, l, m, r);
  }
  
} 

// Recursive Merge Function
void mergeSort(int *a, int *b, int l, int r) {
  int m;
  if(l < r){
    m = (l + r)/2;
    
    mergeSort(a, b, l, m);
    mergeSort(a, b, (m + 1), r);
    merge(a, b, l, m, r);
  }
  
} 

void mergeSortFinal(int *a, int *b, int l, int r) {
  int m;
  if(r - l > slice && r > l){
    m = (l + r)/2;
    
    mergeSortFinal(a, b, l, m);
    mergeSortFinal(a, b, (m + 1), r);
    merge(a, b, l, m, r);
  }
  
}
int exists(const char *fname){
    FILE *file;
    if (file = fopen(fname, "r")){
        fclose(file);
        return 1;
    }
    return 0;
}




int mpiSort(int worldRank,int subarraySize, int* originalArray, int* sorted){
  
  // Send each subarray to each process 
  int *subArray = malloc(subarraySize * sizeof(int));
  MPI_Scatter(originalArray, subarraySize, MPI_INT, subArray, subarraySize, MPI_INT, 0, MPI_COMM_WORLD);
  
  
  // Perform the mergesort on each process 
  int *tmpArray = malloc(subarraySize * sizeof(int));
  mergeSortInsertion(subArray, tmpArray, 0, (subarraySize - 1));
  printf("Finished %d\n",worldRank);
  
  // Gather the sorted subarrays into one 
  
  MPI_Gather(subArray, subarraySize, MPI_INT, sorted, subarraySize, MPI_INT, 0, MPI_COMM_WORLD);
  free(subArray);
  free(tmpArray);
  return 0;
}



int main(int argc, char** argv) {
	
  
  // Create and populate the array 
  fullSize = atoi(argv[1]);
  int print = atoi(argv[2]);
  int *arrayToSort = malloc(fullSize * sizeof(int));
  float startTime;
  int c;
  srand(time(NULL));
    
  for(c = 0; c < fullSize; c++) {
    arrayToSort[c] = rand() % fullSize;
  }

  // Initialize MPI
  int worldRank;
  int worldSize;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  // Divide the array in equal-sized chunks 
  int size = fullSize/worldSize;
  slice = size;
  int *sorted = NULL;
  if(worldRank == 0) {
    sorted = malloc(fullSize * sizeof(int));
    startTime = (float)clock()/CLOCKS_PER_SEC;
  }
  mpiSort(worldRank,size,arrayToSort,sorted);
  
  // Make the final mergeSort call 
  if(worldRank == 0) {
    int *otherArray = malloc(fullSize * sizeof(int));
    mergeSortFinal(sorted, otherArray, 0, (fullSize - 1));

    if(worldRank == 0){
      float timeElapsed = (float)clock()/CLOCKS_PER_SEC - startTime;
      char* filePath = "result";
      printf("world size %d\n",worldSize);
      int suffix = 0;
      while(1){
        char newFilePath[50];
        sprintf(newFilePath,"res_np%d_%d.txt",worldSize,suffix);
        if(!exists(newFilePath)){
          printf("new path = %s\n",newFilePath);
          filePath = newFilePath;
          break;
        }
        suffix++;
        
      }
      FILE *f = fopen(filePath,"w");
      fprintf(f,"Number of proccesses: %d\n",worldSize);
      fprintf(f,"Size: %d\n",fullSize);
      fprintf(f,"Time: %f \n\n",timeElapsed);
      int ok = 1;
      if(print){
        fprintf(f,"%d\n",sorted[0]);
      }
        for(c = 1; c < fullSize; c++) {
          if(print){
            fprintf(f,"%d\n",sorted[c]);
          }
          if(sorted[c-1]> sorted[c]){
            ok = 0;
          }
        }
      printf("ok? %d\n",ok);
      fclose(f);
    }
    // Clean up root 
    free(sorted);
    free(otherArray);
    
  }
	
  // Clean up rest 
  free(arrayToSort);
  

  // Finalize MPI 
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  
}


