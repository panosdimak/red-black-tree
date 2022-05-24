#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RBT.h"
#define N 10        //number of elements
#define LIMIT 51    //limit for rand()

int main()
{
    srand(time(0)); //seed for rand()
    rblink root = NULL;
    for (int i=0; i<N; i++) {  //insert N random items 
        int x =  rand()%LIMIT;
        InsertKey(&root, x);
    }

    //printing the elements of the tree in ascending order 
    printf("\nPrinting items in ascending order...\n");
    PrintElements(root);
    printf("\n");

    //searching for a random item in the tree
    int x = rand()%LIMIT;                                  
    printf("\nsearching for item \"%d\"... ", x);         
    rblink s = Search(root, x);
    if (s)
        printf("item %d is in the tree\n", Data(s));
    else printf("item %d not found\n", x);

    return 0;
}