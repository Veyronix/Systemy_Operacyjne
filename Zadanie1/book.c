#include "book.h"
#include <stdio.h>
#include <stdlib.h>


struct staticBook createStaticBook(int width,int height){
    static struct staticBook book;
    int maxwidth = sizeof(book.tab)/sizeof(book.tab[0]);
    int maxheight = sizeof(book.tab[0])/sizeof(book.tab[0][0]);

    if(width>maxwidth || height>maxheight || width == 0 || height == 0){
        printf("Maximum widht of table is %d and height is %d",maxwidth,maxheight);
        exit(1);
    }
    book.height=height;
    book.width =width;
    for(int i = 0;i<maxwidth;i++){
        for(int j = 0;j<maxheight;j++)
        {
            book.tab[i][j]=0;
        }
    }
    return book;
}


struct dynamicBook createDynamicBook(int width,int height){
    struct dynamicBook book;
    book.tab = calloc(width,sizeof(char*));
    for(int i = 0;i< width;i++){
        book.tab[i] = calloc(height, sizeof(char));
    }
    book.height=height;
    book.width=width;
    return book;
}
void deleteDynamicBook(struct dynamicBook book){
    for(int i = 0;i<book.width;i++){
        free(book.tab[i]);
    }
    free(book.tab);
}
void deleteStaticBook(struct staticBook book){
    for(int i =0;i<book.width;i++)
    {
        for(int j = 0;j<book.height;j++)
        {
            book.tab[i][j]=0;
        }
    }
}

struct staticBook addBlocksToStaticBook(struct staticBook book,int numberOfBlocks){
    if(book.height+numberOfBlocks>sizeof(book.tab)/sizeof(book.tab[0])){
        printf("You cant enlarge table. Maximum number od blocks is %d",sizeof(book.tab)/sizeof(book.tab[0]));
        return book;
    }
    book.width+=numberOfBlocks;
    return book;
}
struct dynamicBook addBlocksToDynamicBook(struct dynamicBook book,int numberOfBlocks){
    if(numberOfBlocks <1){
        printf("Number of blocks must be 1 or more");
        return book;
    }
    book.tab = realloc(book.tab,(book.width+numberOfBlocks)*sizeof(char*));
    for(int i = book.width;i<book.width+numberOfBlocks;i++){
        book.tab[i]=book.tab[i] = calloc(book.height, sizeof(char));
    }
    book.width+=numberOfBlocks;
    return book;
}
struct staticBook deleteBlocksToStaticBook(struct staticBook book,int numberOfBlocks){
    if(numberOfBlocks>=book.width){
        printf("Cant delete more blocks than book have");
        exit(1);
    }
    for(int i = book.width-1;i>=book.width-numberOfBlocks;i--){
        for(int j = 0;book.height>j;j++){
            book.tab[i][j]=0;
        }
    }
    book.width-=numberOfBlocks;
    return book;
}
struct dynamicBook deleteBlocksToDynamicBook(struct dynamicBook book,int numberOfBlocks){
    if(numberOfBlocks>=book.width){
        printf("Cant delete more blocks than book have");
        exit(1);
    }
    for ( size_t i = 1; i <= numberOfBlocks; i++ )
        free( book.tab[book.width - i] );
    book.tab = realloc(book.tab,(book.width-numberOfBlocks)*sizeof(char*));
    book.width-=numberOfBlocks;

    return book;
}
int dynamicSumASCII(struct dynamicBook book,int element){
    int tab[book.width];
    for(int i = 0;i<book.width;i++)tab[i]=0;
    for(int i = 0;i<book.width;i++) {
        for (int j = 0; j < book.height; j++) {
            tab[i] += book.tab[i][j];
        }
    }
    int min = abs(tab[0]-element);
    int idmin = 0;
    for(int i = 0;i<book.width;i++){
        if(abs(tab[i]-element)<min){
            idmin = i;
            min = abs(tab[i]-element);
        }
        //printf("%d ",tab[i]);
    }
    return idmin;
}
int staticSumASCII(struct staticBook book,int element){
    int tab[book.width];
    for(int i = 0;i<book.width;i++)tab[i]=0;
    for(int i = 0;i<book.width;i++) {
        for (int j = 0; j < book.height; j++) {
            tab[i] += book.tab[i][j];
        }
    }
    int min = abs(tab[0]-element);
    int idmin = 0;
    for(int i = 0;i<book.width;i++){
        if(abs(tab[i]-element)<min){
            idmin = i;
            min = abs(tab[i]-element);
        }
        //printf("%d ",tab[i]);
    }
    return idmin;
}
