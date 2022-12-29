/* entry.cpp
   implementation of class Entry */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "entry.h"
#include "rtnode.h"
#include "../linlist/linlist.h"
//------------------------------------------------------------
Entry::Entry()
  //this ructor does nothing.  remember you should call init_entry
  //to initialize if you use this ructor
{
	son_ptr = NULL;
	bounces = NULL;
}
//------------------------------------------------------------
Entry::Entry(int _dimension, RTree *rt)
{
    dimension = _dimension;
    my_tree = rt;
    bounces = new double[2*dimension];
    son_ptr = NULL;
    son = 0;
	level = 0;
	ptr = 0;
}
//------------------------------------------------------------
Entry::~Entry()
{
    if (bounces)
		delete [] bounces;
    if (son_ptr != NULL)
	    delete son_ptr;
}
//------------------------------------------------------------
void Entry::del_son()
{
	if (son_ptr != NULL)
	{
		delete son_ptr;
		son_ptr = NULL;
	}
}	
//------------------------------------------------------------
Linkable* Entry::gen_Linkable()
{
	Linkable *new_link = new Linkable(dimension);
	new_link -> son = son;
	//memcpy(new_link -> bounces, bounces, 2 * dimension * sizeof(double));
	for (int i = 0; i < 2 * dimension; i ++)
		new_link -> bounces[i] = bounces[i];
	new_link -> level = level;
	new_link->ptr = ptr;
	return new_link;
}
//------------------------------------------------------------
int Entry::get_size()
{
    return 2 * dimension * sizeof(double) + sizeof(int) + sizeof(int);
	  //for bounces and son and ptr
}
//------------------------------------------------------------
RTNode* Entry::get_son()
{
    if (son_ptr == NULL)
	    son_ptr = new RTNode(my_tree, son);

    return son_ptr;
}
//------------------------------------------------------------
void Entry::init_entry(int _dimension, RTree *_rt)
{
	dimension = _dimension;
    my_tree = _rt;
    bounces = new double[2 * dimension];
    son_ptr = NULL;
    son = 0;
	level = 0;
	ptr =0;
}
//------------------------------------------------------------
void Entry::read_from_buffer(char *buffer)
{
    int i;

    i = 2 * dimension * sizeof(double);
    memcpy(bounces, buffer, i);

    memcpy(&son, &buffer[i], sizeof(int));
    i += sizeof(int);

	memcpy(&ptr, &buffer[i], sizeof(int));
    i += sizeof(int);
}
//------------------------------------------------------------
SECTION Entry::section(double *mbr)
{
    bool inside;
    bool overlap;

    overlap = TRUE;
    inside = TRUE;

    for (int i = 0; i < dimension; i++)
    {
		if (mbr[2 * i] > bounces[2 * i + 1] ||  mbr[2 * i + 1] < bounces[2 * i])
			overlap = FALSE;
		if (mbr[2 * i] < bounces[2 * i] ||
			mbr[2 * i + 1] > bounces[2 * i + 1])
			inside = FALSE;
    }
    if (inside)
		return INSIDE;
    else if (overlap)
		return OVERLAP;
    else
		return S_NONE;
}
//------------------------------------------------------------
bool Entry::section_circle(double *center, double radius)
{
	double r2;

	r2 = radius * radius;

	if ((r2 - MINDIST(center,bounces)) < doubleZERO)
		return TRUE;
	else
		return FALSE;
}
//------------------------------------------------------------
void Entry::set_from_Linkable(Linkable *link)
{
	son = link -> son;
	dimension = link -> dimension;
	memcpy(bounces, link -> bounces, 2 * dimension * sizeof(double));
	level = link -> level;
	ptr = link->ptr;
	my_tree = NULL;
	son_ptr = NULL;
}
//------------------------------------------------------------
void Entry::write_to_buffer(char *buffer)
{
    int i;

    i = 2 * dimension * sizeof(double);
    memcpy(buffer, bounces, i);

    memcpy(&buffer[i], &son, sizeof(int));
    i += sizeof(int);

	memcpy(&buffer[i], &ptr, sizeof(int));
    i += sizeof(int);
	//cout << "size " << i << endl;
	
}
//------------------------------------------------------------
bool Entry::operator == (Entry &_d)
  //this function compares two entries based on (1)son (2)dimension (3)extents
{
	if (son != _d.son) return false;
	if (dimension != _d.dimension) return false;
	if( ptr!= _d.ptr) return false;
	for (int i = 0; i < 2 * dimension; i++)
		if (fabs(bounces[i] - _d.bounces[i]) > doubleZERO) return false;
	return true;
}
//------------------------------------------------------------
Entry& Entry::operator = (Entry &_d)
  //this function assigns all fieds of _d with the same values of this entry
{
    dimension = _d.dimension;
    son = _d.son;
    son_ptr = _d.son_ptr;
    memcpy(bounces, _d.bounces, sizeof(double) * 2 * dimension);
    my_tree = _d.my_tree;
	level = _d.level;
	ptr = _d.ptr;

    return *this;
}
//------------------------------------------------------------