#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h> 

int main() 
{ 
  
    // make two process which run same 
    // program after this instruction 
    int code = fork(); 
    if (code == 0) {
        printf("Return in child process, Hello 世界!\n"); 
        exit(0);
    } else if (code > 0) {
        printf("Return in main process, Hello world!\n"); 
        exit(0);
    } else {  // code < 0
        printf("UNK error...Hello world!\n"); 
        exit(1);
    }
} 
