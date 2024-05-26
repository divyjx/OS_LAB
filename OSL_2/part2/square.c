#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    
    if (argc > 1) {
        sprintf(argv[argc-1] , "%d", atoi(argv[argc-1])*atoi(argv[argc-1]));
        printf("Square: Current process id: %d, Current result: %s\n", getpid(), argv[argc-1]);
        ++argv;
        execvp(argv[0], argv);
    }
    
    return 0;
}
