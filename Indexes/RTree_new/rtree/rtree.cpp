/*rtree.cpp
this file implements the RTree class*/
#include <string.h>
#include "rtree.h"
#include "entry.h"
#include "rtnode.h"
#include "../blockfile/cache.h"
#include "../blockfile/blk_file.h"

#include "../linlist/linlist.h"
#include "../heap/heap.h"
extern int DataNum;

//------------------------------------------------------------
RTree::RTree(char *fname, int _b_length, Cache *c, int _dimension)
//use this constructor to build a new tree
{
	

	cout << "Rtree " << Index_f->file->blocklength<< endl;
	


	re_data_cands = new LinList();
	deletelist = new LinList();

	dimension = _dimension;
	root = 0;
	root_ptr = NULL;
	root_is_data = TRUE;
	num_of_data = num_of_inodes = num_of_dnodes = 0;

	root_ptr = new RTNode(this);
	//note that when a tree is constructed, the root is automatically created
	//though at this time there is no entry in the root yet.
	num_of_dnodes++;
	root_ptr->level = 0;
	root = root_ptr->block;
}
//------------------------------------------------------------
RTree::RTree(char *fname, Cache *c)
//use this constructor to restore a tree from a file
{
	int j;

	re_data_cands = new LinList();
	deletelist = new LinList();

	char *header = new char[Index_f->file->get_blocklength()];
	Index_f->file->read_header(header);
	read_header(header);
	delete[] header;

	root_ptr = NULL;
}
//------------------------------------------------------------
RTree::RTree(char *inpname, char *fname, int _b_length, Cache *c, int _dimension, map<int, string> *keywords)
// construct new R-tree from a specified input textfile with rectangles
{
	Entry *d;
	FILE *fp;


	re_data_cands = new LinList();
	deletelist = new LinList();

	char *header = new char[Index_f->file->get_blocklength()];
	read_header(header);
	delete[] header;

	dimension = _dimension;
	root = 0;
	root_ptr = NULL;
	root_is_data = TRUE;
	num_of_data = num_of_inodes = num_of_dnodes = 0;

	root_ptr = new RTNode(this);
	num_of_dnodes++;
	root_ptr->level = 0;
	root = root_ptr->block;

	int record_count = 0;

	if ((fp = fopen(inpname, "r")) == NULL)
	{
		delete this;
		error("Cannot open R-Tree text file", TRUE);
	}
	else
	{
		int id = 0;
		double x0, y0, x1, y1;
		char s[20];

		while (!feof(fp))
		{
			record_count++;
			//id ++; //disable this variable if the id comes with the dataset

			/*	if (record_count%100 == 0)
			{
			for (int i = 0; i < 79; i ++)  //clear a line
			printf("\b");
			printf("inserting object %d", record_count);
			}*/

			//fscanf(fp, "%d %f %f %f %f %s\n", &id, &x0, &x1, &y0, &y1, s);
			//  	d -> bounces[0] = x0;
			//  	d -> bounces[1] = x1;
			//  	d -> bounces[2] = y0;
			//  	d -> bounces[3] = y1;
			// 
			//string key="";
			//key.assign(s);
			fscanf(fp, "%d", &id);
			d = new Entry(dimension, NULL);
			d->son = id;
			for (int i = 0; i<dimension; i++)
			{
				fscanf(fp, "%f", &d->bounces[2 * i]);
				d->bounces[2 * i + 1] = d->bounces[2 * i];
			}

			insert(d);
			//	printf("inserting object %d", id);
			//keywords->insert(map<int,string>::value_type(id,key));
			//d will be deleted in insert()
		}
		printf("inserting object %d", record_count);
	}

	fclose(fp);

	printf("\n");
	delete root_ptr;
	root_ptr = NULL;
}
//------------------------------------------------------------

