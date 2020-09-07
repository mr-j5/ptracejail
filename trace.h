#ifndef MY_TRACE
#define MY_TRACE

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <syscall.h>
#include <sys/reg.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include "strBuilder.h"


char* readTraceeStringAtAddr(pid_t ch, void* addr){
    strb *str = initBuildStr();
    int endOfStr = 1;
    int offset = 0;
     
    while(endOfStr == 1){
    char * nstr = (char*)malloc(sizeof(long));
    long r = ptrace(PTRACE_PEEKTEXT,  ch, (void*)((unsigned long long)addr+offset*sizeof(long)), 0);
    memcpy((void*)nstr, (void*)&r, sizeof(long) );
    str = buildStr(str, nstr);
    
    //found end of string?
    int i = 0;    
    while(i < sizeof(long)){
        if(*(nstr+i) == '\0'){
            endOfStr = 0;
        }
        i++;
    }
    offset++;
    }
    return getStr(str);
}

int handleSysCall(pid_t ch, struct user_regs_struct *regs, char* restrictPath){
    //syscall number
    long syscall = regs->orig_rax;
    switch(syscall){
        case SYS_open:
            //if the path accesed by open is restricted, kill program
            printf("Accessed file at");
            char* openstr = readTraceeStringAtAddr(ch, (void*)regs->rdi);
            printf(" %s\n", openstr);
            if(startsWith(restrictPath, openstr)){
                //system call tried to access restricted file
                kill(ch, SIGKILL);
                printf("++++Exited, child tried to access restricted file: %s++++\n", openstr);
                return SYS_exit;
            }
        break;
        case SYS_exit:
        case SYS_exit_group:
            ptrace(PTRACE_SYSCALL, ch, 0, 0);
            waitpid(ch, 0, 0);
            return SYS_exit;
        break;
        default: break;
    }

    return 0;
}

int handleSysCallRet(pid_t ch, struct user_regs_struct *regs){

}

int execChild(char* path, char* argv[])
{
    //this child will be traced by parent
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    //execl("/usr/bin/wget", "wget", "google.com", NULL);
    //printf("%s, %s, %s, %s", path, argv[0], argv[1], argv[2]);
    return execv(path, argv);
}

int traceChild(pid_t ch, char* restrictPath){
    printf("tracing...");
    //continue child program
    waitpid(ch, 0, 0);
    //kill child if parent exits
    ptrace(PTRACE_SETOPTIONS, ch, 0, PTRACE_O_EXITKILL);
    while(1){
        //stop child at next system call
        ptrace(PTRACE_SYSCALL, ch, 0, 0);
        waitpid(ch, 0, 0);

        //obtain values in registers
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, ch, 0, &regs);
        
        if(handleSysCall(ch, &regs, restrictPath) == SYS_exit){
            return SYS_exit;
        }

        //execute syscall and 
        ptrace(PTRACE_SYSCALL, ch, 0, 0);
        waitpid(ch, 0, 0);

        //we are not using the return value yet
    }
}

#endif