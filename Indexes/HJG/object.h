#ifndef MIN
#define MIN(x, y) ((x<y)? (x): (y))
#define MAX(x, y) ((x>y)? (x): (y))
#endif

#ifndef __Object
#define __Object

#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include "basemetric.h"
using namespace std;


extern int n, m;
extern int* metricm;
extern double* metricmaxdis;
extern double* metricmeandis;
class Object;
typedef struct ts1 {	
	double dis;
	Object* obj;
	struct ts1* next_edge;
}e_s1;

typedef struct ts2 {
	int level;
	e_s1* first_edge;
	struct ts2* next_level;
}e_s2;



typedef struct ts3 {
	int metric_num;
	e_s2* first_level;
	struct ts3* next_space;
}e_s3;

class Object
{
public:
	//data
	int id;		
	int sized, sizes;
	int* size;
	double* datad;
	char* datas;
	//tmpsize
	double* ansres;	
	double dist;           
	int objloc;
	bool visited;
	int fgg;
	bool* metric_f;
	//edges
	e_s3* all_e;
	int* edges_no;
	int* edges_same;
	int level;
	void ini(string str);
	int count();

	int count(int i);

	void outnode();
	Object();	
	
	~Object();
	void add_e(Object* o,double dis,int metric,int level);
	void remove_e(Object* o, double dis, int metric,int level);

	double distance(Object* other, int m) ;

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

