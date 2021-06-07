#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node_t {
	int valid;
	char value;
};

struct node_t* create_board(){
	struct node_t* arr[10][10];
	
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 0; j++){
			struct node_t* new = malloc(sizeof(struct node_t));
			
			new->valid = 0;
			new->value = '*';
			
			arr[i][j] = new;
		}
	}
	
	return arr;
}
