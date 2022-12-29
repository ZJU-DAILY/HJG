#ifndef __sforest
#define __sforest
#include "Object.h"
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include<queue>
#include<cstdlib>


using namespace std;
extern int f;
extern double IOread;
extern double IOwrite;
class Treearr
{
public:
	int tree;
	int metric;
	int len;
	int tot;
	char* datas;
	float* datad;
	Treearr();
	Treearr(Object* o, int m);	
	void outnode();
	~Treearr();
};

class AnsLeaf
{
public:
	int metric;
	int obj;
	float dis;
	float radi;
	int loc;
	Object* o;
	AnsLeaf(int i, int j, float k);
	~AnsLeaf();
};

class SF
{
public:	
	vector<Treearr*> arr;
	vector<AnsLeaf*> knn(Object* que, int metric, int k);
	vector<AnsLeaf*> rnn(Object* que, int metric, float r,int fgg);
	//float dis;
	//float maxdis;
	SF();
	~SF();
	void readfrom(string s);
	void writeto(string s);
};



#endif