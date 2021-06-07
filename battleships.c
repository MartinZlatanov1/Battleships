#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node_t{
	struct node_t* left;
	struct node_t* right;
	struct node_t* up;
	struct node_t* down;
	
	int valid;
	char ship;
};

struct node_t* create_board(){
	
}

void search_find(struct node_t* head, int x, int y){
	for(int i = 1; i < x; i++){
		head = head->right;	
	}
	
	for(int i = 1; i < y; i++){
			head = head->down;
	}
}

int main() {
	
	
	
	return 0;
}
