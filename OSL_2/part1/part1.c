#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    
    char str[] = "Hello World";
    int len = (int) (strlen(str));
    int pid = 0;

    // printf("Parent process id - %d\n", getpid());
    
    for (int  i = 0; i < len; i++)
    {
        if (pid == 0){
            printf("%c %d\n", str[i], getpid());
            pid = fork();
            sleep(1 + rand()%4);
        }
        else wait(NULL);
    }

    return 0;
}