RTree::RTree(char *inpname, char *fname, int _b_length, Cache *c, int _dimension)
// construct new R-tree from a specified input textfile with rectangles
{
	Entry *d;
	FILE *fp;

	fp = fopen(fname, "r");
	if (fp)
	{
		fclose(fp);
		remove(fname);
	}
	

	re_data_cands = new LinList();
	deletelist = new LinList();

	char *header = new char[Index_f->file->get_blocklength()];
	read_header(header);
	delete[] header;

	root = 0;
	root_ptr = NULL;
	root_is_data = TRUE;
	num_of_data = num_of_inodes = num_of_dnodes = 0;
	dimension = _dimension;
	root_ptr = new RTNode(this);
	num_of_dnodes++;
	root_ptr->level = 0;
	root = root_ptr->block;

	int record_count = 0;

	if ((fp = fopen(inpname, "r")) == NULL)
	{
		delete this;
		error("Cannot open R-Tree text file", TRUE);
	}
	else
	{
		while (record_count < DataNum)
		{
			d = new Entry(_dimension, NULL);
			fscanf(fp, "%d ", &d->son);
			d->son = record_count;
			for (int i = 0; i<_dimension; i++)
			{
				fscanf(fp, "%f ", &d->bounces[2 * i]);
				d->bounces[2 * i + 1] = d->bounces[2 * i];
			}

			insert(d);
			record_count++;
		}
		printf("inserting object %d", record_count);
	}

	fclose(fp);

	printf("\n");
	delete root_ptr;
	root_ptr = NULL;
}

RTree::~RTree()
{
	int j;

	char *header = new char[Index_f->file->get_blocklength()];
	write_header(header);
	Index_f->file->set_header(header);
	delete[] header;

	if (root_ptr != NULL)
	{
		delete root_ptr;
		root_ptr = NULL;
	}	

	delete re_data_cands;
	delete deletelist;

	printf("This R-Tree contains %d internal, %d data nodes and %d data\n",
		num_of_inodes, num_of_dnodes, num_of_data);
}

