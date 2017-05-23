#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
  FILE *f = fopen("run","w");
  int computerCount = atoi(argv[1]);
  int procCount = atoi(argv[2]);
  
  fprintf(f,"#!/bin/sh \n");
  fprintf(f,"#SBATCH -p short\n");
  fprintf(f,"#SBATCH -N%d\n",computerCount);
  fprintf(f,"#SBATCH -c%d\n",procCount);
  char* cluster = "alpha";
  if(argc == 4){
    cluster = argv[3];
  }
  fprintf(f,"#SBATCH -C %s\n",cluster);
  fprintf(f,"make run np=%d\n",procCount);
  fclose(f);
  
}
