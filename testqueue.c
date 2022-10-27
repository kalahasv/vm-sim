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

int LRUFindVictimPage(){  //assume queue is full here -> returns index of item in queue to be removed since it needs to be removed by
    //pageRef is string of pages for reference - can replace with actual reference string later, or not 
    //take all of the least accessed values and find out which is the least recently used one. 
    int minimum = queue.queueItems[0].timesAccessed; //minimum times accessed starts at first value
    for(int i = 0; i < queue.size; i++){
        if ( queue.queueItems[i].timesAccessed < minimum ) 
        {
           minimum = queue.queueItems[i].timesAccessed;
        }
    }

    //find the least recent page that has been accessed minimum amount of times 
    for(int i = 0; i < queue.size; i++){
        if(queue.queueItems[i].timesAccessed == minimum){
            return i;
        }
    }

    return -1;


}

int main(){
    //init queue 
    queue.size = 5;

    for (int i = 0; i < 5; i++ )  
    {  
        queue.queueItems[i].mmID = i;
        queue.queueItems[i].timesAccessed = 1;
    }  
    printf("Before\n");
    for (int i = 0; i< 5 ; i++)  
    {  
            printf (" arr[%d] = ", i);  
            printf (" %d \n", queue.queueItems[i].mmID);  
    } 
    
    queue.queueItems[4].timesAccessed--;

    int victim = LRUFindVictimPage();
    printf("Victim Page found: %d\n", victim);

     for (int i = 0; i< 5 ; i++)  
    {  
            printf (" arr[%d] = ", i);  
            printf (" %d \n", queue.queueItems[i].timesAccessed);  
    } 

    


    return 0;
}