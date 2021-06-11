#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool game_over = false;

struct node_t {
	char hidden_value;
	int valid;
	char value;
	int ship_type;
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
			arr[i][j].ship_type = 0;
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

void reverse_validate_surrounding_cells (struct node_t **arr, int x, int y){
	arr[x - 1][y - 1].valid--;
	arr[x - 1][y].valid--;
	arr[x - 1][y + 1].valid--;
	arr[x][y - 1].valid--;
	arr[x][y + 1].valid--;
	arr[x + 1][y - 1].valid--;
	arr[x + 1][y].valid--;
	arr[x + 1][y + 1].valid--;
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
				printf("File contains invalid characters!!! (It should only contain O's and X's)%c%d%d\n", arr[i][j].hidden_value, i, j);
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

void find_ship_type_horizontally(struct node_t **arr, int x, int y){
	int i = 0;
	while (arr[x][y + i].hidden_value == 'X'){
		i++;
	}
	for (int j = 0; j < i; j++){
		arr[x][y + j].ship_type = i;
	}
}

void find_ship_type_vertically(struct node_t **arr, int x, int y){
	int i = 0;
	while (arr[x + i][y].hidden_value == 'X'){
		i++;
	}
	for (int j = 0; j < i; j++){
		arr[x + j][y].ship_type = i;
	}
}

bool check_ships (struct node_t **arr, int *ships_left){
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
				if (!arr[i][j].ship_type && arr[i][j].valid == 1){
					if (arr[i][j + 1].hidden_value == 'X'){
						find_ship_type_horizontally(arr, i, j);
					}
					if (arr[i + 1][j].hidden_value == 'X'){
						find_ship_type_vertically(arr, i, j);
					}
					ships_left[arr[i][j].ship_type - 2]--;
				}
			}
		}
	}
	if (num_of_x != 31){
		printf("Not enough or too many ships!\n");
		game_over = true;
		return true;
	}
	for (int i = 0; i < 5; i++){
		if (!ships_left[i]){
			continue;
		}
		printf("Too many or too few ships of size %d\n", i + 2);
	}
	return false;
}

bool verify_data(int x, char y, char direction, int size, int* ships_left){
	if(x < 1 || x > 10){
		printf("Invalid x!!!\n");
		return true;
	}
	
	if(y < 'A' || y > 'J'){
		printf("Invalid y!!!\n");
		return true;
	}
	
	if(direction != 'U' && direction != 'D' && direction != 'L' && direction != 'R'){
		printf("Invalid direction!!!\n");
		return true;
	}
	
	if(size < 2 || size > 6 || size == 5){
		printf("Invalid ship size!!! \n");
		return true;
	}
	
	if(!ships_left[size - 2]){
		printf("No more ships left of that size!!!\n");
	}
	
	if(direction == 'L' && (y - 'A' + 1) < size){
		printf("Not enought space for ship!!!\n");
		return true;
	}
	
	if(direction == 'R' && 10 - (y - 'A') < size){
		printf("Not enought space for ship!!!\n");
		return true;
	}
	
	if(direction == 'U' && x < size){
		printf("Not enought space for ship!!!\n");
		return true;
	}
	
	if(direction == 'D' && (10 - x + 1) < size){
		printf("Not enought space for ship!!!\n");
		return true;
	}
	
	return false;
}

bool check_if_real(struct node_t** arr, int x, char z, char direction, int size, int* ships_left){
	int y = z - 'A' + 1;
	
	bool flag = true;
	
	for(int i = 0; i < 5; i++){
		if(ships_left[i] == (i + 2) || i == 3){
			continue;
		}
		
		flag = false;
	}
	
	if(flag){
		printf("Empty map!!!\n");
		return true;
	}
	
	if(x < 1 || x > 10){
		printf("Invalid x!!!\n");
		return true;
	}
	
	if(y < 1 || y > 10){
		printf("Invalid y!!!\n");
		return true;
	}
	
	if(arr[x][y].hidden_value != 'X'){
		printf("This is not a ship!!!\n");
		return true;
	}
	
	if(direction != 'U' && direction != 'D' && direction != 'L' && direction != 'R'){
		printf("Invalid direction!!!\n");
		return true;
	}
	
	if(arr[x][y].ship_type != size){
		printf("Check your size again!!!\n");
		return true;
	}
	
	if(direction == 'L'){
		if(y < size){
			printf("Impossible location for a ship!!!\n");
			return true;
		}
		
		for(int i = 0; i < size; i++){
			if(arr[x][y - i].hidden_value != 'X'){
				printf("There\'s no ship!\n");
				return true;
			}
		}
	}
	
	if(direction == 'R'){
		if((10 - y + 1) < size){
			printf("Impossible location for a ship!!!\n");
			return true;
		}
		
		for(int i = 0; i < size; i++){
			if(arr[x][y + i].hidden_value != 'X'){
				printf("There\'s no ship!\n");
				return true;
			}
		}
	}
	
	if(direction == 'U'){
		if(x < size){
			printf("Impossible location for a ship!!!\n");
			return true;
		}
		
		for(int i = 0; i < size; i++){
			if(arr[x - i][y].hidden_value != 'X'){
				printf("There\'s no ship!\n");
				return true;
			}
		}
	}
	
	if(direction == 'D'){
		if((10 - x + 1) < size){
			printf("Impossible location for a ship!!!\n");
			return true;
		}
		
		for(int i = 0; i < size; i++){
			if(arr[x + i][y].hidden_value != 'X'){
				printf("There\'s no ship!\n");
				return true;
			}
		}
	}
	
	printf("The removal was successful!!!\n");
	return false;
}

