
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "book.h"

int pow(int a,int b){
    int sum = 1;
    for(int i = 0;i<b;i++){
        sum*=a;
    }
    return sum;
}
int argument_to_int(int index,char* argv[]){
    int sum = 0;
    for(int j = strlen(argv[index])-1;j>=0;j--){
        if(argv[index][j]<'0' || argv[index][j]>'9'){
            printf("Bad arguments of amount of blocks");
            exit(1);
        }
        sum += (argv[index][j] - '0')*pow(10,(strlen(argv[index])-1)-j);
    }
    return sum;
}
int main(int argc,char* argv[]){
    int amountOfBlocks=0;
    int lengthOfBlock = 0;
    int kindOfAllocation = 0;
    for(int i = 1;i<argc;i++){
        printf("argv[%u] = %s\n", i, argv[i]);
        for(int j = 0;j<sizeof(argv)/sizeof(argv[0]);j++){

        }
    }
    if(argc>3){
        amountOfBlocks = argument_to_int(1,argv);
        lengthOfBlock = argument_to_int(2,argv);
        if(argv[3][0]=='1')kindOfAllocation=1; // dynamic
        else if(argv[3][0]=='0')kindOfAllocation=0; // static
        else{
            printf("Bad kind of allocation");
            exit(1);
        }

    }
    if(1==kindOfAllocation){
        struct dynamicBook book;
        for(int i = 4;i<argc;i++) {
            if (strcmp("create", argv[i]) == 0 && kindOfAllocation == 1) { //dziala
                book = createDynamicBook(amountOfBlocks, lengthOfBlock);
                for(int i = 0;i<book.width;i++){
                    for(int j = 0;j<book.height;j++){
                        book.tab[i][j]='A' + (random() % 26);
                    }
                }
            } else if (strcmp("erase_blocks", argv[i]) == 0) { //dziala
                i++;
                if(i==argc){
                    printf("Bad arguments in command line");
                    exit(1);
                }
                int number_of_blocks_to_delete = argument_to_int(i,argv);
                printf("\n%d %d\n",i,number_of_blocks_to_delete);
                book=deleteBlocksToDynamicBook(book,number_of_blocks_to_delete);
            } else if (strcmp("add_blocks", argv[i]) == 0) { //dziala
                i++;
                if(i==argc){
                    printf("Bad arguments in command line");
                    exit(1);
                }
                int number_of_blocks_to_add = argument_to_int(i,argv);
                book=addBlocksToDynamicBook(book,number_of_blocks_to_add);
            } else if (strcmp("nearest_sum_of_char_to_element", argv[i]) == 0) {//dziala
                i++;
                if(i==argc){
                    printf("Bad arguments in command line");
                    exit(1);
                }
                int element = argument_to_int(i,argv);
                int idOfBlock = dynamicSumASCII(book,element);
            } else if (strcmp("delete_book", argv[i]) == 0) {//działa
                deleteDynamicBook(book);
            }else{
                printf("Bad commend");
                exit(1);
            }
        }
    }
    else{
        struct staticBook book;
        for(int i = 4;i<argc;i++) {
            if (strcmp("create", argv[i]) == 0) { // dziala
                book = createStaticBook(amountOfBlocks,lengthOfBlock);
                for(int i = 0;i<book.width;i++){
                    for(int j = 0;j<book.height;j++){
                        book.tab[i][j]='A' + (random() % 26);
                        printf("%d ",book.tab[i][j]);
                    }
                    printf("\n");
                }
            } else if (strcmp("erase_blocks", argv[i]) == 0) {//dziala
                i++;
                if(i==argc){
                    printf("Bad arguments in command line");
                    exit(1);
                }
                int number_of_blocks_to_delete = argument_to_int(i,argv);
                book = deleteBlocksToStaticBook(book,number_of_blocks_to_delete);
            } else if (strcmp("add_blocks", argv[i]) == 0) { //dziala
                i++;
                if(i==argc){
                    printf("Bad arguments in command line");
                    exit(1);
                }
                int number_of_blocks_to_add= argument_to_int(i,argv);
                book = addBlocksToStaticBook(book,number_of_blocks_to_add);
            } else if (strcmp("nearest_sum_of_char_to_element", argv[i]) == 0 ) { //dziala
                i++;
                if(i==argc){
                    printf("Bad arguments in command line");
                    exit(1);
                }
                int element = argument_to_int(i,argv);
                int idOfBlock = staticSumASCII(book,element);
            } else if (strcmp("delete_book", argv[i]) == 0) {
                deleteStaticBook(book);
            }
        }
    }

    return 0;
}



