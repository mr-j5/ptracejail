#include "trace.h"


int main(int argc, char ** argv)
{

    char* path;
    int opt;
    if(argc < 4){
        printf("Usage:\n ./ptracejail -p [path to restricted file or folder] [path to program] [arg1 for program] [arg2 for program] ... \n");
        return -1;    
    }
    
    while((opt = getopt(argc, argv, ":p:e:")) != -1){
        switch(opt){
        case 'p':
            printf("Access to %s restricted\n", optarg);
            path = optarg;
            break;
        case ':':
            printf("option p needs a value\n");
            return -1;
         case '?':
            printf("undefined option specified: %c\n", optopt);
            return -1;
        }
    }
    
    

    //fork and run child
    pid_t ch = fork();
    // char* argvch[] ={
    //         "/usr/bin/wget",
    //         "google.com",
    //         NULL

    //     } ;

    if(ch == 0){
        
        execChild(argv[3], argv+3);
    }else{
        //trace the child to intercept syscalls
        traceChild(ch, path);
    }

    return 0;
}
