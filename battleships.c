#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define not_guessed_symbol '*'
#define ship_symbol 'X'
#define water_symbol 'O'

bool game_over = false;
int turn = 0;

struct node_t {
	char hidden_value;
	int valid;
	char value;
	int ship_type;
};


void print_board(struct node_t** arr, bool hidden){
	printf("\033[93;1m	A ");

	for(int i = 2; i < 11; i++){
		printf("%c ", 'A' + i - 1);
	}

	printf("\n\n");

	for(int i = 1; i < 11; i++){
		printf("\033[32;1m%d	\033[0m", i);

		for(int j = 1; j < 11; j++){
			if (hidden){
				if (arr[i][j].hidden_value == water_symbol){
					printf("\033[36;1mO \033[0m");

				}
				else{
					printf("X ");
				}
			}
			else{
                if(arr[i][j].value == water_symbol){
                    printf("\033[36;1mO \033[0m");
                }
                else if (arr[i][j].value == ship_symbol){
                    printf("\033[31;1mX \033[0m");
                }
                else{
                    printf("* ");
                }
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
			arr[i][j].hidden_value = water_symbol;
			arr[i][j].value = not_guessed_symbol;
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
			if (!(arr[i][j].hidden_value == water_symbol || arr[i][j].hidden_value == ship_symbol)){
				printf("File contains invalid characters!!! (It should only contain O's and X's)%c%d%d\n", arr[i][j].hidden_value, i, j);
				game_over = true;
				fclose(file);
				return true;
			}
			if (arr[i][j].hidden_value == ship_symbol){
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
	while (arr[x][y + i].hidden_value == ship_symbol){
		i++;
	}
	for (int j = 0; j < i; j++){
		arr[x][y + j].ship_type = i;
	}
}

void find_ship_type_vertically(struct node_t **arr, int x, int y){
	int i = 0;
	while (arr[x + i][y].hidden_value == ship_symbol){
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
			if (arr[i][j].hidden_value == ship_symbol){
				num_of_x++;
				if (arr[i][j].valid == 0 || arr[i][j].valid > 2){
					printf("The map is not valid!\n");
					game_over = true;
					return true;
				}
				if (arr[i][j].valid == 2){
					if (arr[i][j - 1].hidden_value == ship_symbol && arr[i][j + 1].hidden_value == ship_symbol){
						continue;
					}
					if (arr[i - 1][j].hidden_value == ship_symbol && arr[i + 1][j].hidden_value == ship_symbol){
						continue;
					}
					game_over = true;
					return true;
				}
				if (!arr[i][j].ship_type && arr[i][j].valid == 1){
					if (arr[i][j + 1].hidden_value == ship_symbol){
						find_ship_type_horizontally(arr, i, j);
					}
					if (arr[i + 1][j].hidden_value == ship_symbol){
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

bool check_xy (int x, char y){
    if(x < 1 || x > 10){
		printf("Invalid x!!!\n");
		return true;
	}

	if(y < 'A' || y > 'J'){
		printf("Invalid y!!!\n");
		return true;
	}
	return false;
}

bool check_if_occupied(struct node_t **arr, int x, char z, char direction, int size, int* ships_left){
	int y = z - 'A' + 1;

	if (check_xy (x, z))
    {
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
		return true;
	}

	if(direction == 'L' && y + 1 < size){
		printf("Not enought space for ship!!!\n");
		return true;
	}

	if(direction == 'R' && 10 - y < size){
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

	if(arr[x][y].hidden_value != ship_symbol){
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
			if(arr[x][y - i].hidden_value != ship_symbol){
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
			if(arr[x][y + i].hidden_value != ship_symbol){
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
			if(arr[x - i][y].hidden_value != ship_symbol){
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
			if(arr[x + i][y].hidden_value != ship_symbol){
				printf("There\'s no ship!\n");
				return true;
			}
		}
	}

	printf("The removal was successful!!!\n");
	return false;
}

void paste_ship_in_map(struct node_t** arr, int x, char z, char direction, int size){
	int y = z - 'A' + 1;

	if(direction == 'U'){
		for(int i = 0; i < size; i++){
			arr[x - i][y].hidden_value = ship_symbol;
			arr[x - i][y].ship_type = size;
			validate_surrounding_cells(arr, (x - i), y);
		}
	}

	if(direction == 'D'){
		for(int i = 0; i < size; i++){
			arr[x + i][y].hidden_value = ship_symbol;
			arr[x + i][y].ship_type = size;
			validate_surrounding_cells(arr, (x + i), y);
		}
	}

	if(direction == 'R'){
		for(int i = 0; i < size; i++){
			arr[x][y + i].hidden_value = ship_symbol;
			arr[x][y + i].ship_type = size;
			validate_surrounding_cells(arr, x, (y + i));
		}
	}

	if(direction == 'L'){
		for(int i = 0; i < size; i++){
			arr[x][y - i].hidden_value = ship_symbol;
			arr[x][y - i].ship_type = size;
			validate_surrounding_cells(arr, x, (y - i));
		}
	}

}

int print_menu(char *options[], int num_of_options, int curr_option){
	int option[num_of_options];
	char a;
	if (curr_option == num_of_options + 1){
		curr_option = 1;
	}
	if (curr_option == 0){
		curr_option = num_of_options;
	}
	system("clear");
	for (int i = 1; i < num_of_options + 1; i++){
		option[i] = 8;
	}
	option[curr_option] = 5;
	if (turn){
		printf("Turn: %d\n", turn);
	}
	printf("%s\n", options[0]);
	for (int i = 1; i < num_of_options + 1; i++){
		printf("\033[%dm––>\033[0m%s\n", option[i], options[i]);
	}
	system("/bin/stty raw");
	do{
		do{
		    printf("\033[8m");
			a = getchar();
		    printf("\033[0m");
		}
		while (a != '[' && a != 13);
		if (a == 13){
			break;
		}
		a = getchar();
	}
	while (a != 'A' && a != 'B');
	system("/bin/stty cooked");
	if (a == 13){
		printf("\n");
		return curr_option;
	}
    if (a == 'A'){
    	curr_option--;
    }
	if (a == 'B'){
    	curr_option++;
    }
	return print_menu(options, num_of_options, curr_option);
}

void ask_for_ship(struct node_t** arr, int* ships_left){
	printf("Ships you have left to put in place(number ships x size):");
	for(int i = 0, k = 2; i < 5; i++, k++){
		if(i != 3){
			printf("%dx%d ", ships_left[i], k);
		}
	}

	int x, size;
	char y, direction;

	do{
		printf("\nEnter starting point(example \033[32;1m4\033[93;1mA\033[0m), direction(U, D, L, R), size of the new ship: ");
		scanf("%d%c %c %d", &x, &y, &direction, &size);
		getc(stdin);
	}
	while(check_if_occupied(arr, x, y, direction, size, ships_left));

	paste_ship_in_map(arr, x, y, direction, size);

	ships_left[size - 2]--;
}

bool yes_ships_left(int* ships_left){
	for(int i = 0; i < 5; i++){
		if (ships_left[i]){
			return true;
		}
	}
	return false;
}

void remove_from_board(struct node_t** arr, int x, char z, char direction, int size){
	int y = z - 'A' + 1;

	if(direction == 'U'){
		for(int i = 0; i < size; i++){
			arr[x - i][y].hidden_value = water_symbol;
			arr[x - i][y].ship_type = 0;
			reverse_validate_surrounding_cells(arr, (x - i), y);
		}
	}

	if(direction == 'D'){
		for(int i = 0; i < size; i++){
			arr[x + i][y].hidden_value = water_symbol;
			arr[x + i][y].ship_type = 0;
			reverse_validate_surrounding_cells(arr, (x + i), y);
		}
	}

	if(direction == 'R'){
		for(int i = 0; i < size; i++){
			arr[x][y + i].hidden_value = water_symbol;
			arr[x][y + i].ship_type = 0;
			reverse_validate_surrounding_cells(arr, x, (y + i));
		}
	}

	if(direction == 'L'){
		for(int i = 0; i < size; i++){
			arr[x][y - i].hidden_value = water_symbol;
			arr[x][y - i].ship_type = 0;
			reverse_validate_surrounding_cells(arr, x, (y - i));
		}
	}
}

void remove_ship(struct node_t** arr, int* ships_left){
	int x, size;
	char y, direction;

	do{
		printf("Enter parameters of the ship for removal: ");
		scanf("%d%c %c %d", &x, &y, &direction, &size);
		getc(stdin);

	}
	while(check_if_real(arr, x, y, direction, size, ships_left));

	remove_from_board(arr, x, y, direction, size);

	ships_left[size - 2]++;
}

void enter_map(struct node_t** arr){
	int answer;
	char *options[4] = {"Do you want to use a file to create your map?", "\033[92;1mYes\033[0m", "\033[91;1mNo\033[0m"};
	int ships_left [] = {4, 3, 2, 0, 1};
	answer = print_menu(options, 2, 1);
	if(answer == 1){
		if (extract_file (arr)){
			return ;
		 }
		if (check_ships(arr, ships_left)){
			return ;
		}

	}
	if (answer == 2){
		bool entering_map = true;
		do{
			if(yes_ships_left(ships_left)){
				options[1] = "Add a ship";
			}
			else{
				options[1] = "Pass";
			}
			options[2] = "Change position of a ship";
			options[3] = "Show board";
			answer = print_menu(options, 3, 1);
			switch(answer){
			case 1:
				if(yes_ships_left(ships_left)){
					ask_for_ship(arr, ships_left);
				}
				else{
					entering_map = false;
				}

				break;

			case 2:
				remove_ship(arr, ships_left);

				ask_for_ship(arr, ships_left);

				break;

			case 3:
				print_board(arr, true);
				break;

			default:
				printf("Incorrect option!!!\n");

			}
		}
		while(entering_map);
	}
	//system("cls");
	system("clear");
}

void update_surrounding_water (struct node_t **arr, int x, int y){
	if (arr[x - 1][y - 1].value != ship_symbol){
		arr[x - 1][y - 1].value = water_symbol;
	}
	if (arr[x - 1][y].value != ship_symbol){
		arr[x - 1][y].value = water_symbol;
	}
	if (arr[x - 1][y + 1].value != ship_symbol){
		arr[x - 1][y + 1].value = water_symbol;
	}
	if (arr[x][y - 1].value != ship_symbol){
		arr[x][y - 1].value = water_symbol;
	}
	if (arr[x][y + 1].value != ship_symbol){
		arr[x][y + 1].value = water_symbol;
	}
	if (arr[x + 1][y - 1].value != ship_symbol){
		arr[x + 1][y - 1].value = water_symbol;
	}
	if (arr[x + 1][y].value != ship_symbol){
		arr[x + 1][y].value = water_symbol;
	}
	if (arr[x + 1][y + 1].value != ship_symbol){
		arr[x + 1][y + 1].value = water_symbol;
	}
}

void update_for_sunken_ships(struct node_t **arr){
	bool flag;
	int size;
	for (int i = 1; i < 11; i++){
		for (int j = 1; j < 11; j++){
			if (arr[i][j].value == ship_symbol && arr[i][j].ship_type && arr[i][j].valid == 1){
				flag = true;
				size = arr[i][j].ship_type;
				if (arr[i + 1][j].value == ship_symbol){
					for (int k = 0; k < size; k++){
						if (arr[i + k][j].value != ship_symbol){
							flag = false;
						}
					}
					if (flag){
						for (int k = 0; k < size; k++){
							update_surrounding_water(arr, i + k, j);
							arr[i + k][j].ship_type = 0;
						}
						printf("\033[96;1mThe ship is underwater!!!\033[0m\n");
					}
				}
				if (arr[i][j + 1].value == ship_symbol){
					for (int k = 0; k < size; k++){
						if (arr[i][j + k].value != ship_symbol){
							flag = false;
						}
					}
					if (flag){
						for (int k = 0; k < size; k++){
							update_surrounding_water(arr, i, j + k);
							arr[i][j + k].ship_type = 0;
						}
						printf("\033[96;1mThe ship is underwater!!!\033[0m\n");
					}
				}
			}
		}
	}
}

bool valid_position(struct node_t **arr, int x, int y, char direction, int size){
	if(direction == 'L' && y < size){
		return true;
	}

	if(direction == 'R' && 10 - y < size){
		return true;
	}

	if(direction == 'U' && x < size){
		return true;
	}

	if(direction == 'D' && (10 - x + 1) < size){
		return true;
	}
	if(direction == 'U'){
		for(int i = 0; i < size; i++){
			if(arr[x - i][y].valid){
				return true;
			}
		}
	}

	if(direction == 'D'){
		for(int i = 0; i < size; i++){
			if(arr[x + i][y].valid){
				return true;
			}
		}
	}

	if(direction == 'R'){
		for(int i = 0; i < size; i++){
			if(arr[x][y + i].valid){
				return true;
			}
		}
	}

	if(direction == 'L'){
		for(int i = 0; i < size; i++){
			if(arr[x][y - i].valid){
				return true;
			}
		}
	}
	return false;
}

void auto_generate_map(struct node_t **arr){
	int ships_left[] = {4, 3, 2, 0, 1};
	char directions[4] = {'U', 'D', 'R', 'L'};
	int x, y, i = 4;
	char direction;
	srand(time(0));
	do{
		if (!ships_left[i]){
			i--;
			continue;
		}
		do{
			x = rand() % 10 + 1;
			y = rand() % 10 + 1;
			direction = directions[rand() % 4];
		}
		while (valid_position(arr, x, y, direction, i + 2));
		paste_ship_in_map(arr, x, (y + 'A' - 1), direction, i + 2);
		ships_left[i]--;
	}
	while(yes_ships_left(ships_left));
}

bool make_a_guess(struct node_t **arr, int *last_p){
	char input[4] = "";
	int x = 0, y;
	for (int i = 0; i < 4; i++){
		input[i] = 0;
	}
    if(*last_p){
        printf("\nEnter new point or direction to previous(U, D, L, R): ");
        for (int i = 0; i < 4; i++){
        	input[i] = getchar();
        	if (input[i] == '\n'){
        		break;
        	}
        }
        if (input[3] == '\n'){
        	x = 10;
        }
    }
    else{
        printf("\nEnter point(example \033[32;1m4\033[93;1mA\033[0m): ");
        scanf("%c%c%c", &input[0], &input[1], &input[2]);
        if (input[2] != '\n'){
        	getc(stdin);
        	x = 10;
        }
    }

    if(input[1] != '\n'){
        if (x){
        	y = input[2] - 'A' + 1;
        }
        else{
		    x = input[0] - '1' + 1;
		    y = input[1] - 'A' + 1;
		}
        if(check_xy(x, (y + 'A' - 1))){
			return make_a_guess(arr, last_p);
        }
        if(arr[x][y].value != not_guessed_symbol){
            printf("Already guessed!!!\n");
            printf("Try again!\n");
			return make_a_guess(arr, last_p);
        }
    }
	else {
		x = *last_p / 10;
		y = *last_p % 10;
		if (!y){
			x--;
			y = 10;
		}
		if (input[0] == 'U'){
			if (x == 1){
				printf("You\'re trying to go to unknown lands!\n");
				return make_a_guess(arr, last_p);
			}
			x--;
		}
		else if (input[0] == 'D'){
			if (x == 10){
				printf("You\'re trying to go to unknown lands!\n");
				return make_a_guess(arr, last_p);
			}
			x++;
		}
		else if (input[0] == 'L'){
			if (y == 1){
				printf("You\'re trying to go to unknown lands!\n");
				return make_a_guess(arr, last_p);
			}
			y--;
		}
		else if (input[0] == 'R'){
			if (y == 10){
				printf("You\'re trying to go to unknown lands!\n");
				return make_a_guess(arr, last_p);
			}
			y++;
		}
		else {
			printf("Invalid direction!!!");
			return make_a_guess(arr, last_p);
		}
		if (arr[x][y].value != not_guessed_symbol){
            printf("Already guessed!!!\n");
            printf("Try again!\n");
            return make_a_guess(arr, last_p);
        }
	}
    arr[x][y].value = arr[x][y].hidden_value;
    *last_p = 10*x + y;
	if (arr[x][y].value == ship_symbol){
		return true;
	}
	return false;
}

void wait_for_enter_pressed(){
	char a = ' ';
	printf("\033[8m");
	while (a != '\n'){
		a = getchar();
	}
	printf("\033[0m");
}

void player_turn(struct node_t **other_map, struct node_t **our_map, int *last_p, int *num_of_x, bool computer){
	int answer;
	if (game_over){
		return ;
	}
	if (computer){
		char *options[] = {"Choose an option:", "Check your progress", "Make a guess", "Check the computer\'s progress"};
		answer = print_menu(options, 3, 1);
	}
	else{
		 char *options[] = {"Choose an option:", "Show other player\'s board", "Make your guess"};
		 answer = print_menu(options, 2, 1);
	}
	switch (answer){
	case 1:
		//system("cls");
		system("clear");
		if (computer){
			printf("Computer\'s board:\n");
		}
		else{
			printf("Other player\'s board:\n");
		}
		print_board(other_map, false);
		printf("\nPress enter to go back to the menu!\n");
		wait_for_enter_pressed();
		player_turn(other_map, our_map, last_p, num_of_x, computer);
		break;
	case 2:
		if (make_a_guess(other_map, last_p)){
			(*num_of_x)++;
			game_over = *num_of_x == 31;
			printf("\033[91;1mYou hit a ship!\033[0m\n");
			update_for_sunken_ships(other_map);
			printf("\nPress enter to continue!\n");
			wait_for_enter_pressed();
			player_turn(other_map, our_map, last_p, num_of_x, computer);
		}
		else{
			printf("You missed!\n");
			printf("Press enter to pass!");
			wait_for_enter_pressed();
			turn++;
		}
		break;
	case 3:
		//system("cls");
		system("clear");
		printf("Your board:\n");
		print_board(our_map, false);
		printf("\nPress enter to go back to the menu!\n");
		wait_for_enter_pressed();
		player_turn(other_map, our_map, last_p, num_of_x, computer);
		break;
	}
}

void computer_turn(struct node_t **arr, int *num_of_x){
	int x, y;
	if (game_over){
		return ;
	}
	do {
		x = rand() % 10 + 1;
		y = rand() % 10 + 1;
	}
	while (arr[x][y].value != not_guessed_symbol);
	arr[x][y].value = arr[x][y].hidden_value;
	if (arr[x][y].value == ship_symbol){
		(*num_of_x)++;
		game_over = *num_of_x == 31;
		computer_turn(arr, num_of_x);
	}
	else{
		turn++;
	}
}

void destroy(struct node_t **arr){
	for (int i = 0; i < 12; i++){
		free (arr[i]);
	}
	free (arr);
}

void singleplayer(){
	struct node_t **player = create_board();
	struct node_t **bot = create_board();
	auto_generate_map(bot);
	enter_map(player);
	int player_x = 0, comp_x = 0, last_p = 0;
	turn = 1;
	while (!game_over){
		player_turn(bot, player, &last_p, &player_x, true);
		if (game_over){
			printf("\033[92;1mYou won!!!\033[0m");
			break;
		}
		if (!game_over){
			computer_turn(player, &comp_x);
			if (game_over){
				printf("\033[93;1mHAA NOOB!\033[0m");
			}
		}
	}
	destroy(player);
	destroy(bot);
}

void wait(int i){
	while (i){
		i--;
	}
}

void turn_change_text(char *str, int text_color, int player_color){
	system("clear");
	system("/bin/stty raw");
	while (*str != '\0'){
		if (*str != 'A' && *str != 'B'){
			printf("\033[%d;1m%c\033[0m", text_color, *str);
		}
		else{
			printf("\033[%d;1m%c\033[0m", player_color, *str);
		}
		wait(4000000);
		str++;
	}
	system("/bin/stty cooked");
	printf("\n\n\n\033[%d;1mPress \033[%d;1menter \033[%d;1mto \033[%d;1mgo \033[%d;1mto \033[%d;1mthe \033[%d;1mmenu!\n", text_color, player_color, text_color, player_color, text_color, player_color, text_color);
	wait_for_enter_pressed();
}

void two_player_game(){
	struct node_t** arr_A = create_board();
	struct node_t** arr_B = create_board();
	int correct_guesses_A = 0, correct_guesses_B = 0, last_p_A = 0, last_p_B = 0;
	//system("cls");
	system("clear");

	turn_change_text("Player\n\nA\n\ntime\n\nto\n\ncreate\n\nyour\n\nmap!\n", 92, 93);
    enter_map(arr_A);
    if(!game_over){
        turn_change_text("Player\nB\nplease\nenter\nyour\nmap!\n", 93, 92);
        enter_map(arr_B);
    }
	turn = 1;

    while(!game_over){
        turn_change_text("It\'s \n\n player \n\n A\'s \n\n turn!", 92, 93);
        player_turn(arr_B, arr_A, &last_p_A, &correct_guesses_A, false);
        if (game_over){
        	system("clear");
        	print_board(arr_A, false);
        	printf("\033[92;1;4m\n\n		Player A WINS!\n\n\n\033[0m");
        	print_board(arr_B, false);
        }
        if (!game_over){
        	turn_change_text("It\'s \n\n player \n\n B\'s \n\n turn!", 93, 92);
        	player_turn(arr_A, arr_B, &last_p_B, &correct_guesses_B, false);
        	if (game_over){
        		system("clear");
        		print_board(arr_B, false);
        		printf("\033[93;1;4m\n\n		Player B WINS!\n\n\n\033[0m");
        		print_board(arr_A, false);
        	}
        }
    }

	destroy (arr_A);
	destroy (arr_B);
}

int main(){
	int answer;
	char *options[] = {"Choose game mode:", "Singleplayer", "Two player game"};
	answer = print_menu(options, 2, 1);
	if (answer == 1){
		singleplayer();
	}
	if (answer == 2){
		two_player_game();
	}
	return 0;
}

