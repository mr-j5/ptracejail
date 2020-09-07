#ifndef STR_BUILDER
#define STR_BUILDER


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct strbs
{
    long len;
    char* str;
    struct strbs* nxt;
} strb;

int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

strb* initBuildStr(){
    return NULL;
}


strb* initBuildStrP(char* init){
    strb *str = (strb*)malloc(sizeof(strb)); 
    str->len = strlen(init);
    str->str= init;
    str->nxt= NULL;
    return str;
}

strb* buildStr(strb * str, char* add){
    if (str == NULL){
        str = initBuildStrP(add);
        return str;
    }
    strb *s2 = initBuildStrP(add);
    s2->len = str->len + s2->len;
    s2->nxt = str;

return s2;

}

char* getStr(strb * src){
    char* dest = (char*)malloc(src->len + 1);
    dest[src->len] = '\0';
    char* write = (char*)((long)dest + src->len) ;
    while(src->nxt != NULL){
        long off = (src->len - src->nxt->len);
        write = (char*) (long)write - off;
        memcpy(write, src->str, src->len - src->nxt->len);
        src = src->nxt;
    }
        memcpy(dest, src->str, src->len);

    return dest;
}

void printStr(strb* a){

    while (a!=NULL)
    {
        printf("{len: %ld, str: %s}->", a->len, a->str);
        a=a->nxt;
    }   
    printf("NULL\n");
}

#endif