bool check_if_occupied(struct node_t** arr, int x, char z, char direction, int size){
	int y = z - 'A' + 1;
	
	if(direction == 'U'){
		for(int i = 0; i < size; i++){
			if(arr[x - i][y].valid){
				printf("Occupied already or too close to another ship!!!\n");
				return true;
			}
		}
	}
	
	if(direction == 'D'){
		for(int i = 0; i < size; i++){
			if(arr[x + i][y].valid){
				printf("Occupied already or too close to another ship!!!\n");
				return true;
			}
		}
	}
	
	if(direction == 'R'){
		for(int i = 0; i < size; i++){
			if(arr[x][y + i].valid){
				printf("Occupied already or too close to another ship!!!\n");
				return true;
			}
		}
	}
	
	if(direction == 'L'){
		for(int i = 0; i < size; i++){
			if(arr[x][y - i].valid){
				printf("Occupied already or too close to another ship!!!\n");
				return true;
			}
		}
	}
	
	return false;
}

void paste_ship_in_map(struct node_t** arr, int x, char z, char direction, int size){
	int y = z - 'A' + 1;
	
	if(direction == 'U'){
		for(int i = 0; i < size; i++){
			arr[x - i][y].hidden_value = 'X';
			arr[x - i][y].ship_type = size;
			validate_surrounding_cells(arr, (x - i), y);
		}
	}
	
	if(direction == 'D'){
		for(int i = 0; i < size; i++){
			arr[x + i][y].hidden_value = 'X';
			arr[x + i][y].ship_type = size;
			validate_surrounding_cells(arr, (x + i), y);
		}
	}
	
	if(direction == 'R'){
		for(int i = 0; i < size; i++){
			arr[x][y + i].hidden_value = 'X';
			arr[x][y + i].ship_type = size;
			validate_surrounding_cells(arr, x, (y + i));
		}
	}
	
	if(direction == 'L'){
		for(int i = 0; i < size; i++){
			arr[x][y - i].hidden_value = 'X';
			arr[x][y - i].ship_type = size;
			validate_surrounding_cells(arr, x, (y - i));
		}
	}
	
}

void ask_for_ship(struct node_t** arr, int* ships_left){
	int x, size;
	char y, direction, prevent_enter_entered;
	
	do{
		do{
			printf("Enter starting point(example 4A), direction(U, D, L, R), size of the new ship: ");
			scanf("%d%c %c %d", &x, &y, &direction, &size);
			scanf("%c", &prevent_enter_entered);
			
		}
		while(verify_data(x, y, direction, size, ships_left));
	}
	while(check_if_occupied(arr, x, y, direction, size));
	
	paste_ship_in_map(arr, x, y, direction, size);
	
	ships_left[size - 2]--;
}

int yes_ships_left(int* ships_left){
	int sum = 0;
	
	for(int i = 0; i < 5; i++){
		sum += ships_left[i];
	}
	
	return sum;
}

void remove_from_board(struct node_t** arr, int x, char z, char direction, int size){
	int y = z - 'A' + 1;
	
	if(direction == 'U'){
		for(int i = 0; i < size; i++){
			arr[x - i][y].hidden_value = 'O';
			arr[x - i][y].ship_type = 0;
			reverse_validate_surrounding_cells(arr, (x - i), y);
		}
	}
	
	if(direction == 'D'){
		for(int i = 0; i < size; i++){
			arr[x + i][y].hidden_value = 'O';
			arr[x + i][y].ship_type = 0;
			reverse_validate_surrounding_cells(arr, (x + i), y);
		}
	}
	
	if(direction == 'R'){
		for(int i = 0; i < size; i++){
			arr[x][y + i].hidden_value = 'O';
			arr[x][y + i].ship_type = 0;
			reverse_validate_surrounding_cells(arr, x, (y + i));
		}
	}
	
	if(direction == 'L'){
		for(int i = 0; i < size; i++){
			arr[x][y - i].hidden_value = 'O';
			arr[x][y - i].ship_type = 0;
			reverse_validate_surrounding_cells(arr, x, (y - i));
		}
	}
}

void remove_ship(struct node_t** arr, int* ships_left){
	int x, size;
	char y, direction, prevent_enter_entered;
	
	do{
		printf("Enter parameters of the ship for removal: ");
		scanf("%d%c %c %d", &x, &y, &direction, &size);
		scanf("%c", &prevent_enter_entered);
			
	}
	while(check_if_real(arr, x, y, direction, size, ships_left));
	
	remove_from_board(arr, x, y, direction, size);
	
	ships_left[size - 2]++;
}

void enter_map(struct node_t** arr){
	char answer, prevent_enter_entered;
	int ships_left [] = {4, 3, 2, 0, 1};
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
		if (check_ships(arr, ships_left)){
			return ;
		}

	}
	else{
		bool entering_map = true;
		
		do{
			if(yes_ships_left(ships_left)){
				printf("a. Add a ship\n");
			}
			else{
				printf("a. Pass\n");
			}
			
			printf("b. Change position of a ship\nc. Show board\nEnter option: ");
			
			scanf("%c", &answer);
			scanf("%c", &prevent_enter_entered);
			
			if(prevent_enter_entered != '\n'){
				answer = prevent_enter_entered;
			}
			
			switch(answer){
			case 'a':
				if(yes_ships_left(ships_left)){
					ask_for_ship(arr, ships_left);
				}
				else{
					entering_map = false;
				}
				
				break;
			
			case 'b':
				remove_ship(arr, ships_left);
				
				ask_for_ship(arr, ships_left);
				
				break;
			
			case 'c':
				print_board(arr, true);
				break;
				
			default:
				printf("Incorrect option!!!\n");
				
			}
		}
		while(entering_map);
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
	print_board(arr1, false);
	destroy (arr1);
	return 0;
}
