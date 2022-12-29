#pragma once
#ifndef __bpnode
#define __bpnode

#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include "object.h"
using namespace std;
extern RAF* Obj_f;
extern int* metricm;
class bpnode
{
public:	
	int metric;
	int objloc;
	int len;
	int son;
	char* datas;
	float* datad;
	float disl,disr;
	float dis;
	void outnode(string str);
	int getsize();
	int write_to_buffer(char* buffer);
	int read_from_buffer(char* buffer);
	bpnode();
	void copy(bpnode* m);
	float distance(bpnode* m);
	bpnode(int m, Object* o);
	~bpnode();
};

#endif