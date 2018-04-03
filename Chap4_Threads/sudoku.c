#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define num_threads 27

int valid[num_threads] = {0};

typedef struct {
	int row;
	int column;
} parameters;

int sudoku[9][9] = {
	{6, 2, 4, 5, 3, 9, 1, 8, 7},
	{5, 1, 9, 7, 2, 8, 6, 3, 4},
	{8, 3, 7, 6, 1, 4, 2, 9, 5},
	{1, 4, 3, 8, 6, 5, 7, 2, 2},
	{9, 5, 8, 2, 4, 7, 3, 6, 1},
	{7, 6, 2, 3, 9, 1, 4, 5, 8},
	{3, 7, 1, 9, 5, 6, 8, 4, 2},
	{4, 9, 6, 1, 8, 2, 5, 7, 3},
	{2, 8, 5, 4, 7, 3, 9, 1, 6}
};

void *isColumnValid(void* param) {
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
	if (row != 0 || col > 8) {
		fprintf(stderr, "Invalid row or column for col subsection! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}

	int validityArray[9] = {0};
	int i;
	for (i = 0; i < 9; i++) {
		int num = sudoku[i][col];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			pthread_exit(NULL);
		} else {
			validityArray[num - 1] = 1;
		}
	}
	valid[18 + col] = 1;
	pthread_exit(NULL);
}

void *isRowValid(void* param) {
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
	if (col != 0 || row > 8) {
		fprintf(stderr, "Invalid row or column for rowsubsection! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}

	int validityArray[9] = {0};
	int i;
	for (i = 0; i < 9; i++) {
		int num = sudoku[row][i];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			pthread_exit(NULL);
		} else {
			validityArray[num - 1] = 1;
		}
	}
	valid[9 + row] = 1;
	pthread_exit(NULL);
}

void *is3x3Valid(void* param) {
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
	if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0) {
		fprintf(stderr, "Invalid row or column for subsection! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}
	int validityArray[9] = {0};
	int i, j;
	for (i = row; i < row + 3; i++) {
		for (j = col; j < col + 3; j++) {
			int num = sudoku[i][j];
			if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
				pthread_exit(NULL);
			} else {
				validityArray[num - 1] = 1;
			}
		}
	}
	valid[row + col/3] = 1; // Maps the subsection to an index in the first 8 indices of the valid array
	pthread_exit(NULL);
}

int main(int argv, char *args) {
	pthread_t threads[num_threads];
	int k;

	if(argv == 2){

		printf("Enter sudoku line by line as follows:\n");
		for(k = 0; k < 9; k++){
		printf("%d %d %d %d %d %d %d %d %d\n", 	sudoku[k][0], sudoku[k][1], sudoku[k][2], sudoku[k][3],
																						sudoku[k][4], sudoku[k][5], sudoku[k][6], sudoku[k][7],
																						sudoku[k][8]
																					);
		}
		printf("\n");

		for(k = 0; k < 9; k++){
		scanf("%d %d %d %d %d %d %d %d %d\n", 	&sudoku[k][0], &sudoku[k][1], &sudoku[k][2], &sudoku[k][3],
																						&sudoku[k][4], &sudoku[k][5], &sudoku[k][6], &sudoku[k][7],
																						&sudoku[k][8]
																					);
		}
	}

	printf("\n");
	printf("Sudoku to be checked\n");

	for(k = 0; k < 9; k++){
	printf("%d %d %d %d %d %d %d %d %d\n", 	sudoku[k][0], sudoku[k][1], sudoku[k][2], sudoku[k][3],
																					sudoku[k][4], sudoku[k][5], sudoku[k][6], sudoku[k][7],
																					sudoku[k][8]
																				);
	}

	int threadIndex = 0;
	int i,j;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (i%3 == 0 && j%3 == 0) {
				parameters *data = (parameters *) malloc(sizeof(parameters));
				data->row = i;
				data->column = j;
				pthread_create(&threads[threadIndex++], NULL, is3x3Valid, data); // 3x3 subsection threads
			}
			if (i == 0) {
				parameters *columnData = (parameters *) malloc(sizeof(parameters));
				columnData->row = i;
				columnData->column = j;
				pthread_create(&threads[threadIndex++], NULL, isColumnValid, columnData);	// column threads
			}
			if (j == 0) {
				parameters *rowData = (parameters *) malloc(sizeof(parameters));
				rowData->row = i;
				rowData->column = j;
				pthread_create(&threads[threadIndex++], NULL, isRowValid, rowData); // row threads
			}
		}
	}

	for (i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);			// Wait for all threads to finish
	}

	for (i = 0; i < num_threads; i++) {
		if (valid[i] == 0) {
			printf("Sudoku solution is invalid!\n");
			return EXIT_SUCCESS;
		}
	}
	printf("Sudoku solution is valid!\n");
	return EXIT_SUCCESS;
}
