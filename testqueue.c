#include <stdlib.h>
#include<string.h>
#include<stdio.h>

struct Page {
    int mmID;       // ID in main memory
    int mmPageID;   // page ID associated to main memory
    int position;   // position in the data structures
    int timesAccessed; // times accessed while in queue, reset to zero after page exits queue
};

struct Queue {
    int size;
    struct Page queueItems[5];
} queue;

void popByPosition(int position){ //pops element at specified position, then shifts everything to the right left
      for (int i = position; i < queue.size -1; i++)  
        {  
            queue.queueItems[i] = queue.queueItems[i+1]; // assign arr[i+1] to arr[i]  
        }  
}

int main(){
    //init queue 
    queue.size = 5;

    for (int i = 0; i < 5; i++ )  
    {  
        queue.queueItems[i].mmID = i;
    }  
    printf("Before\n");
    for (int i = 0; i< 5 ; i++)  
    {  
            printf (" arr[%d] = ", i);  
            printf (" %d \n", queue.queueItems[i].mmID);  
    } 
    popByPosition(2);
    printf("After\n");
    for (int i = 0; i< 5 - 1; i++)  
    {  
            printf (" arr[%d] = ", i);  
            printf (" %d \n", queue.queueItems[i].mmID);  
    }  


    return 0;
}