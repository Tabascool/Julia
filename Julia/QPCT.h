#ifndef __QPCT_H__
#define __QPCT_H__


#define START_QPCT   LARGE_INTEGER StartingTime, EndingTime;\
                     LARGE_INTEGER Frequency;\
					 QueryPerformanceFrequency(&Frequency);\
					 printf( ".. START ...\n");\
					 QueryPerformanceCounter(&StartingTime);

#define STOP_QPCT    QueryPerformanceCounter(&EndingTime);\
					 double elapsedTime=(float)(EndingTime.QuadPart-StartingTime.QuadPart)/Frequency.QuadPart*1000.; \
					 printf( ".. STOP ...\n");\
					 printf("Elapsed time : %fms \n",elapsedTime);


#endif // __QPCT_H__