#include <stdio.h>
#include <stdlib.h>

#define BSIZE 512
#define PRINT_SIZE(string, size)	do{printf(string);\
									if(size/1024 <= 0){\
										printf("%ld Byte.\n" , size);\
									}else if(size/1024/1024 <= 0){\
										printf("%.2f KB.\n" , (float)size/1024);\
									}else{\
										printf("%.2f MB.\n" , (float)size/1024/1024);\
									}}while(0)
int main(int argc, char *argv[])
{
    FILE *file_src, *file_desc;
    long fsize;
    unsigned char *mbuf;
    long start=0;
    if(argc<2)
	{
        printf("No enough argvs.\n");
        return 1;
    }
    file_src=fopen(argv[1],"rb");
    if(file_src==NULL)
	{
       printf("Source file cannot open.\n");
       return 1;
    }
    file_desc=fopen(argv[2],"rb+");
    if(file_desc==NULL)
	{
       printf("Desc file cannot open.\n");
       return 1;
    }
    fseek(file_desc, 0L, SEEK_END);
    fsize = ftell(file_desc);
	PRINT_SIZE("Destination file size :", fsize);
    if(fsize==0)
	{
        unsigned char *fbuf=(unsigned char *)malloc(1474560);
        fbuf[510]= 0x55;
        fbuf[511]= 0xaa;
        fseek(file_desc, 0L, SEEK_SET);
        fwrite(fbuf,1474560,1,file_desc);
        printf("Made a new file!\n");
        free(fbuf);
    }
    fseek(file_src, 0L, SEEK_END);
    fsize = ftell(file_src);
    PRINT_SIZE("Source file size :",fsize);
    mbuf=malloc(fsize);
    fseek(file_src, 0L, SEEK_SET);
    fread(mbuf,fsize,1,file_src);
    fclose(file_src);
    
    if(argc>2)start=atoi(argv[3]);
    fseek(file_desc, start, SEEK_SET);
    fwrite(mbuf,fsize,1,file_desc);
    PRINT_SIZE("Write source file at ",start);
    fclose(file_desc);
    free(mbuf);
    return 0;
}
