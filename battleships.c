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
	printf("Enter the name of the name of the file.\n");
	printf("\033[94;1m");
	fgets (filename, 31, stdin);
	printf("\033[0m");
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
				printf("File contains invalid characters!!! (It should only contain O's and X's)\n");
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
	bool flag = false;
	for (int i = 1; i < 11; i++){
		for (int j = 1; j < 11; j++){
			if (arr[i][j].hidden_value == ship_symbol){
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
					printf("Ships cannot touch each other!!!\n");
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
	for (int i = 0; i < 5; i++){
		if (!ships_left[i]){
			continue;
		}
		if (ships_left[i] < 0){
			printf("Too many ships of size %d\n", i + 2);
			flag = true;
		}
		else{
			printf("Too few ships of size %d\n", i + 2);
			flag = true;
		}
	}
	if (flag){
		game_over = true;
		return true;
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

	if(direction == 'L' && y < size){
		printf("Not enought space for ship!!!\n");
		return true;
	}

	if(direction == 'R' && 10 - y + 1 < size){
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

	if (check_xy(x, z)){
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

	printf("\033[91;1;3mThe removal was successful!!!\033[0m\n");
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
		printf("\033[%d;1mTurn: %d\n\n", 91 + turn % 6, turn);
	}
	printf("%s\n", options[0]);
	for (int i = 1; i < num_of_options + 1; i++){
		printf("\033[%dm>>>\033[0m%s\n", option[i], options[i]);
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
	printf("Ships you have to put in place:\n\033[95;1mships \033[0;1mx \033[93;1msize\033[0m\n");
	for(int i = 0; i < 5; i++){
		if(i == 3){
			continue;
		}
		printf("\033[95;1m%d\033[0;1m x \033[93;1m%d\033[0m\n", ships_left[i], i + 2);
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

bool invalid_position(struct node_t **arr, int x, int y, char direction, int size){
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
		while (invalid_position(arr, x, y, direction, i + 2));
		paste_ship_in_map(arr, x, (y + 'A' - 1), direction, i + 2);
		ships_left[i]--;
	}
	while(yes_ships_left(ships_left));
}

void wait_for_enter_pressed(){
	char a = ' ';
	printf("\033[8m");
	while (a != '\n'){
		a = getchar();
	}
	printf("\033[0m");
}

void enter_map(struct node_t** arr){
	int answer;
	char *options[4] = {"\033[97;1mHow do you want to create your map?\033[0m", "Organize \033[91;1myour\033[0m map", "Using a \033[94;1mfile\033[0m", "Generate \033[95;1mrandom\033[0m map"};
	int ships_left [] = {4, 3, 2, 0, 1};
	answer = print_menu(options, 3, 1);
	if(answer == 2){
		if (extract_file (arr)){
			return ;
		 }
		if (check_ships(arr, ships_left)){
			return ;
		}

	}
	if (answer == 3){
		auto_generate_map(arr);
	}
	if (answer == 1){
		bool entering_map = true;
		system("clear");
		printf("\033[93;1mTime \033[94;1mto \033[92;1madd \033[95;1myour \033[91;1mfirst \033[96;1mship!\033[0m\n");
		ask_for_ship(arr, ships_left);
		options[0] = "\033[97;1mChoose an option:\033[0m";
		do{
			if(yes_ships_left(ships_left)){
				options[1] = "\033[92;1mAdd\033[0m a ship";
			}
			else{
				options[1] = "\033[93;1mPass\033[0m";
			}
			options[2] = "\033[91;1mChange\033[0m position of a ship";
			options[3] = "\033[96;1mShow\033[0m board";
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
				printf("\n\nPress Enter to hide the map!");
				wait_for_enter_pressed();
				break;
			}
		}
		while(entering_map);
	}
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

bool update_for_sunken_ships(struct node_t **arr, bool computer){
    bool flag, ship_destroyed = false;
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
						if (computer){
							printf("\033[96;1mA ship is underwater!!!\033[0m\n");
						}
						else{
							printf("\033[96;1mThe ship is underwater!!!\033[0m\n");
						}
						ship_destroyed = true;
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
						if (computer){
							printf("\033[96;1mA ship is underwater!!!\033[0m\n");
						}
						else{
							printf("\033[96;1mThe ship is underwater!!!\033[0m\n");
						}
						ship_destroyed = true;
					}
				}
			}
        }
	}
	return ship_destroyed;
}

bool surrounded_by_water (struct node_t **arr, int x, int y){
	if (x != 1 && arr[x - 1][y].value != water_symbol){
		return false;
	}
	if (x != 10 && arr[x + 1][y].value != water_symbol){
		return false;
	}
	if (y != 1 && arr[x][y - 1].value != water_symbol){
		return false;
	}
	if (y != 10 && arr[x][y + 1].value != water_symbol){
		return false;
	}
	return true;
}

bool possible_guess(char *guess, int *last_p){
	if (*last_p && (guess[0] == 'L' || guess[0] == 'U' || guess[0] == 'D' || guess[0] == 'R') && guess[1] == '\n'){
		return true;
    }
	if ((guess[0] >= '1' && guess[0] <= '9') && (guess[1] >= 'A' && guess[1] <= 'J')){
	    return true;
	}
	if (guess[0] == '1' && guess[1] == '0' && (guess[2] >= 'A' && guess[2] <= 'J')){
    	return true;
    }
    return false;
}

bool make_a_guess(struct node_t **arr, int *last_p){
	char input[4] = "";
	int x = 0, y;
	bool flag = false;
	
	if(*last_p){
		printf("\nEnter new point or direction to previous(U, D, L, R): ");	
	}
	else{
		printf("\nEnter point(example \033[32;1m4\033[93;1mA\033[0m): ");
	}
	for (int i = 0; i < 4; i++){
		input[i] = getchar();

		if (input[i] == '\n' && !i){
		    i--;
		    continue;
		}
		if (input[i] == '\n'){
			flag = true;
			break;
		}
		if (input[i] == ' '){
		    i--;
		}
	}
	if (!flag){
		while (input[3] != '\n'){
		    input[3] = getchar();
		}
	}
	if (flag){
		flag = possible_guess(input, last_p);
	}
	if (!flag){
		printf("Invalid input\n");
		return make_a_guess(arr, last_p);
	}
    if(input[1] != '\n'){
	        
		if (input[1]=='0'){
			x = 10;
	        y = input[2] - 'A' + 1;
		}
        
		else{
			x = input[0] - '0';
			y = input[1] - 'A' + 1;
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
	}

	if (arr[x][y].value != not_guessed_symbol){
		printf("Already guessed!!!\n");
		printf("Try again!\n");
		return make_a_guess(arr, last_p);
		
    	}

    	arr[x][y].value = arr[x][y].hidden_value;
    	*last_p = 10*x + y;
    
	if (surrounded_by_water(arr, x, y)){
    	*last_p = 0;
	}
	if (arr[x][y].value == ship_symbol){
		return true;
	}
	return false;
}

bool is_game_over(struct node_t** arr){
	for (int i = 1; i < 11; i++){
		for (int j = 1; j < 11; j++){
			if (arr[i][j].ship_type){
				return false;
			}
		}
	}

	return true;
}

void player_turn(struct node_t **other_map, struct node_t **our_map, int *last_p, bool computer){
    int answer;                                                 
	if (game_over){
		return ;
	}
    if (computer){
		char *options[] = {"\033[0;1mChoose an option:\033[0m", "Check \033[95;1myour\033[0m progress", "Make a \033[92;1mguess\033[0m", "Check the \033[94;1mcomputer\'s\033[0m progress"};
		answer = print_menu(options, 3, 1);
	}
	else{
		 char *options[] = {"\033[0;1mChoose an option:\033[0m", "Show other \033[92;1mplayer\'s\033[0m board", "Make \033[91;1myour\033[0m guess"};
		 answer = print_menu(options, 2, 1);
	}
	switch (answer){
	case 1:
		system("clear");
		if (computer){
			printf("\033[94;1;3mComputer\'s board:\033[0m\n");
		}
		else{
			printf("\033[93;1;3mOther\033[0;3m player\'s board:\033[0m\n");
		}
		print_board(other_map, false);
		printf("\nPress enter to go back to the menu!\n");
		wait_for_enter_pressed();
		player_turn(other_map, our_map, last_p, computer);
		break;

    case 2:
        if(make_a_guess(other_map, last_p)){
        	printf("\033[91;1mYou hit a ship!\033[0m\n");
			if (update_for_sunken_ships(other_map, computer)){
				*last_p = 0;
			}
			game_over = is_game_over(other_map);
			printf("\nPress enter to continue!\n");
			wait_for_enter_pressed();

			player_turn(other_map, our_map, last_p, computer);
		}
		else{
			printf("You missed!\n");
			printf("Press enter to pass!\n");
			wait_for_enter_pressed();
			turn++;
		}
		break;

    case 3:
		system("clear");
		printf("\033[95;1;3mYour board:\033[0m\n");
		print_board(our_map, false);
		printf("\nPress enter to go back to the menu!\n");
		wait_for_enter_pressed();
		player_turn(other_map, our_map, last_p, computer);
		break;
	}
	system("clear");
}

void computer_turn(struct node_t **arr, int *last_p){
	int x, y, i;
	bool flag = true;
	if (game_over){
		return ;
	}
	if (!*last_p){
		do {
			x = rand() % 10 + 1;
			y = rand() % 10 + 1;
		}
		while (arr[x][y].value != not_guessed_symbol || surrounded_by_water(arr, x, y));
	}
	else{
		x = *last_p / 10;
		y = *last_p % 10;
		if (!y){
			x--;
			y = 10;
		}
		if (x != 1 && arr[x - 1][y].value != water_symbol){
			i = 0;
			flag = false;
			while (arr[x - i][y].value == ship_symbol){
				i++;
			}
			if (arr[x - i][y].value == water_symbol || x == i){
				flag = true;
			}
			else{
				x = x - i;
			}
		}
		if (x != 10 && arr[x + 1][y].value != water_symbol && flag){
			i = 0;
			flag = false;
			while (arr[x + i][y].value == ship_symbol){
				i++;
			}
			if (arr[x + i][y].value == water_symbol || x == 11 - i){
				flag = true;
			}
			else{
				x = x + i;
			}
		}
		if (y != 1 && arr[x][y - 1].value != water_symbol && flag){
			i = 0;
			flag = false;
			while (arr[x][y - i].value == ship_symbol){
				i++;
			}
			if (arr[x][y - i].value == water_symbol || y == i){
				flag = true;
			}
			else{
				y = y - i;
			}
		}
		if (y != 10 && flag){
			i = 0;
			while (arr[x][y + i].value == ship_symbol || y == 11 - i){
				i++;
			}
			y = y + i;
		}
	}
	arr[x][y].value = arr[x][y].hidden_value;
	if (arr[x][y].value == ship_symbol){
		printf("\033[91;1mThe computer hit a ship with cords %d%c!\033[0m\n", x, y + 'A' - 1);
		if (!*last_p){
			*last_p = 10*x + y;
		}
		if (update_for_sunken_ships(arr, true)){
			*last_p = 0;
		}
		game_over = is_game_over(arr);
		computer_turn(arr, last_p);
	}
	else{
		turn++;
		printf("Press Enter to start your turn!\n");
		wait_for_enter_pressed();
	}
}

void destroy(struct node_t **arr){
	for (int i = 0; i < 12; i++){
		free (arr[i]);
	}
	free (arr);
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
		wait(2000000);
		str++;
	}
	system("/bin/stty cooked");
	printf("\n\n\033[%d;1mPress \033[%d;1menter \033[%d;1mto \033[%d;1mgo \033[%d;1mto \033[%d;1mthe \033[%d;1mmenu!\n", text_color, player_color, text_color, player_color, text_color, player_color, text_color);
	wait_for_enter_pressed();
}

void singleplayer(){
	struct node_t **player = create_board();
	struct node_t **bot = create_board();
	auto_generate_map(bot);
	turn_change_text("It\'s \n\n time \n\n to \n\n create \n\n your \n\n map!\n\n", 95, 94);
	enter_map(player);
	int last_p = 0, comp_p = 0;
	turn = 1;
	while (!game_over){
		player_turn(bot, player, &last_p, true);
		if (game_over){
			system("clear");
			printf("\033[33t\033[95;1;3mYour board:\n\033[0m");
			print_board(player, false);
			printf("\n\033[95;1;3;4;5m		You won!!!\033[0m\n");
			printf("\n\033[94;1;3mComputer's board:\n\033[0m");
			print_board(bot, false);
			break;
		}
		if (!game_over){
			computer_turn(player, &comp_p);
			if (game_over){
				system("clear");
				printf("\033[33t\033[94;1;3mComputer's board:\n\033[0m");
				print_board(bot, false);
				printf("\n\033[94;1;3;4;5m		You lost!\033[0m\n");
				printf("\n\033[95;1;3mYour board:\n\033[0m");
				print_board(player, false);
			}
		}
	}
	destroy(player);
	destroy(bot);
}

void two_player_game(){
	struct node_t** arr_A = create_board();
	struct node_t** arr_B = create_board();
	int last_p_A = 0, last_p_B = 0;
	
	system("clear");
	turn_change_text("Player \n\n A \n\n time \n\n to \n\n create \n\n your \n\n map!\n\n", 92, 93);
	enter_map(arr_A);
    if(!game_over){
        turn_change_text("Player \n\n B \n\n time \n\n to \n\n create \n\n your \n\n map!\n\n", 93, 92);
        enter_map(arr_B);
    }
	turn = 1;

	while(!game_over){
        turn_change_text("Player \n\n A \n\n It\'s \n\n your \n\n turn!\n\n", 92, 93);
        player_turn(arr_B, arr_A, &last_p_A, false);

        if (game_over){
        	system("clear");
        	printf("\033[33t\033[92;1;3mPlayer A\'s board:\n\033[0m");
        	print_board(arr_A, false);
        	printf("\033[92;1;3;4;5m\n\n		Player A WINS!\n\n\n\033[0m");
        	printf("\n\033[93;1;3mPlayer B\'s board:\n\033[0m");
        	print_board(arr_B, false);
        }
        if (!game_over){
        	turn_change_text("Player \n\n B \n\n It\'s \n\n your \n\n turn!\n\n", 93, 92);
        	player_turn(arr_A, arr_B, &last_p_B, false);
        	if (game_over){
        		system("clear");
        		printf("\033[33t\033[93;1;3mPlayer B\'s board:\n\033[0m");
        		print_board(arr_B, false);
        		printf("\033[93;1;3;4;5m\n\n		Player B WINS!\n\n\n\033[0m");
        		printf("\n\033[92;1;3mPlayer A\'s board:\n\033[0m");
        		print_board(arr_A, false);
        	}
        }
    }
	destroy (arr_A);
	destroy (arr_B);
}

void colorful_print(char *str){
	while (*str != '\0'){
		printf("\033[%d;1;3m%c", 91 + rand() % 6, *str);
		str++;
	}
	printf("\033[0m");
}

int main(){
	srand(time(0));
	int answer;
	char *options[] = {"Choose game mode:", "\033[95;1mSingle\033[94;1mplayer\033[0m", "\033[93;1mTwo \033[92;1mplayer \033[93;1mgame\033[0m"};
	system("clear");
	printf("\n\n		");
	colorful_print("Battleships");
	printf("\n\n\n\n\nPress Enter to start the game!");
	wait_for_enter_pressed();
   	system("clear");
   	printf("\033[27t		\033[37;3mDESCRIPTION\n\nBattleships is a game for 1 to 2 players. The point is to guess the locations of your opponent\'s ships.\n\nLegend:\n\033[31;1m%c\033[0m\033[37;3m - ship symbol\n\033[36;1m%c\033[0m\033[37;3m - water symbol\n%c - unguessed point\n\nShips a map should contain:\n\033[95;1mships\033[0;1m x \033[93;1msize\033[0m\n\033[95;1m    4\033[0;1m x \033[93;1m2\033[0m\n\033[95;1m    3\033[0;1m x \033[93;1m3\033[0m\n\033[95;1m    2\033[0;1m x \033[93;1m4\033[0m\n\033[95;1m    1\033[0;1m x \033[93;1m6\033[0m\n\n\033[37;3mShips can be placed horizontally or vertically (not diagonally) and they cannot touch each other in any way.\n\nIf you\'re about to use a file as a map, be aware that it\'s first 10 lines should only contain 10 symbols (X or O) each andan enter at the end. The rest doesn't matter!\n\n", ship_symbol, water_symbol, not_guessed_symbol);
	printf("\033[0m\n\nPress Enter to start the game!");
	wait_for_enter_pressed();
	answer = print_menu(options, 2, 1);
	if (answer == 1){
		singleplayer();
	}
	if (answer == 2){
		two_player_game();
	}
	return 0;
}

