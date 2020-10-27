
// to compile: gcc cpuusage.c -o cpu
// to run ./cpu filename.txt

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
 #include <unistd.h>


int main(int argc, char *argv[])
{
    long double a[4], b[4], loadavg;
    char filename[80];
	FILE *fp;
    char dump[50];

    if( argc != 2 )
    {
        printf("ERROR! Run using following: ./cpu filename.txt\n");
        return -1;
    }

    strcpy(filename, argv[1]); // save filename

    if((fp=fopen(filename, "w+"))==NULL) {
    	printf("Cannot open file.\n");
  	}

	fclose(fp); // close file handler

    for(;;)
    {
        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
        fclose(fp);
        sleep(1);

        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
        fclose(fp);

        loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
        // printf("The current CPU utilization is : %Lf\n",loadavg);
        
        // logging
        if((fp=fopen(filename, "a"))==NULL) {
    	    printf("Cannot open file for logging.\n");
  	    }

        fprintf(fp, "%Lf\n", loadavg); 
        fclose(fp); // close file handler
    }

    return 0;
}