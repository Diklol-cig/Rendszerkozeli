#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>

int main()
{
	int f,i;
	float A[1000];

	srand(time(NULL));
	for(i=0;i<1000;i++){
		A[i]=(float)rand()/RAND_MAX*1000000.0;
	}
	f=open("nums.txt", O_CREAT|O_TRUNC|O_WRONLY);

	if(f<0){
		write(2,"FILE ERROR\n",12);
		return 1;
	}
	write(f,A,sizeof(A));
	close(f);
	return 0;
}