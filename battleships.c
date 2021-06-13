#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

bool game_over = false;

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
				if (arr[i][j].hidden_value == 'O'){
					printf("\033[36;1mO \033[0m");

				}
				else{
					printf("X ");
				}
			}
			else{
                if(arr[i][j].value!= '*'){
                    if(arr[i][j].value=='O'){
                        printf("\033[36;1m0 \033[0m");
                    }
                    else{
                        printf("\033[0;31mX \033[0m");
                    }
                }
                else{
                    printf("%c ", arr[i][j].value);
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
	char answer;

	int ships_left [] = {4, 3, 2, 0, 1};
	do {
		printf("Do you want to enter with a file? (Y/N): ");
		scanf("%c", &answer);
		getc(stdin);

        if(!(answer == 'Y' || answer == 'N')){
            printf("Unexisting option!");
        }
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
			getc(stdin);

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
	//system("cls");
	system("clear");
}

bool check_the_guess (struct node_t** arr, int x, char z){
    int y = z - 'A' + 1;
    char prev= arr[x][y].value;
    char input[2];
    bool flag= false;

    arr[x][y].value = arr[x][y].hidden_value;
    if (arr[x][y].hidden_value == 'O' && prev == '*')
    {
        return false;
    }
    else{

        if (prev != '*')
        {
            printf("Already guessed!!!\n");
            printf("Try again!\n");
        }

        printf("Enter new point or direction to previous(U, D, L, R): ");
        scanf("%c%c", &input[0], &input[1]);
		if (input[1] != '\n'){
			getc(stdin);
		}
        if(input[1]== '\n'){
        	
            do{
                switch(input[0]){
                case 'U':
                    if (x!=1){
                        x--;
                        flag = false;
                    }
                    break;
                case 'D':
                    if (x!=10){
                        x++;
                        flag = false;
                    }break;
                case 'L':
                    if(y!=1){
                        y--;
                        z = y + 'A' - 1;
                        flag = false;
                    }break;
                case 'R':
                    if(y!=10){
                        y++;
                        z = y + 'A' - 1;
                        flag= false;
                    }break;
                default:
                    printf("Invalid direction!!!\n");
                    flag= true;
                    printf("Enter correct direction(U, D, L, R): ");
                    scanf("%c", &input[1]);
                    getc(stdin);
                    break;
                }
            }while(flag);
        }
        else{
        	getc(stdin);
           x = input[0]-'0';
           z = input[1];
        }
    }


    return check_the_guess(arr, x, z);
    return false;
}

void player(struct node_t **arr)
{
    char y;
    int x, answer;

    do{
        printf("1. Show other player\'s board\n2. Make your guess\nEnter option: ");
        scanf("%d", &answer);
        getc(stdin);

        if (answer != 1 && answer != 2)
        {
            printf("Unexisting option!\n");
        }

    }while(answer != 1 && answer != 2);

    switch(answer){
    case 1:
        print_board(arr, false);
        player(arr);
        break;

    case 2:
		do {
			printf("Enter point(example \033[32;1m4\033[93;1mA\033[0m): ");
			scanf("%d%c", &x, &y);
			getc(stdin);
		}while (check_xy(x, y));
		check_the_guess(arr, x, y);
        break;
        }
        //system("cls");
        system("clear");
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

void player_turn(struct node_t **computer_map, struct node_t **player_map int *num_of_x){
	char z;
	int x, y;
	if (game_over){
		return ;
	}
	switch (answer){
	case 'a':
		do{
			printf("Please enter cords (exaple 4A): ");
			scanf ("%d%c", &x, &z);
			getc(stdin);
			y = z - 'A' + 1;
		}
		while (arr[x][y].value != '*');
		
		arr[x][y].value = arr[x][y].hidden_value;
		if (arr[x][y].value == 'X'){
			(*num_of_x)++;
			game_over = *num_of_x == 31;		
			player_turn(arr, num_of_x);
		}
		break;
	case 'b':
		printf("Computer\'s board:\n");
		print_board(computer_map, false);
		player_turn(computer_map, player_map, num_of_x);
		break;
	case 'c':
		printf("Your board:\n");
		print_board(player_map, false);
		player_turn(computer_map, player_map, num_of_x);
		break;
	default:
		printf("Incorrect option!\n");
		player_turn(computer_map, player_map, num_of_x);
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
	while (arr[x][y].value != '*');
	
	arr[x][y].value = arr[x][y].hidden_value;
	if (arr[x][y].value == 'X'){ // guess_surrounding
		(*num_of_x)++;
		game_over = *num_of_x == 31;
		computer_turn(arr, num_of_x);
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
	int player_x = 0, comp_x = 0;
	while (!game_over){
		player_turn(bot, player, &player_x);
		printf("Computer\'s board\n");
		print_board(bot, false);
		if (game_over){
			printf("\033[92;1mYou won!!!\033[0m");
			break;
		}
		if (!game_over){
			computer_turn(player, &comp_x);
			printf("Your board\n");
			print_board(player, false);
		}
		if (game_over){
			printf("\033[93;1mHAA NOOB!\033[0m");
		}
	}
	destroy(player);
	destroy(bot);
}

void two_player_game(){
	struct node_t** arr_A = create_board();
	struct node_t** arr_B = create_board();
	//system("cls");
	system("clear");

	printf("Player A please enter your map!\n");
    enter_map(arr_A);
    if(!game_over){

        printf("Player B please enter your map!\n");
        enter_map(arr_B);
    }

    if(!game_over){
        printf("Let the game begin!!!\n");
    }

    while(!game_over){
        printf("Player A\n");
        player(arr_B);
        printf("Player B\n");
        player(arr_A);
    }

	destroy (arr_A);
	destroy (arr_B);
}

int main(){
	int answer;
	do{
		printf("Singleplayer or two player mode?(1 or 2) ");
		scanf("%d", &answer);
		getc(stdin);
	}
	while (answer != 1 && answer != 2);
	if (answer == 1){
		singleplayer();
	}
	if (answer == 2){
		two_player_game();
	}
	return 0;
}

