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
    int mmID;       // ID in main memory
    int mmPageID;   // page ID associated to main memory
    int position;   // position in the data structures
    int timesAccessed; // times accessed while in queue, reset to zero after page exits queue
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
        
        if (pageID == queue.queueItems[i].mmPageID){     // if the pageID has been loaded to main (exists in queue)
            queue.queueItems[i].timesAccessed++;
            return 0; //not a hit since value already exists, update accessed value of item 
        }
    }
    return 1; //page fault. value isn't in the queue, and there's no space for it.

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

    return -1; //error 
}

void RemoveVictimPage(int pagePosition){ //remove the specified victim page from queue. updates pageTable

    //do something
}

void pushToRear(int mmID,int mmPageID,int pageNum) { //adds new page to queue with a timeAccessed val of 1, updates pageTable

    struct Page newPage = { mmID, mmPageID,pageNum,1 }; //constructs a new page to be added to the queue
    if(queue.size+1 <= MAX_MM_PAGE) //check if you're allowed to addor not
    {
         //increase ssize to accomodate new element
        queue.queueItems[queue.size] = newPage;
        queue.size++;
    }
    else
    {
        printf("Queue add error.Attempting to insert element into full queue.\n");
    }
}

void popByPosition(int position){ //pops element at specified position, then shifts everything to the right left
      for (int i = position; i < queue.size -1; i++)  
        {  
            queue.queueItems[i] = queue.queueItems[i+1]; // assign arr[i+1] to arr[i]  
        }  
        queue.size --; //decrease the queue size
}



void eval(char **argv, int argc, enum ALGORITHM algo){
    
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

        
        if (isAPageFault(pageNum) == 1) { // check if a Page is in queue yet. If not that means nothing in main memory -> Page fault
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

                pageTable[pageNum].validBit = 1; // set pageTable validBit at the pageNum to 1 since we load the pageID in main memory
                // push this page to Queue; times accessed should equal 1 here since it got added to the queue 
                // set the pageNum on the pageTable this mmPageID 

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
