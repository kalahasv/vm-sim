//test setting up the github
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

//int vm[128] = { [0 ... 127 ] = -1} ;    //virtual memory, 128 addresses init to -1, also known as "disk"
//int mm[32] = {[0 ... 31] = -1};         //main memory, 32 addresses init to -1

// using 2-d for both the disk and main memory [pageNum][address ID]
// memory layouts would be: disk 16 x 8 and main memory 4 x 8 
// easier than 1-d when working with page ID. Using offset and base address to access actuall address ID
int vm[MAX_VM_PAGE][NUM_ADDRESSES];
int mm[MAX_MM_PAGE][NUM_ADDRESSES];

// Page structure contains information of a Page in main memory
struct Page {
    int pageID;     // pageID refers to virtual page
    int mmPageID;   // main memory's page ID
    //int position;   // position in the data structures
    int time; // time since been in queue. reset to 0 upon exiting.
};

// a queue data structure of Pages for FIFO algorithm
// since with queue it's easy to handle FIFO items
struct Queue {
    int size;
    struct Page queueItems[MAX_MM_PAGE]; //max queue size is 4
} queue;

// define suitable data structure to handle LRU properties 
// data is Page

void updateTimes(){
    for(int i= 0; i < queue.size; i ++){
        queue.queueItems[i].time++;
    }
}