//------------------------------------------------------------
bool RTree::delete_entry(Entry *d)
{
	load_root();

	R_DELETE del_ret;
	del_ret = root_ptr->delete_entry(d);

	if (del_ret == NOTFOUND) return false;
	if (del_ret == ERASED)
		error("RTree::delete_entry--The root has been deleted\n", true);

	if (root_ptr->level > 0 && root_ptr->num_entries == 1)
		//there is only one entry in the root but the root
		//is not leaf.  in this case, the child of the root is exhalted to root
	{
		root = root_ptr->entries[0].son;
		delete root_ptr;
		root_ptr = NULL;
		load_root();
		num_of_inodes--;
	}

	//Now will reinsert the entries
	while (deletelist->get_num() > 0)
	{
		Linkable *e;
		e = deletelist->get_first();
		Entry *new_e = new Entry(dimension, NULL);
		new_e->set_from_Linkable(e);
		deletelist->erase();
		insert(new_e);
	}

	delete root_ptr;
	root_ptr = NULL;

	return true;
}
//------------------------------------------------------------
bool RTree::FindLeaf(Entry *e)
{
	load_root();
	return root_ptr->FindLeaf(e);
}
//------------------------------------------------------------
void RTree::insert(Entry* d)
{
	int i, j;
	RTNode *sn;
	RTNode *nroot_ptr;
	int nroot;
	Entry *de;
	R_OVERFLOW split_root;
	Entry *dc;
	double *nmbr;

	// load root into memory
	load_root();

	// no overflow occured until now
	re_level = new bool[root_ptr->level + 1];
	for (i = 0; i <= root_ptr->level; i++)
		re_level[i] = FALSE;

	// insert d into re_data_cands as the first entry to insert
	// make a copy of d because it should be erased later
	Linkable *new_link;
	new_link = d->gen_Linkable();
	re_data_cands->insert(new_link);

	delete d;  //we follow the convention that the entry will be deleted when insertion finishes

	j = -1;
	while (re_data_cands->get_num() > 0)
	{
		// first try to insert data, then directory entries
		Linkable *d_cand;
		d_cand = re_data_cands->get_first();
		if (d_cand != NULL)
		{
			// since "erase" deletes the data itself from the
			// list, we should make a copy of the data before
			// erasing it
			dc = new Entry(dimension, NULL);
			dc->set_from_Linkable(d_cand);
			re_data_cands->erase();

			// start recursive insert with root
			split_root = root_ptr->insert(dc, &sn);
		}
		else
			error("RTree::insert: inconsistent list re_data_cands", TRUE);

		if (split_root == SPLIT)
			// insert has lead to split --> new root-page with two sons (i.e. root and sn)
		{
			nroot_ptr = new RTNode(this);
			nroot_ptr->level = root_ptr->level + 1;
			num_of_inodes++;
			nroot = nroot_ptr->block;

			de = new Entry(dimension, this);
			nmbr = root_ptr->get_mbr();
			memcpy(de->bounces, nmbr, 2 * dimension * sizeof(double));
			delete[] nmbr;
			de->son = root_ptr->block;
			de->son_ptr = root_ptr;
			nroot_ptr->enter(de);

			de = new Entry(dimension, this);
			nmbr = sn->get_mbr();
			memcpy(de->bounces, nmbr, 2 * dimension * sizeof(double));
			delete[] nmbr;
			de->son = sn->block;
			de->son_ptr = sn;
			nroot_ptr->enter(de);

			root = nroot;
			root_ptr = nroot_ptr;

			root_is_data = FALSE;
		}
		j++;
	}

	num_of_data++;

	delete[] re_level;

	delete root_ptr;
	root_ptr = NULL;
}
//------------------------------------------------------------
void RTree::load_root()
{
	if (root_ptr == NULL)
		root_ptr = new RTNode(this, root);
}
//------------------------------------------------------------
void RTree::NNQuery(double *QueryPoint,
	SortedLinList *res, string query, map<int, string> * keywords, int k)
{
	double nearest_distanz;

	// load root node into main memory
	load_root();

	nearest_distanz = MAXREAL;

	root_ptr->NNSearch(QueryPoint, res, &nearest_distanz, query, keywords, k);

	delete root_ptr;
	root_ptr = NULL;
}
//------------------------------------------------------------
void RTree::rangeQuery(double *mbr, SortedLinList *res)
{
	load_root();

	root_ptr->rangeQuery(mbr, res);

	delete root_ptr;
	root_ptr = NULL;
}
//------------------------------------------------------------
void RTree::read_header(char *buffer)
{
	int i;

	memcpy(&dimension, buffer, sizeof(dimension));
	i = sizeof(dimension);

	memcpy(&num_of_data, &buffer[i], sizeof(num_of_data));
	i += sizeof(num_of_data);

	memcpy(&num_of_dnodes, &buffer[i], sizeof(num_of_dnodes));
	i += sizeof(num_of_dnodes);

	memcpy(&num_of_inodes, &buffer[i], sizeof(num_of_inodes));
	i += sizeof(num_of_inodes);

	memcpy(&root_is_data, &buffer[i], sizeof(root_is_data));
	i += sizeof(root_is_data);

	memcpy(&root, &buffer[i], sizeof(root));
	i += sizeof(root);
}
//------------------------------------------------------------
void RTree::write_header(char *buffer)
{
	int i;

	memcpy(buffer, &dimension, sizeof(dimension));
	i = sizeof(dimension);

	memcpy(&buffer[i], &num_of_data, sizeof(num_of_data));
	i += sizeof(num_of_data);

	memcpy(&buffer[i], &num_of_dnodes, sizeof(num_of_dnodes));
	i += sizeof(num_of_dnodes);

	memcpy(&buffer[i], &num_of_inodes, sizeof(num_of_inodes));
	i += sizeof(num_of_inodes);

	memcpy(&buffer[i], &root_is_data, sizeof(root_is_data));
	i += sizeof(root_is_data);

	memcpy(&buffer[i], &root, sizeof(root));
	i += sizeof(root);

}
