#ifndef __RTNODE
#define __RTNODE
//------------------------------------------------------------
#include "../func/gendef.h"
#include <string>
#include <map>
using namespace std;

//------------------------------------------------------------
class SortedLinList;
class Entry;
class RTree;
//------------------------------------------------------------
class RTNode
{
public:
//--===on disk===--
	int level; 
	int block;
	int num_entries;
	Entry *entries;
//--===others===--
	bool dirty;
	int capacity;
    int dimension;
	RTree *my_tree;  
//--===functions===--
	RTNode(RTree *rt);
    RTNode(RTree *rt, int _block);
    ~RTNode();

    int choose_subtree(double *brm);
	R_DELETE delete_entry(Entry *e); 
	void enter(Entry *de);
	bool FindLeaf(Entry *e);
	double *get_mbr();
	int get_num_of_data();
	R_OVERFLOW insert(Entry *d, RTNode **sn);
	bool is_data_node() { return (level==0) ? TRUE : FALSE ;};
	void NNSearch(double *QueryPoint, SortedLinList *res,
				      double *nearest_distanz, string query,map<int,string> *keywords,int k);
	void NNSearch(double *QueryPoint, SortedLinList *res, double *nearest_distanz, int k);
	void print();
	void rangeQuery(double *mbr, SortedLinList *res);
    void read_from_buffer(char *buffer);
	int split(double **mbr, int **distribution);
	void split(RTNode *sn);
    void write_to_buffer(char *buffer); 
};

#endif