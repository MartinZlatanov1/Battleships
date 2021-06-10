
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool game_over = false;

struct node_t {
	char hidden_value;
	int valid;
	char value;
};

void print_board(struct node_t** arr, bool hidden){
	printf("	A ");
	
	for(int i = 2; i < 11; i++){
		printf("%c ", 'A' + i - 1);
	}
	
	printf("\n\n");
	
	for(int i = 1; i < 11; i++){
		printf("%d	", i);
		
		for(int j = 1; j < 11; j++){
			if (hidden){
				printf("%c ", arr[i][j].hidden_value);
			}
			else{
				printf("%c ", arr[i][j].value);
			}
		}
		
		printf("\n");
	}
}

struct node_t** create_board(){
	struct node_t** arr = malloc(12*sizeof(struct node_t));
	
	for(int i = 0; i < 12; i++){
		arr[i] = malloc(12*sizeof(struct node_t));
	}
	
	for(int i = 0; i < 12; i++){
		for(int j = 0; j < 12; j++){
			arr[i][j].hidden_value = 'O';
			arr[i][j].value = '*';
			arr[i][j].valid = 0;
			
		}
	}
	
	return arr;
}


void validate_surrounding_cells (struct node_t **arr, int x, int y){
	arr[x - 1][y - 1].valid++;
	arr[x - 1][y].valid++;
	arr[x - 1][y + 1].valid++;
	arr[x][y - 1].valid++;
	arr[x][y + 1].valid++;
	arr[x + 1][y - 1].valid++;
	arr[x + 1][y].valid++;
	arr[x + 1][y + 1].valid++;
}

bool extract_file(struct node_t **arr){
	char filename[31];
	printf("Enter the name of the name of the file.\n\
The first 10 lines should be 10 characters of O's and X's and an enter. The rest doesn't matter.\n");
	fgets (filename, 31, stdin);
	if (filename[strlen(filename) - 1] == '\n'){
		filename[strlen(filename) - 1] = '\0';
	}
	FILE *file;
	file = fopen (filename, "r");
	if (file == NULL){
		printf("File does not exist!!!\n");
		game_over = true;
		return true;
	}
	for (int i = 1; i < 11; i++){
		for (int j = 1; j < 11; j++){
			arr[i][j].hidden_value = getc(file);
			if (!(arr[i][j].hidden_value == 'O' || arr[i][j].hidden_value == 'X')){
				printf("File contains invalid characters!!! (It should only contain O's and X's)");
				game_over = true;
				fclose(file);
				return true;
			}
			if (arr[i][j].hidden_value == 'X'){
				validate_surrounding_cells(arr, i, j);
			}
		}
		getc(file);
	}
	fclose(file);
	return false;
}

bool check_ships (struct node_t **arr){
	int num_of_x = 0;
	for (int i = 1; i < 11; i++){
		for (int j = 1; j < 11; j++){
			if (arr[i][j].hidden_value == 'X'){
				num_of_x++;
				if (arr[i][j].valid == 0 || arr[i][j].valid > 2){
					printf("The map is not valid!\n");
					game_over = true;
					return true;
				}
				if (arr[i][j].valid == 2){
					if (arr[i][j - 1].hidden_value == 'X' && arr[i][j + 1].hidden_value == 'X'){
						continue;
					}
					if (arr[i - 1][j].hidden_value == 'X' && arr[i + 1][j].hidden_value == 'X'){
						continue;
					}
					game_over = true;
					return true;
				}
			}
		}
	}
	if (num_of_x != 31){
		printf("Not enough or too many ships!\n");
		game_over = true;
		return false;
	}
	return false;
}

bool verify_data(int x, char y, char direction, int size){
	if(x < 1 || x > 10){
		printf("Invalid x!!! \n");
		return true;
	}
	
	if(y < 'A' || y > 'J'){
		printf("Invalid y!!! \n");
		return true;
	}
	
	if(direction != 'U' && direction != 'D' && direction != 'L' && direction != 'R'){
		printf("Invalid direction!!! \n");
		return true;
	}
	
	if(size < 2 || size > 6 || size == 5){
		printf("Invalid ship size!!! \n");
		return true;
	}
	
	if(direction == 'L' && (y - 'A' + 1) < size){
		printf("Not enought space for ship!!! \n");
		return true;
	}
	
	if(direction == 'R' && 10 - (y - 'A') < size){
		printf("Not enought space for ship!!! \n");
		return true;
	}
	
	if(direction == 'U' && x < size){
		printf("Not enought space for ship!!! \n");
		return true;
	}
	
	if(direction == 'D' && (10 - x + 1) < size){
		printf("Not enought space for ship!!! \n");
		return true;
	}
	
	return false;
}

bool check_if_occupied(struct node_t** arr, int x, char y, char direction, int size){
	y = y - 'A' + 1;
	
	if(direction == 'U'){
		for(int i = 0; i < size; i++){
			if(!arr[x - i][y].valid){
				return true;
			}
		}
	}
	
	if(direction == 'D'){
		for(int i = 0; i < size; i++){
			if(!arr[x + i][y].valid){
				return true;
			}
		}
	}
	
	if(direction == 'R'){
		for(int i = 0; i < size; i++){
			if(!arr[x][y + i].valid){
				return true;
			}
		}
	}
	
	if(direction == 'L'){
		for(int i = 0; i < size; i++){
			if(!arr[x][y - i].valid){
				return true;
			}
		}
	}
	
	return false;
}

void ask_for_ship(struct node_t** arr){
	int x, size;
	char y, direction;
	
	do{
		do{
			printf("Enter starting point(example 4A), direction(U, D, L, R), size : \n");
			scanf("%d%c %c %d", &x, &y, &direction, &size);
		}
		while(verify_data(x, y, direction, size));
	}
	while(check_if_occupied(arr, x, y, direction, size));
	
}

void enter_map(struct node_t** arr){
	char answer, prevent_enter_entered;
	do {
		printf("Do you want to enter with a file? (Y/N): ");
		scanf("%c", &answer);
		scanf("%c", &prevent_enter_entered);
	}
	while (!(answer == 'Y' || answer == 'N'));
	if(answer == 'Y'){
		if (extract_file (arr)){
			return ;
		 }
		if (check_ships(arr)){
			return ;
		}
		
	}
	else{
		printf("a. Add a ship\nb. Change position of a ship\nc. Show board\nEnter option: ");
		
		scanf("%c", &answer);
		
		switch(answer){
		case 'a':
			ask_for_ship(arr);
		}
	}
}

void destroy(struct node_t **arr){
	for (int i = 0; i < 12; i++){
		free (arr[i]);
	}
	free (arr);
}

int main(){
	struct node_t** arr1 = create_board();
	enter_map(arr1);
	print_board(arr1, true);
	destroy (arr1);
	return 0;
}
