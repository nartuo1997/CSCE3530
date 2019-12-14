#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> 

#define INF 99
#define N 6

struct edge{
	char currentNode;
	char nextNode;
	int cost;
};

struct shortest{
	char currentNode;
	char prevNode;
	int cost;
};

void readfile(struct edge Edges[]);
void adjacency(struct edge Edges[], int arr[][N]);
int minDist(int distance[], bool node_visited[]) ;
void dijkstra(int arr[][N], int source);
int min(int a, int b);
void writeFile(int distance[], int prevNode[], int src);
void prevPath(int prevNode[], int i, int*c);

int isVisitied(char *visited, char node);
void printMatrix(int matrix[][N]);
void print(struct shortest path[][N]);



struct edge Edges[10];	
struct shortest path[6][6] = {0};

int matrix[6][6] = {0};
char nodes[N] = { 'u', 'v', 'w', 'x', 'y', 'z' };

int counter = 0;

int main()
{
	int shortest_path[6][6] = {0};
	
	readfile(Edges);		// get data from file

	printMatrix(matrix);


for(int i = 0 ; i < 6; i++)
{
	dijkstra(matrix, i);		// dikstra
}	

	
	printf("\n");

	

	return 0;
}



void adjacency( struct edge Edges[] , int arr[][6])	// building adjacency matrix
{
	int src = 0;
	int dest = 0;

	for(int i =0; i < 10; i++)
	{
		src = Edges[i].currentNode - 'u';		// source index
		dest = Edges[i].nextNode - 'u';			// dest index
		
		arr[src][dest] = Edges[i].cost;		// cost from source to index or vice versa
		arr[dest][src] = Edges[i].cost;
	}
}


void dijkstra(int matrix[][6], int source)
{
	
	bool node_visited[6] ; 	// intialize visited nodes to be empty
	
	int distance[N];	// initialize disance as INF
	int prevNode[N];	// store previous node

	
	for (int i = 0; i < N; i++) {
         distance[i] = INF; 		// assign inf
         node_visited[i] = 0;		// assign false
    }


	distance[source] = 0;		// distance to itself is 0
	prevNode[source] = 100;		// intialize first node starting index


	int min = 0	;

	for(int i = 0; i < N  ; i++)	
	{
		min = minDist(distance, node_visited);	// get smallest distance to the next node
		node_visited[min] = 1;	// keep track of visited node that has the smallest distance


		for(int j = 0; j < N; j++)
		{
			// dijkstra algorithm
			if(node_visited[j] == false && matrix[min][j] && distance[min] != INF && distance[min] + matrix[min][j] < distance[j])
			{	
					// add the smallest distance and store it in distance array
					prevNode[j] = min;
					distance[j] = distance[min] + matrix[min][j];	
				
			}
		}
	
	}

	// printf("\n");printf("%d, %d\n", distance[0], prevNode[0]);
	// for(int i = 0; i < N; i++)
	// {
	// 	// printf("%d ", distance[i]);
	// 	printf("%d ", temp[i]);
	// }
	// printf("\n");
	// printf("source: %d ", source);
	writeFile(distance, prevNode, source);
	


}

int minDist(int distance[], bool node_visited[]) // get the min index of node
{ 
    int min = 999; 
	int min_index = 0; 
  
    for (int i = 0; i < N; i++) 
	{
        if (node_visited[i] == 0 && distance[i] <= min) 
		{
	        min = distance[i]; 
			min_index = i; 
		}
	}
    return min_index; 
} 

void printMatrix(int matrix[][6])
{
	printf ("u v w x y z\n");
	for(int i = 0; i < 6; i++)
	{
		for(int j = 0; j < 6; j++)
		{
			printf("%d ", matrix[i][j] );
		}
		printf("\n");
	}
}



int min(int a, int b)
{
	if (a < b)
		return a;
	else return b;
}

int isVisitied(char *visited, char node){
    
    int found = 0;
    for(int i = 0; i < strlen(visited); i++){
        if (node == visited[i])
            found = 1;
    }
    
    return found;
}


void readfile(struct edge Edges[]){
	FILE * file;
	file = fopen("router.txt", "r");

	int c;
	int index = 0;

	while(c != EOF)
	{	
		fscanf(file,"%c %c %d", &Edges[index].currentNode, &Edges[index].nextNode, &Edges[index].cost );
		c = fgetc(file);
		index++;	
	}
	fclose(file);

	// for(int i =0; i < 10; i++)
	// {
	// 	printf("%c %c %d\n", Edges[i].currentNode, Edges[i].nextNode, Edges[i].cost );
	// }
	adjacency( Edges , matrix);		// building 2d array
}

void writeFile(int distance[], int prevNode[], int src)		// write file to file
{
	FILE * fp;
	fp = fopen("LS.txt", "a");
	int temp ;
	for(int i = 0; i < N; i++)
	{
		prevPath(prevNode, i, &temp);
        // printf("distances[%d]: %d\n",i, distance[i] );
		if (distance[i] != 0)
		{
			fprintf(fp, "%c: (%c, %c)\n", nodes[i], nodes[src], nodes[temp]);
		}
		counter = 0;

	}
	fputs("---------\n", fp);
	fclose(fp);
}

void prevPath(int prevNode[], int i, int *c) 
{ 
	
	
    if (prevNode[i] == 100 ) return ;

   
    prevPath(prevNode, prevNode[i], c); 

   
    counter++; 
	if(counter==1) 
		*c=i; 

	
		
} 









