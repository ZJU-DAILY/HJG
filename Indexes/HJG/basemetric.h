#ifndef MIN
#define MIN(x, y) ((x<y)? (x): (y))
#define MAX(x, y) ((x>y)? (x): (y))
#endif

#ifndef __Basemetric
#define __Basemetric


double baseme(double* a1, double* a2, char* s1, char* s2, int m,int size);
double trans(double ddd1, double ddd2);
double trans2(double ddd1, double ddd2);
extern int* metricm;
extern double* metricmaxdis;
#endif