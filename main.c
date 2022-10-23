//test setting up the github
#include <stdlib.h>
#include<string.h>
#include<stdio.h>

#define MAX_LINE 80
//define page 
struct page_entry{
    int validBit;
    int dirtyBit;
    int pageNum; //page number where virtual page is found in main memory
};

//each address holds a single integer
int vm[128] = { [0 ... 127 ] = -1} ; //virtual memory,128 addresses init to -1, also known as "disk"
int mm[32] = {[0 ... 31] = -1}; //main memory, 32 addresses init to -1
struct page_entry vmPageTable[16]; //virtual memory has 16 pages
char rpAlg[MAX_LINE]; //page replacement algorithm 

//Note: Make sure to refer back to 3.1 to determine offsets for pageNum and accessing addresses

void initPageTable(){
    for(int i = 0; i < 16; i ++){
        vmPageTable[i].validBit = 0;
        vmPageTable[i].dirtyBit = -999; //test init val REMOVE/CHANGE LATER
        vmPageTable[i].pageNum = -999; //test init val REMOVE/CHANGE LATER
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
void showpTable(){ //prints pagetable
    for(int i = 0; i < 16; i++){
        printf("%d:%d:%d:%d\n",i+1,vmPageTable[i].validBit,vmPageTable[i].dirtyBit,vmPageTable[i].pageNum);
    }
}

void printVM(){ //helper function. prints the vm table.
    for(int i = 0;i < 128; i ++){
        printf("%d\n",vm[i]);
    }
}
void printMM(){ //helper function. prints the mm table.
    for(int i = 0;i < 32; i ++){
        printf("%d\n",mm[i]);
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
            //TO-DO
        }
        else if(strcmp(argv[0],"showptable") == 0){
            showpTable();
        }
        else if(strcmp(argv[0],"printVM") == 0){ //helper command
            printVM();
        }
        else if(strcmp(argv[0],"printMM") == 0){ //helper command
            printMM();
        }
        else if(strcmp(argv[0],"quit") == 0){
            exit(0);
        }
}
int main(int argc, char * argv[]){

    char input[MAX_LINE]; //user input
    int u_argc; //user entered: number of arguments 
    char* u_argv[MAX_LINE]; //user entered: list of arguments. First arg is command.

    strcpy(rpAlg,argv[1]); //pg replacement alg
    initPageTable(); //fill valid bit with 0

    //take user input
    while(1) //loop until quit is entered
    {
        fflush(stdin);
        fflush(stdout);
        u_argc = 0;
        printf("> ");
        fgets(input,MAX_LINE,stdin);
        printf("finish\n");
        distributeInput(input,&u_argc,u_argv);
        eval(u_argv,u_argc);
        
    }


    //test
    return 0;
}