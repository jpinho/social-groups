#include <stdio.h>
#include <stdlib.h>

void createTestLinear(int);
void createTestQuadratic(int);
	
int main(int argc, char* argv[]){

	int i=0;
	
	for(i=10; i<=100; i+=10){
		createTestLinear(i);
	}
	
	for(i=10; i<=100; i+=10){
		createTestQuadratic(i);
	}
	
	exit(0);
}

void createTestLinear(int nvertex){
    FILE *fs;
	char filename[100];
	
	sprintf(filename, "test_linear_v%d.in", nvertex);
    fs = fopen(filename, "w");
    printf("creating linear test file.\n");

    if(fs == NULL){
        perror("Unable to create test file!");
        exit(-1);
    }

    fprintf(fs, "%d %d\n", nvertex, nvertex);
	
	int i;

	for(i=1; i <= nvertex; i++){
		fprintf(fs, "%d %d\n", i, i >= nvertex ? ((nvertex - (nvertex-i) - 1) % nvertex) : i+1);
	}

    fclose(fs);
}

void createTestQuadratic(int nvertex){
    FILE *fs;
	char filename[100];
	
	sprintf(filename, "test_quadratic_v%d.in", nvertex);
    fs = fopen(filename, "w");
    printf("creating quadratic test file.\n");

    if(fs == NULL){
        perror("Unable to create test file!");
        exit(-1);
    }

	int i, nshares = nvertex * nvertex;
	int from, to;
    fprintf(fs, "%d %d\n", nvertex, nshares);
	
	for(i=1; i <= nshares; i++){
		from=i%nvertex;
		to=i >= nvertex ? ((nvertex - (nvertex-i) - 1) % nvertex) : i+1;
		fprintf(fs, "%d %d\n", from==0?1:from, to==0?1:to);
	}

    fclose(fs);
}