void initPageTable() {
    for(int i = 0; i < MAX_VM_PAGE; i ++){
        pageTable[i].pageID = i;
        pageTable[i].validBit = 0;  // All virtual pages are initially on disk so validBit = 0
        pageTable[i].dirtyBit = 0; 
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
            printf("Items at %d: pageID [%d]\n", i, queue.queueItems[i].mmPageID);
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

/*void printVM(){ //helper function. prints the vm table.
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

int convertVAtoIndex(int va) {
    return va & 7;
}

int convertVAtoPageID(int offset, int va) {
    // formula: pageID * NUM_ADDRESSES + offset = VA
    // pageID = (VA - off) / NUM_ADDRESSES
    return (va - offset) / NUM_ADDRESSES;
}

int isAPageFault(int pageID) { //checks if incoming page is a fault

    for(int i = 0; i < queue.size; i++){
        
        if (pageID == queue.queueItems[i].pageID){     // if the pageID has been loaded to main (exists in queue)
            queue.queueItems[i].time = 0;
            return 0; //not a hit since value already exists, update accessed value of item 
        }
    }
    return 1; //page fault. value isn't in the queue

}

int LRUFindVictimPage(){  //assume queue is full here -> returns index of item in queue to be removed since it needs to be removed by
    //pageRef is string of pages for reference - can replace with actual reference string later, or not 
    //take all of the least accessed values and find out which is the least recently used one. 
    int max = queue.queueItems[0].time; //minimum times accessed starts at first value
    for(int i = 0; i < queue.size; i++){
        if ( queue.queueItems[i].time > max ) 
        {
           max = queue.queueItems[i].time;
        }
    }

    //find the least recent page that has been accessed minimum amount of times 
    for(int i = 0; i < queue.size; i++){
        if(queue.queueItems[i].time == max){ //finds first element w that time
            return i; //returns position of element in queue
        }
    }

    return -1; //error 
}

void RemoveVictimPage(int pagePosition){ //remove the specified victim page from queue. updates pageTable

    //do something
}

void pushToRear(int mmPageID, int pageNum) { //adds new page to queue with a timeAccessed val of 1, updates pageTable
    int index = queue.size;
    queue.queueItems[index].pageID = pageNum;
    queue.queueItems[index].mmPageID = mmPageID;
    queue.queueItems[index].time = 0;
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


void eval(char **argv, int argc, enum ALGORITHM algo){
    
    int pageIndex = -1;
    int pageNum = -1;

    // print and quit commands
    if (strcmp(argv[0],"showmain") == 0){
        // print main memory with page ID
        int ID = atoi(argv[1]);
        printMM(ID);
        
    }
    else if (strcmp(argv[0],"showptable") == 0){
        showpTable();
    }
    else if(strcmp(argv[0],"showqueue") == 0){ //helper command
        printQueue();
    }
    /*else if(strcmp(argv[0],"printMM") == 0){ //helper command
        printMM();
    }*/
    else if (strcmp(argv[0],"quit") == 0){
        exit(0);
    }

    else {  // read and write commands which would change the internal of pageTable and memory systems 
        int VA = atoi(argv[1]);     // getting the virtual address
        int pageIndex = convertVAtoIndex(VA);           // find the pageIndex of this VA. va & 7 (might not need to call another function)
        int pageNum = convertVAtoPageID(pageIndex, VA);  // find pageID of this VA. pageID = (VA - off) / NUM_ADDRESSES
        int victimPageQueuePosition = -1; // default val of -1 for testing sake -> victim page's position in the queue
       
        //printf("page ID: %d at index %d\n", pageID, pageIndex);

        
        /*if (isAPageFault(pageNum) == 1) { // check if a Page is in queue yet. If not that means nothing in main memory -> Page fault
            print("A Page Fault Has Occurred!\n");
            if (queue.size == MAX_MM_PAGE) 
            {    // All pages in main are in queue -> need to choose a victim page for eviction
                    // Below is the job of removing victim page from main memory 
                    if(algo == FIFO){
                        victimPageQueuePosition = 0; //since FIFO is always the first element 
                    }
                    else{
                        victimPageQueuePosition = LRUFindVictimPage(); //find by least recent accessed page 
                    }
                    
                    // copy main page to disk page. this is the eviction we would need to do 
                   
                    pageTable[pageNum].validBit = 0; // set the pageID in pageTable validBit = 0
                    pageTable[pageNum].dirtyBit = 0; // set the pageID in pageTable dirtyBit = 0
                    pageTable[pageNum].pageNum = pageNum; // set the pageID in pageTable pageNum = pageID
                    
                    popByPosition(victimPageQueuePosition); // pop by the position; resets page times access to 0
    
            }
            else
            {

            
                // copy disk page to main memory0`
                // find available lowest mmPageID !! 


                // doing actual copy

                // set pageTable validBit at the pageNum to 1 since we load the pageID in main memory
                // push this page to Queue; times accessed should equal 1 here since it got added to the queue 
                // set the pageNum on the pageTable this mmPageID 

            }
        
        }*/

        if (algo == FIFO) {
            if (isAPageFault(pageNum) == 1) {
                printf("A Page Fault Has Occured!\n");
                if (queue.size == MAX_MM_PAGE) { // All pages in main are in queue -> need to choose a victim page for eviction
                    copyPageToMemory(mm[queue.queueItems[0].mmPageID], vm[queue.queueItems[0].pageID]);     // queue's full. copy the victim page back to disk
                    // for FIFO. First item in queue will be evicted
                    // victim page is copied back to disk with page number is the same as the number virtual page
                    // clear validBit and dirtyBit since the page is no longer in main memory
                    pageTable[queue.queueItems[0].pageID].validBit = 0;
                    pageTable[queue.queueItems[0].pageID].dirtyBit = 0;
                    pageTable[queue.queueItems[0].pageID].pageNum = pageTable[queue.queueItems[0].pageID].pageID;
                    popByPosition(0);

                    // all statements above will come into this
                    evictVictimPagebyPosition(victimPageQueuePosition);
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
            // page might be already in queue which is on main
            int pos = posOfPageInQueue(pageNum);
            if (strcmp(argv[0], "write") == 0) {
                mm[queue.queueItems[pos].mmPageID][pageIndex] = atoi(argv[2]);      // write content to main memory
                pageTable[pageNum].dirtyBit = 1;
            }
            else {  // read content of a page. either -1 when page fault or the real content
                printf("%d\n", mm[queue.queueItems[pos].mmPageID][pageIndex]);
            }

        }

        
        else {  // here is for LRU Algorithm. Separating two algorithm at the beginning is easier to handle
                // Optimization can be done later
            if (isAPageFault(pageNum) == 1) {
                printf("A Page Fault Has Occured!\n");
                int pos;
                if (queue.size == MAX_MM_PAGE) { // All pages in main are in queue -> need to choose a victim page for eviction
                    pos = LRUFindVictimPage();
                    copyPageToMemory(mm[queue.queueItems[pos].mmPageID], vm[queue.queueItems[pos].pageID]);     // queue's full. copy the victim page back to disk
                    // for LRU. \Least recently used item in queue
                    // victim page is copied back to disk with page number is the same as the number virtual page
                    // clear validBit and dirtyBit since the page is no longer in main memory
                    pageTable[queue.queueItems[pos].pageID].validBit = 0;
                    pageTable[queue.queueItems[pos].pageID].dirtyBit = 0;
                    pageTable[queue.queueItems[pos].pageID].pageNum = pageTable[queue.queueItems[pos].pageID].pageID;
                    popByPosition(0);
                }

                // else there is still available page in 
                updateTimes(); //update times of everything there
                int availableMMPageID = findLowestMMPageID();
                pushToRear(availableMMPageID, pageNum);
                // copy disk page into main memory (loading page to main memory)
                copyPageToMemory(vm[pageNum], mm[availableMMPageID]);
                // update pageTable is we already loaded page to main memory
                pageTable[pageNum].validBit = 1;    // already in main memory
                pageTable[pageNum].pageNum = availableMMPageID;


            }
           
           

        }




    }
}
int main(int argc, char* argv[]){

    char input[MAX_LINE];       //user input
    int u_argc = 0;             //user entered: number of arguments 
    char* u_argv[MAX_LINE];     //user entered: list of arguments. First arg is command.
    enum ALGORITHM algo = FIFO; // default page replacement argorithm
    //char pageRepAlgo[MAX_LINE];

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
