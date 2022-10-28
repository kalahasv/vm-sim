//final version
#include <stdlib.h>
#include<string.h>
#include<stdio.h>

#define MAX_LINE 80
#define MAX_MM_PAGE 4
#define MAX_VM_PAGE 16
#define NUM_ADDRESSES 8

enum ALGORITHM {
    FIFO,
    LRU
};
enum POTENTIALS {
    ZERO,
    ONE,
    TWO,
    THREE,
    USED
};

// define page table: same format as the provided example
struct page_Table{  
    int pageID;     // 0 - 3 (4 pages) for main memory and 0 - 15 (16 pages) for disk/virtual memory
    int validBit;   // 1: page is in main memory, 0: page is on virtual memory
    int dirtyBit;   // 1: page has been written to while in main memory, else 0
    int pageNum;    // page number of virtual page either on main memory (validBit = 1) or disk/virtual memory (validBit = 0)
} pageTable[MAX_VM_PAGE];

// using 2-d array for both the disk and main memory [pageNum][address ID]
// memory layouts would be: disk 16 x 8 and main memory 4 x 8 
// easier than 1-d when working with page ID. Using offset and base address to access actuall address ID
int vm[MAX_VM_PAGE][NUM_ADDRESSES];
int mm[MAX_MM_PAGE][NUM_ADDRESSES];

// Page structure contains information of a Page in main memory
struct Page {
    int pageID;     // pageID refers to virtual page
    int mmPageID;   // main memory's page ID
    int timeSinceAccessed; // times accessed while in queue, reset to zero after page exits queue
};

// a queue data structure of Pages for FIFO algorithm
// since with queue it's easy to handle FIFO items
struct Queue {
    int size;
    struct Page queueItems[MAX_MM_PAGE]; //max queue size is 4
} queue;

// define suitable data structure to handle LRU properties 
// data is Page

void initPageTable() {
    for(int i = 0; i < MAX_VM_PAGE; i ++){
        pageTable[i].pageID = i;
        pageTable[i].validBit = 0;  // All virtual pages are initially on disk so validBit = 0
        pageTable[i].dirtyBit = 0;  // All mm pages have initially not been written so validBit = 0
        pageTable[i].pageNum = i; 
    }
}

void initMemory() {
    for (int i = 0; i < MAX_VM_PAGE; i++) {
        for (int j = 0; j < NUM_ADDRESSES; j++) {
            vm[i][j] = -1;
        }
    }
    for (int i = 0; i < MAX_MM_PAGE; i++) {
        for (int j = 0; j < NUM_ADDRESSES; j++) {
            mm[i][j] = -1;
        }
    }

}

void initQueue() {
    queue.size = 0;
}

void printQueue() {     // creating for testing
    if (queue.size > 0) {
        for (int i = 0; i < queue.size; i++) {
            printf("Items at %d: pageID [%d] timeSinceAccessed: [%d] \n", i, queue.queueItems[i].mmPageID,queue.queueItems[i].timeSinceAccessed);
        }
    }
    else {
        printf("Queue is empty!\n");
    }
}

void distributeInput(char* input, int* argc, char** argv) { //distributes input into argc & argv
    char* token;        
    const char* delims = " \t\n";
    token = strtok(input, delims);      // first token is the command
    while (token != NULL) {             // getting next arguments in to argv
        argv[(*argc)++] = token;
        token = strtok(NULL, delims);
    }
}
void showpTable(){ 
    for(int i = 0; i < MAX_VM_PAGE; i++){
        printf("%d:%d:%d:%d\n",pageTable[i].pageID,pageTable[i].validBit,pageTable[i].dirtyBit,pageTable[i].pageNum);
    }
}

/*void printVM(){ //helper function for testing. prints the vm table.
    for(int i = 0;i < 128; i ++){
        printf("%d\n",vm[i]);
    }
}*/

void printMM(int pageID){   // showmain command
    int address = 0;
    for(int i = 0; i < NUM_ADDRESSES; i ++){
        // print contents in pageID with addr = offset + base_ID * NUM_ADDRESSES 
        address = (pageID * NUM_ADDRESSES) + i;
        printf("%d:%d\n", address, mm[pageID][i]);
    }
}


int isAPageFault(int pageID) { //checks if incoming page is a fault

    for(int i = 0; i < queue.size; i++){
        
        if (pageID == queue.queueItems[i].pageID){     // if the pageID has been loaded to main (exists in queue)
            queue.queueItems[i].timeSinceAccessed = 0;
            
            return 0; //not a hit since value already exists, update accessed value of item 
        }
    }
    return 1; //page fault. value isn't in the queue

}

int LRUFindVictimPage(){  //assume queue is full here -> returns index of item in queue to be removed since it needs to be removed by
    //pageRef is string of pages for reference - can replace with actual reference string later, or not 
    //take all of the least accessed values and find out which is the least recently used one. 
    int max = queue.queueItems[0].timeSinceAccessed; //max times accessed starts at first value
    for(int i = 0; i < queue.size; i++){
        if ( queue.queueItems[i].timeSinceAccessed > max ) 
        {
           max = queue.queueItems[i].timeSinceAccessed;
        }
    }

    //find the least recent page that has been accessed minimum amount of times 
    for(int i = 0; i < queue.size; i++){
        if(queue.queueItems[i].timeSinceAccessed == max){ //finds first element 
            return i; //returns position of element in queue
        }
    }

    return -1; //error 
}


