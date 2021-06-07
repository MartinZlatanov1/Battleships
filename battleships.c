#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node_t {
	int valid;
	char value;
};

void print_board(struct node_t** arr){
	printf("	A ");
	
	for(int i = 2; i < 11; i++){
		printf("%c ", 'A' + i - 1);
	}
	
	printf("\n\n");
	
	for(int i = 1; i < 11; i++){
		printf("%d	", i);
		
		for(int j = 1; j < 11; j++){
			printf("%c ", arr[i][j].value);
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
			arr[i][j].value = '*';
			arr[i][j].valid = 0;
			
		}
	}
	
	return arr;
}

/*void enter_map(struct node_t* arr){
	char answer;
	printf("Do you want to enter with a file? (Y/N): ");
	scanf("%c", &answer);
	
	if(!strcmp(answer, 'Y')){
		//TO DO FILE
	}
	else{
		
	}
}*/

int main(){
	struct node_t** arr1 = create_board();
	print_board(arr1);
	
	return 0;
}
