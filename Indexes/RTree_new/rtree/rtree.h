/* rtree.h
   this file defines the class RTree*/

#ifndef __RTREE
#define __RTREE
//------------------------------------------------------------
#include "../func/gendef.h"
#include "../RAF.h"
#include <string>
#include<map>
#include <iostream>

using namespace std;
extern RAF* Index_f;
//------------------------------------------------------------
class LinList;
class SortedLinList;
class Cache;
class RTNode;
class Entry;
//------------------------------------------------------------
class RTree //: public Cacheable
{
public:
//--===on disk===--
	int dimension;                       
	int num_of_data;	                 
    int num_of_dnodes;	                 
    int num_of_inodes;	                 
	int root;                            
	bool root_is_data;                   
//--===others===--
	RTNode *root_ptr;
    bool *re_level;  
    LinList *re_data_cands; 
	LinList *deletelist;
//--===functions===--
	RTree(char *fname, int _b_length, Cache* c, int _dimension);
    RTree(char *fname, Cache* c);
    RTree(char *inpname, char *fname, int _blength, Cache* c, int _dimension,map<int,string> * keywords);
	RTree(char *inpname, char *fname, int _blength, Cache* c, int _dimension);
    ~RTree();

	bool delete_entry(Entry *d);
	bool FindLeaf(Entry *e);
    int get_num() { return num_of_data; }
	void insert(Entry *d);
	void load_root();  
	void NNQuery(double *QueryPoint, SortedLinList *res,string query,map<int,string> * keywords,int k);
	void rangeQuery(double *mbr, SortedLinList *res);
	void read_header(char *buffer);      
	void write_header(char *buffer);    
};

#endif // __RTREE
