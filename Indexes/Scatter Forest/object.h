#ifndef MIN
#define MIN(x, y) ((x<y)? (x): (y))
#define MAX(x, y) ((x>y)? (x): (y))
#endif

#ifndef __Object
#define __Object


#define PIVOTN 0.2





#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include "basemetric.h"
#include "./iopack/RAF.h"
using namespace std;


extern int n, m;
extern int f;
extern int* metricm;
extern float* metricmaxdis;
extern float* metricmeandis;
extern double compdists;
class Object
{
public:
	//data
	int id;		
	int sized, sizes;
	int* size;
	float* datad;
	char* datas;

	//tmpsize
	int istravel;
	float* ansres;
	float dist;           

	
	void ini(string str);
	void outnode();
	Object();	
	Object(Object* o);
	~Object();
	int write_to_buffer(char* buffer);
	int read_from_buffer(char* buffer);
	int getsize();

	float distance(Object* other, int m) ;

//	bool operator < (const Object& a) const;
//	bool operator > (const Object& a) const;
};

class Objectarr
{
public:
	Object** arr;	
	Objectarr();
	~Objectarr();
	
	void readfile(string filedata);	
};


#endif

