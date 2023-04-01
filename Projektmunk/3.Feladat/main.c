#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

char bitmap[10000000];
int file_size_bytes;
int padding;
int image_width;
int file_size_bytes;
int center_line;

void write_int(int *p, int value){
    p[0] = value;
    p[1] = value >> 8;
    p[2] = value >> 16;
    p[3] = value >> 24;
}

int int_pow(int a, int b) {
    int result = 1;
    while (b > 0) {
        if (b & 1) {
            result *= a;
        }
        a *= a;
        b >>= 1;
    }
    return result;
}

void BMPcreator(int *Values, int NumValues){
    padding = 32-(NumValues%32);
    image_width = NumValues;
    printf("%d\n", image_width);
    file_size_bytes = (image_width*NumValues/8)+62;
    printf("File size in bytes: %d\n", file_size_bytes);
    // -- FILE HEADER -- //
    // bitmap signature
    bitmap[0] = 'B';
    bitmap[1] = 'M';
    // file size
    write_int(&bitmap[2], file_size_bytes);
    // reserved field (in hex. 00 00 00 00)
    write_int(&bitmap[6], 0);
    // offset of pixel data inside the image
    write_int(&bitmap[10], 62);
    // -- BITMAP HEADER -- //
    // header size
    write_int(&bitmap[14], 40);
    // width of the image
    write_int(&bitmap[18], image_width);
    // height of the image
    write_int(&bitmap[22], image_width);
    // reserved field
    bitmap[26] = 0x01;
    bitmap[27] = 0x00;
    // number of bits per pixel
    bitmap[28] = 0x01;
    bitmap[29] = 0x00;
    // compression method (no compression here)
    for(int i = 30; i < 34; i++) bitmap[i] = 0; 
    // size of pixel data
    bitmap[34] = 0;
    bitmap[35] = 0;
    bitmap[36] = 0;
    bitmap[37] = 0;
//-----------------------------------------TO_DO-----------------------------------------
    // horizontal resolution of the image - pixels per meter (2835)
    write_int(&bitmap[38], 3937);
    // vertical resolution of the image - pixels per meter (2835)
    write_int(&bitmap[42], 3937);
//-----------------------------------------TO_DO-----------------------------------------
    write_int(&bitmap[46], 0);
    write_int(&bitmap[50], 0);
    // color pallette information
    bitmap[54] = 0xCC;
    bitmap[55] = 0x00;
    bitmap[56] = 0xCC;
    bitmap[57] = 0xFF;
    bitmap[58] = 0x00;
    bitmap[59] = 0xFF;
    bitmap[60] = 0x00;
    bitmap[61] = 0xFF;


    int line_bit_count = image_width + padding;
    center_line = image_width/2+1;
    printf("Nyilvan a loop elott bassza meg magat ez a szar");
    
    for(int i = 0; i<NumValues; i++){

        char mask = (char)int_pow(2, 7 - (i % 8));

        int current_line_offset = center_line + Values[i];

        int offset_line_index = ((image_width + padding)/8) * current_line_offset;

        int current_byte_index = offset_line_index + ( i / 8 );

        bitmap[62 + current_byte_index] |= mask;
    }
    
    int f = open("chart.bmp", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    
    write(f, bitmap, file_size_bytes); //File size in bytes
    close(f);
}

int calculate_size(int min, int sec){
   if(min>= 0 && min < 15){
       if(60*(min)+sec<100){
              return 100;
         }
         else{
              return 60*(min)+sec;
       }
   }
   if(min>= 15 && min < 30){
       if(60*(min-15)+sec<100){
              return 100;
         }
         else{
              return 60*(min-15)+sec;
       }
    }
    if(min>= 30 && min < 45){
        if(60*(min-30)+sec<100){
            return 100;
        }
        else{
            return 60*(min-30)+sec;
        }
    }
    if(min>= 45 && min <=59){
       if(60*(min-45)+sec<100){
              return 100;
         }
         else{
              return 60*(min-45)+sec;
       }
    }
}

int Measurement(int **p_values)
{
    double rand_num;
    struct tm *T4;
    int T2, T3;
    time_t T1;
    T2 = time(&T1);
    T4 = localtime(&T1);
    int size_of_array = calculate_size(T4->tm_min,T4->tm_sec);
    // printf("%d\n", size_of_array);
    *p_values=malloc(size_of_array*sizeof(int));
    (*p_values)[0]=0;
    for(int i=1;i<size_of_array;i++)
    {
        rand_num= (double)rand()/((unsigned)RAND_MAX+1); // 0.0 <= y < 1.0
        if(rand_num<=0.48571)
        {
            (*p_values)[i]=(*p_values)[i-1]+1;
        }
        else if(rand_num>0.48571&&rand_num<0.8348)
        {
            (*p_values)[i]=(*p_values)[i-1]-1;
        }
        else
        {
            (*p_values)[i]=(*p_values)[i-1];
        }
    }
    return size_of_array;
}

int main ()
{
    srand(time(NULL));
    int *values=NULL;    
    int length=Measurement(&values);     
    printf("The length of the array is: %d\n",length);
    BMPcreator(values, length);
    for(int i=0;i<length;i++)
    {
        printf("%d ",values[i]);
    }   
    puts("");
  return 0;
}