void pushToRear(int mmPageID, int pageNum) { //adds new page to queue with a timeAccessed val of 1, updates pageTable
    int index = queue.size;
    queue.queueItems[index].pageID = pageNum;
    queue.queueItems[index].mmPageID = mmPageID;
    queue.queueItems[index].timeSinceAccessed = 0; // set back to 0 every time. meaning this pageID is newly accessed
    queue.size++;

}

void popByPosition(int position){ // pops element at specified position, then shifts everything to the left
      for (int i = position; i < queue.size -1; i++)  
        {  
            queue.queueItems[i] = queue.queueItems[i+1]; // assign arr[i+1] to arr[i]  
        }  
        queue.size--; // decrease the queue size
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

void copyPageToMemory(int* src, int* target) {
    for (int i = 0; i < NUM_ADDRESSES; i++) {
        target[i] = src[i];
    }
}

int posOfPageInQueue(int pageID) {
    int pos = queue.size;   // since we always take lowest mmPageID is to use. The one available left would be at queue.size
    for (int i = 0; i < queue.size; i++) {
        if (queue.queueItems[i].pageID == pageID) {
            pos = i;
        }
    }
    return pos;
}

void updateTimes(){
    for(int i= 0; i < queue.size; i ++){
        queue.queueItems[i].timeSinceAccessed++;
    }
}

void evictVictimPageByPosition(int pos) {
    copyPageToMemory(mm[queue.queueItems[pos].mmPageID], vm[queue.queueItems[pos].pageID]);  // queue's full. copy the victim page back to disk
    // for FIFO: First item in queue will be evicted. for LRU: Max timeSinceAccessed will be evicted.
    // victim page is copied back to disk with page number is the same as the number virtual page
    // clear validBit and dirtyBit since the page is no longer in main memory
    pageTable[queue.queueItems[pos].pageID].validBit = 0;
    pageTable[queue.queueItems[pos].pageID].dirtyBit = 0;
    pageTable[queue.queueItems[pos].pageID].pageNum = pageTable[queue.queueItems[pos].pageID].pageID;
    popByPosition(pos);
}

void eval(char **argv, int argc, enum ALGORITHM algo){
 
    // prints and quit commands
    if (strcmp(argv[0],"showmain") == 0){
        // print main memory with page ID
        int ID = atoi(argv[1]);
        printMM(ID);
        
    }
    else if (strcmp(argv[0],"showptable") == 0){
        showpTable();
    }
    else if(strcmp(argv[0],"showqueue") == 0){ // helper command
        printQueue();
    }
    /*else if(strcmp(argv[0],"printVM") == 0){ // helper command
        printVM();
    }*/
    else if (strcmp(argv[0],"quit") == 0){
        exit(0);
    }

    else {  // read and write commands which would change the internal of pageTable and memory systems 
        updateTimes(); //increase time for everything in the queue.
        int VA = atoi(argv[1]);     // getting the virtual address
        int pageIndex = VA & 7; // taking off first 3 bits to get the offset of this virtual page since we have addresses from 0 - 7 each page
        int pageNum = (VA - pageIndex) / NUM_ADDRESSES; // find pageID of this VA based on offset
        int victimPos = -1; // default val of -1 for testing sake -> victim page's position in the queue

        if (isAPageFault(pageNum) == 1) {
            printf("A Page Fault Has Occured!\n");
            if (queue.size == MAX_MM_PAGE) { // All pages in main are in queue -> need to choose a victim page for eviction
                if (algo == FIFO) {
                    victimPos = 0;
                }
                else {
                    victimPos = LRUFindVictimPage();
                }
                evictVictimPageByPosition(victimPos);   // evict the found victim page on queue, copy and reset bits
            }

            // else there is still available page in 
            int availableMMPageID = findLowestMMPageID();        
            pushToRear(availableMMPageID, pageNum);
            // copy disk page into main memory (loading page to main memory)
            copyPageToMemory(vm[pageNum], mm[availableMMPageID]);
            // update pageTable is we already loaded page to main memory
            pageTable[pageNum].validBit = 1;    // already in main memory
            pageTable[pageNum].pageNum = availableMMPageID;
        }

        // page might or might not in queue which is on main memory
        // write command still needs to load page to main and write content
        int pos = posOfPageInQueue(pageNum);
        if (strcmp(argv[0], "write") == 0) {
            mm[queue.queueItems[pos].mmPageID][pageIndex] = atoi(argv[2]);      // write content to main memory
            pageTable[pageNum].dirtyBit = 1;
        }
        else {  // read content of a page. either -1 when page fault or the real content
            printf("%d\n", mm[queue.queueItems[pos].mmPageID][pageIndex]);
        }
    }
}
int main(int argc, char* argv[]){

    char input[MAX_LINE];       //user input
    int u_argc = 0;             //user entered: number of arguments 
    char* u_argv[MAX_LINE];     //user entered: list of arguments. First arg is command.
    enum ALGORITHM algo = FIFO; // default page replacement argorithm

    // check for command-line argument if LRU, assign LRU for algorithm else use default
    if (argc == 2) {
        if (strcmp(argv[1], "LRU") == 0) {
            algo = LRU;
        }
    }

    // Initialize the memory and page table 
    initMemory();
    initPageTable();     

    // Initiazlie queue for FIFO algorithm
    initQueue();   

    //take user input
    while(1) //loop until quit is entered
    {
        fflush(stdin);
        fflush(stdout);
        u_argc = 0;

        printf("> ");
        fgets(input,MAX_LINE,stdin);
        //printf("finish\n");

        distributeInput(input, &u_argc, u_argv);
        eval(u_argv, u_argc, algo);
        
    }

    //test
    return 0;
}
