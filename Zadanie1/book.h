//
// Created by bartek on 07.03.18.
//
struct staticBook {
    char  tab[1000][1000];
    int   width;
    int   height;
};

struct dynamicBook {
    char** tab;
    int   width;
    int   height;
};


struct staticBook createStaticBook(int width,int height);
struct dynamicBook createDynamicBook(int width,int height);
void deleteDynamicBook(struct dynamicBook book);
void deleteStaticBook(struct staticBook book);
struct staticBook addBlocksToStaticBook(struct staticBook book,int numberOfBlocks);
struct dynamicBook addBlocksToDynamicBook(struct dynamicBook book,int numberOfBlocks);
struct staticBook deleteBlocksToStaticBook(struct staticBook book,int numberOfBlocks);
struct dynamicBook deleteBlocksToDynamicBook(struct dynamicBook book,int numberOfBlocks);
int staticSumASCII(struct staticBook book,int element);
int dynamicSumASCII(struct dynamicBook book,int element);






