#ifndef MIN
#define MIN(x, y) ((x<y)? (x): (y))
#define MAX(x, y) ((x>y)? (x): (y))
#endif

#ifndef __Basemetric
#define __Basemetric


float baseme(float* a1, float* a2, char* s1, char* s2, int m,int size);
float trans(float ddd1, float ddd2);
float trans2(float ddd1, float ddd2);
extern int* metricm;
extern float* metricmaxdis;
#endif