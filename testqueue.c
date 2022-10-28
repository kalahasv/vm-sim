#include <stdlib.h>
#include<string.h>
#include<stdio.h>

#define MAX_MM_PAGE 4
#define MAX_VM_PAGE 16

enum POTENTIALS {
    ZERO,
    ONE,
    TWO,
    THREE,
    USED
};

struct Page {
    int mmID;       // ID in main memory
    int mmPageID;   // page ID associated to main memory
    int position;   // position in the data structures
    int distance; // distance from front of queue. If exit the queue, reset to 0. If enter the queue, change distance to 0. 
};

struct Queue {
    int size;
    struct Page queueItems[5];
} queue;

struct page_Table{  
    int pageID;     // 0 - 3 (4 pages) for main memory and 0 - 15 (16 pages) for disk/virtual memory
    int validBit;   // 1: page is in main memory, 0: page is on virtual memory
    int dirtyBit;   // 1: page has been written to while in main memory, else 0
    int pageNum;    // page number of virtual page either on main memory (validBit = 1) or disk/virtual memory (validBit = 0)
} pageTable[MAX_VM_PAGE];


void popByPosition(int position){ //pops element at specified position, then shifts everything to the right left
      for (int i = position; i < queue.size -1; i++)  
        {  
            queue.queueItems[i] = queue.queueItems[i+1]; // assign arr[i+1] to arr[i]  
        }  
}

void updateDistances(int newEntry){
    for(int i = 0; i < queue.size; i++){
        if(queue.queueItems[i].mmPageID)
        queue.queueItems[i].distance++; 
    }
}

int LRUFindVictimPage(){  //assume queue is full here -> returns index of item in queue to be removed since it needs to be removed by
    //pageRef is string of pages for reference - can replace with actual reference string later, or not 
    //take all of the least accessed values and find out which is the least recently used one. 
    int maximum = queue.queueItems[0].distance; //minimum times accessed starts at first value
    for(int i = 0; i < queue.size; i++){
        if ( queue.queueItems[i].distance > maximum ) 
        {
           maximum = queue.queueItems[i].distance;
        }
    }

    //find the least recent page that has been accessed minimum amount of times 
    for(int i = 0; i < queue.size; i++){
        if(queue.queueItems[i].distance == maximum){
            return i;
        }
    }

    return -1;


}

void addtoQueue(int mmID,int mmPageID,int pageNum) { //adds new page to queue with a timeAccessed val of 1, updates pageTable

    struct Page newPage = { mmID, mmPageID,pageNum,MAX_MM_PAGE-queue.size-1 }; //constructs a new page to be added to the queue
    queue.queueItems[queue.size] = newPage;
    queue.size++; //increase ssize to accomodate new element
   
}

int findLowestMMPageID(){

    enum POTENTIALS valset[4] = {ZERO,ONE,TWO,THREE};
    int currentPage;
    for(int i = 0; i < queue.size;i++){
        //can either be 0,1,2,3 
        //look through the page table and check if the validBit is 1 meaning the page is in main memory
            currentPage = queue.queueItems[i].mmPageID; //figure out what page it's on
            valset[currentPage] = USED; //change status to used 
    }
    //check for first non-used status and return that index

    for(int i = 0; i < 4; i ++){
        if(valset[i] != USED){
            return i ;
        }
    }
    return -1; //error all pages are used already

}
int main(){
    //init queue 
    queue.size = 0;

    addtoQueue(0,0,0); //adds 2 as "used pages"
    addtoQueue(2,2,0); //adds 2 as "used pages"
    addtoQueue(0,4,0); //adds 2 as "used pages"

    printf("Before\n");
    for (int i = 0; i< 4 ; i++)  
    {  
            printf (" queue[%d] = ", i);  
            printf (" %d \n", queue.queueItems[i].distance);  
    } 
    printf("Value found as unused: %d \n",findLowestMMPageID());
    return 0;
}