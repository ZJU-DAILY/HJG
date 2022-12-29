#ifndef __bptree
#define __bptree

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
#include "iopack/RAF.h"
#include "iopack\blk_file.h"
#include "object.h"
#include "bpnode.h"
using namespace std;
extern int f;
extern float eps;
class bptree //: public Cacheable
{
public:
	/*
	
	

	
	
	
	void delnode();
	void update();
	
	*/
	//bptree* querydis(float d);
	int block, dirty, fa, isleaf, leafnum;
	float dis;
	float tmpdis;	
	bpnode** son;
	void outnode(string str);
	int getsize();
	int write_to_buffer(char* buffer);
	int read_from_buffer(char* buffer);
	void flush();
	void bfs(string str);
	void loadroot(int page);	
	void insert(bpnode* q);
	bptree* prenode(int loc);
	bptree* lastnode();
	void delnode(int loc);
	void update();
	bptree();
	~bptree();

	void entere(bpnode* q);
	void split(int flag,bptree* rt);

};

#endif