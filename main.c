//test setting up the github
#include <stdlib.h>
#include<string.h>
#include<stdio.h>

#define MAX_LINE 80
#define MAX_MM_PAGE 4
#define MAX_VM_PAGE 16
#define NUM_ADDRESSES 8

typedef ALGORITHM {
    FIFO,
    LRU
};

//define page 
struct page_Table{
    int pageID;     // 0 - 3 (4 pages) for main memory and 0 - 15 (16 pages) for disk/virtual memory
    int validBit;   // 1: page is in main memory, 0: page is on virtual memory
    int dirtyBit;   // 1: page has been written to while in main memory, else 0
    int pageNum;    // page number of virtual page either on main memory (validBit = 1) or disk/virtual memory (validBit = 0)
} pageTable[MAX_VM_PAGE];

//int vm[128] = { [0 ... 127 ] = -1} ;    //virtual memory, 128 addresses init to -1, also known as "disk"
//int mm[32] = {[0 ... 31] = -1};         //main memory, 32 addresses init to -1

// using 2-d for both the disk and main memory [page ID][address ID]
// memory layouts would be: disk 16 x 8 and main memory 4 x 8 
// easier than 1-d when finding page ID
int vm[MAX_VM_PAGE][MAX_ADDRESSES];
int mm[MAX_MM_PAGE][MAX_ADDRESSES;]


//Note: Make sure to refer back to 3.1 to determine offsets for pageNum and accessing addresses

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
        for (int j = 0; j < MAX_ADDRESSES; j++) {
            vm[i][j] = -1;
        }
    }
    for (int i = 0; i < MAX_MM_PAGE; i++) {
        for (int j = 0; j < MAX_ADDRESSES; j++) {
            mm[i][j] = -1;
        }
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
    for(int i = 0; i < MAX_VIRTUAL_PAGE; i++){
        printf("%d:%d:%d:%d\n",pageTable[i].pageID,pageTable[i].validBit,pageTable[i].dirtyBit,pageTable[i].pageNum);
    }
}

/*void printVM(){ //helper function. prints the vm table.
    for(int i = 0;i < 128; i ++){
        printf("%d\n",vm[i]);
    }
}*/

void printMM(int pageID){   // showmain command
    for(int i = 0; i < MAX_ADRESSES; i ++){
        // print contents in pageID with addr = offset of MAX_ADRESSES + base_ID
        address = (pageID * MAX_ADRESSES) + i;
        printf("%d:%d\n", address, mm[pageID][i]);
    }
}


void eval(char **argv, int argc){
    if(strcmp(argv[0],"read") == 0){
        //TO-DO
    }
    else if(strcmp(argv[0],"write") == 0 ){
        //TO-DO
    }
    else if(strcmp(argv[0],"showmain") == 0){
        // print main memory with page ID
        int ID = atoi(argv[1]);
        printMM(ID);
        
    }
    else if(strcmp(argv[0],"showptable") == 0){
        showpTable();
    }
    /*else if(strcmp(argv[0],"printVM") == 0){ //helper command
        printVM();
    }
    else if(strcmp(argv[0],"printMM") == 0){ //helper command
        printMM();
    }*/
    else if(strcmp(argv[0],"quit") == 0){
        exit(0);
    }
}
int main(int argc, char * argv[]){

    char input[MAX_LINE];   //user input
    int argc;               //user entered: number of arguments 
    char* argv[MAX_LINE];   //user entered: list of arguments. First arg is command.
    enum ALGORITHM algo = FIFO // default argorithm


    //strcpy(rpAlg, argv[1]);  //pg replacement alg


    // Initialize the memory and page table 
    initMemory();
    initPageTable();        

    //take user input
    while(1) //loop until quit is entered
    {
        fflush(stdin);
        fflush(stdout);
        argc = 0;

        printf("> ");
        fgets(input,MAX_LINE,stdin);
        printf("finish\n");

        distributeInput(input, &argc, argv);
        eval(argv, argc);
        
    }


    //test
    return 0;
}
