#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

/* Function pointers to hw3 functions */
void* (*mm_malloc)(size_t);
void* (*mm_realloc)(void*, size_t);
void (*mm_free)(void*);
void (*mm_print_mem)();

void load_alloc_functions() {
    void *handle = dlopen("hw3lib.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    char* error;
    mm_malloc = dlsym(handle, "mm_malloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_realloc = dlsym(handle, "mm_realloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_free = dlsym(handle, "mm_free");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_print_mem = dlsym(handle, "mm_print_mem");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }    
}

int main() {
    load_alloc_functions();
    
    int *data;
    int *data2;
    int *data3;
    int *data4;
    int isFound = 0;
    int i;
    
    data = (int *) mm_malloc(sizeof(int) * 70);
    if(data == NULL)
    {
        printf("\nSpace unavailable");
    }
    else
    {
        data[0] = 0x162;
        mm_free(data);
        printf("\nmalloc test successful!");
    }

    mm_print_mem();

    //Bu kontrol kismi malloc ile yer acilamamasi durumunun kontrol edilmesi icin olusturulmustur.
    /*
    data = (int *) mm_malloc(sizeof(int)*70);
    while(!isFound)
    {
       data2 = (int*) mm_malloc(sizeof(int)*1024*1024*10);
       //printf("\n%p",data2);
       if(data2 == NULL)
       {
            printf("\nGerekli yer acilamamistir.");
            isFound = 1;
       }
    }
    */

    
    //Bu kontrol iki bos blogun birlestirildigini gostermek icin olusturulmustur.
    /*
    data = (int *) mm_malloc(sizeof(int) * 70);
    printf("\nMALLOC data\t%p",data);
    data2 = (int*) mm_malloc(sizeof(int) * 70);
    printf("\nMALLOC data2\t%p",data2);
   	data3 = (int*) mm_malloc(sizeof(int) * 10);
   	printf("\nMALLOC data3\t%p",data3);
   	mm_free(data2);
   	mm_free(data);
    data = (int*) mm_malloc(sizeof(int) * 10);
    printf("\nMALLOC data3\t%p",data3);
   	data2 = (int*) mm_malloc(sizeof(int) * 30);
   	printf("\nMALLOC data2\t%p",data2);
    */

    /*
    data = (int *) mm_malloc(sizeof(int) * 70);
    data2 = (int *)mm_malloc(sizeof(int) * 80);
    printf("\nMALLOC data\t%p",data);
    printf("\nMALLOC data2\t%p",data2);
    data = (int *)mm_realloc(data,sizeof(int) * 60);
    printf("\nREALLOC data\t%p",data);
    data = (int *)mm_realloc(data,sizeof(int) * 80);
    printf("\nREALLOC data\t%p",data);
    */
    printf("\n\n");
    return 0;
}
