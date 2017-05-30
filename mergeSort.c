#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>


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



// Recursive Merge Function
void mergeSort(int *a, int *b, int l, int r) {
  int m;
  if(l < r) {
    m = (l + r)/2;
    mergeSort(a, b, l, m);
    mergeSort(a, b, (m + 1), r);
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

int main(int argc, char** argv) {
	
  
  // Create and populate the array 
  int n = atoi(argv[1]);
  int print = atoi(argv[2]);
  int *originalArray = malloc(n * sizeof(int));
  float startTime;
  int c;
  srand(time(NULL));
    
  for(c = 0; c < n; c++) {
    originalArray[c] = rand() % n;
  }

  // Initialize MPI
  int worldRank;
  int worldSize;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  // Divide the array in equal-sized chunks 
  int size = n/worldSize;

  // Send each subarray to each process 
  int *subArray = malloc(size * sizeof(int));
  MPI_Scatter(originalArray, size, MPI_INT, subArray, size, MPI_INT, 0, MPI_COMM_WORLD);
  if(worldRank == 0){
    startTime = (float)clock()/CLOCKS_PER_SEC;
  }
  
  // Perform the mergesort on each process 
  int *tmpArray = malloc(size * sizeof(int));
  mergeSort(subArray, tmpArray, 0, (size - 1));
  printf("Finished %d\n",worldRank);
  
  // Gather the sorted subarrays into one 
  int *sorted = NULL;
  if(worldRank == 0) {
    sorted = malloc(n * sizeof(int));
  }
  MPI_Gather(subArray, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);

  // Make the final mergeSort call 
  if(worldRank == 0) {
    int *otherArray = malloc(n * sizeof(int));
    mergeSort(sorted, otherArray, 0, (n - 1));

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
      fprintf(f,"Size: %d\n",n);
      fprintf(f,"Time: %f \n\n",timeElapsed);
      if(print){
        for(c = 0; c < n; c++) {
          fprintf(f,"%d\n",sorted[c]);
        }
      }
      fclose(f);
    }
    // Clean up root 
    free(sorted);
    free(otherArray);
    
  }
	
  // Clean up rest 
  free(originalArray);
  free(subArray);
  free(tmpArray);

  // Finalize MPI 
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  
}


