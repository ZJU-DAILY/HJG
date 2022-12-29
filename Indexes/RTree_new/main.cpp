//***************************************************
//This is the implementation of R*-tree v0.1

//Last revised July 4.
//***************************************************

#include <math.h>
#include <time.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include "./rtree/rtree.h"
#include "./rtree/rtnode.h"
#include "./rtree/entry.h"
#include "./blockfile/blk_file.h"
#include "./blockfile/cache.h"
#include "./linlist/linlist.h"
#include <time.h>
#include "object.h"
#include <fstream>
#include <iostream>
#include "RAF.h"
#include <stack>
#include <vector>
#include<queue>
#include "heap\heap.h"
#include "basemetric.h"
using namespace std;

int dimension;
int DataNum;
int func;
int pn;
double IOread;
double IOwrite;
double compdists;
int BLOCK_SIZE;
double* querym=new double[100];
int* metricm=new int[100];
double* metricmaxdis=new double[100];
int m;

Object * ptable;
Object** pivott;
Object * cand;
RAF* draf;
RAF* Index_f;
RTree *rt;
Cache * c;
Object** os;

Object ** readobjects(char* filename)
{
	ifstream in1;
	m = 10000;
	string str;
	in1.open(filename);
	in1 >> DataNum >> m;
	for (int i = 0; i < m; i++) in1 >> metricm[i];
	for (int i = 0; i < m; i++) in1 >> metricmaxdis[i];
	


	Object** objset =new Object * [DataNum];;
	cout <<" Get Objects " << DataNum << " with Dimension " << m << endl;
	getline(in1, str);
	int maxsize = 0;
	for (int i = 0; i < DataNum; i++) {
		getline(in1, str);
		//cout << "trying " << str << endl;
		objset[i] = new Object();
		//		cout << "new finish " << str << endl;
		objset[i]->ini(str);
		objset[i]->id = i;
		maxsize = max(maxsize, objset[i]->getsize());
		//arr[i]->outnode();		
	}
	cout << "Max size is " << maxsize << endl;	
	return objset;
}
Object* getobject(int ptr)
{
	int i = ptr / draf->file->blocklength;
	char* buffer = new char[draf->file->blocklength];

	if (c == NULL)
	{
		draf->file->read_block(buffer, i);
	}
	else
	{
		c->read_block(buffer, i, draf);
	}

	int j = ptr - i * draf->file->blocklength;
	Object* o = new Object();
	o->read_from_buffer(&buffer[j]);

	delete[] buffer;
	//printf("%d %d %f\n",o->id, o->size, o->data[0]);

	return o;
}
clock_t bulidIndex(char* index_name, char * filename, int pn)
{
	/*******
	index_name -- the path of the index to be stored
	filename -- the path of the dataset
	n_p -- the number of pivots
	*******/

	//pivot selection    
	
	os = readobjects(filename); // the function needed to be rewirte to read differnt formats of the dataset
	
	int* obj_order = new int[DataNum];
	for (int i = 0; i < DataNum; i++) obj_order[i] = i;
	draf = new RAF();
	draf->num_obj = DataNum;
	draf->init(index_name, 8192, NULL);
	char* buffer = new char[draf->file->blocklength];
	draf->file->append_block(buffer);
	for (int i = 0; i < DataNum; i++) { obj_order[i] = draf->add_object(os[i]); }
	Index_f = new RAF();	
	Index_f->init("SFIndex-Block", BLOCK_SIZE,NULL);
	cout << "BLOCK_SIZE "<<BLOCK_SIZE << endl;
	cout << "BLOCK_SIZE " << Index_f->file->get_blocklength() << endl;
	char* buf = new char[Index_f->file->get_blocklength()];
	Index_f->file->append_block(buf);

	//int* result = draf->buid_from_array(os, obj_order);
	pivott = new Object * [pn];
	int* pparr = new int[m];
	for (int iii = 0; iii < pn;iii++) {
		pivott[iii] = new Object();
		pivott[iii]->size = new int[m * 2];
		pivott[iii]->sized = 0;
		pivott[iii]->sizes = 0;
				
		for (int i = 0; i < m; i++) {
			int p = rand()%DataNum;
			pparr[i] = p;
			switch (metricm[i]) {
			case 0:
			case 3:
			case 1:
			case 2:
			case 4:
			case 5:
				pivott[iii]->sized += os[pparr[i]]->size[i * 2 + 1] - os[pparr[i]]->size[i * 2];
				break;
			case 6:
				pivott[iii]->sizes += os[pparr[i]]->size[i * 2 + 1] - os[pparr[i]]->size[i * 2];
				break;
			default:
				cout << "Object reading data error!!!! " << endl;
			}
		}
		pivott[iii]->datad = new double[pivott[iii]->sized];
		pivott[iii]->datas = new char[pivott[iii]->sizes];
		pivott[iii]->sized = 0;
		pivott[iii]->sizes = 0;
		for (int j = 0; j < m; j++) {
			switch (metricm[j]) {
			case 0:
			case 3:
			case 1:
			case 2:
			case 4:
			case 5:
			{
				pivott[iii]->size[j * 2] = pivott[iii]->sized;
				pivott[iii]->sized += os[pparr[j]]->size[j * 2 + 1] - os[pparr[j]]->size[j * 2];
				pivott[iii]->size[j * 2 + 1] = pivott[iii]->sized;
				for (int i = pivott[iii]->size[j * 2]; i < pivott[iii]->size[j * 2 + 1]; i++) {
					pivott[iii]->datad[i] = os[pparr[j]]->datad[i];
				}
				break;
			}
			case 6:
				pivott[iii]->size[j * 2] = pivott[iii]->sizes;
				pivott[iii]->sizes += os[pparr[j]]->size[j * 2 + 1] - os[pparr[j]]->size[j * 2];
				pivott[iii]->size[j * 2 + 1] = pivott[iii]->sizes;
				for (int i = pivott[iii]->size[j * 2]; i < pivott[iii]->size[j * 2 + 1]; i++) {
					pivott[iii]->datas[i] = os[pparr[j]]->datas[i];
				}
				break;
			default:
				cout << "Object reading data error!!!! " << endl;
			}
		}
		//cout << "Pivot is ";
		//pivott[iii]->outnode();
	}
	
	delete[] pparr;

	IOread = IOwrite = compdists = 0;
	

	clock_t begin;
	begin = clock();
	char bl[] = "bulkload.txt";
	FILE* f = fopen(bl, "w+");
	if (f != NULL)
	{
		for (int i = 0; i < DataNum; i++)
		{
			fprintf(f, "%d ", os[i]->id);
			for (int j = 0; j < pn; j++)
			{
				for (int jj = 0; jj < m; jj++)
				fprintf(f, "%f ", os[i]->distance(pivott[j],jj));
			}
			fprintf(f, "\n");
		}
	}
	fclose(f);
	

	//c = new Cache(0, BLOCK_SIZE);
	//rt = new RTree("bulkload.txt", index_name, BLOCK_SIZE,NULL, pn*m);
	rt = new RTree(index_name, BLOCK_SIZE,NULL, pn*m);
	cout << " initial rtree block " << Index_f->file->get_blocklength() << endl;
	for (int ii = 0; ii < DataNum; ii++) {

		Entry* d = new Entry(pn * m, NULL);
		d->son = -1;
		for (int i = 0; i < pn; i++)

			for (int j = 0; j < m; j++) {
				d->bounces[2 * (i * m + j)] = os[ii]->distance(pivott[i], j);
				d->bounces[2 * (i * m + j) + 1] = d->bounces[2 * (i * m + j)];
			}
		d->ptr = obj_order[ii];
		rt->load_root();
		rt->insert(d);
		delete rt->root_ptr;
		rt->root_ptr = NULL;
	}
	cout << " end rtree block " << Index_f->file->get_blocklength() << endl;
	/*

	for (int i = 0; i < DataNum; i++)
	{



		fprintf(f, "%d ", os[i]->id);
		for (int j = 0; j < pn; j++)
		{
			for (int jj = 0; jj < m; jj++)
				fprintf(f, "%f ", os[i]->distance(pivott[j], jj));
		}
		fprintf(f, "\n");
	}
	stack<RTNode*> s;
	////build RAF
	rt->load_root();
	s.push(rt->root_ptr);
	RTNode * node;
	
	int k = 0;
	while (!s.empty())
	{
		node = s.top();
		s.pop();
		if (node->level != 0)
		{
			for (int i = node->num_entries - 1; i >= 0; i--)
			{
				s.push(node->entries[i].get_son());
				node->entries[i].son_ptr = NULL;
			}
		}
		else
		{
			for (int i = 0; i < node->num_entries; i++)
			{
				obj_order[k] = node->entries[i].son;
				//cout << node->entries[i].son << endl;
				k++;
			}
		}
		delete node;
	}

	rt->root_ptr = NULL;

	draf = new RAF();
	draf->num_obj = DataNum;
	draf->init(index_name, BLOCK_SIZE, NULL);

	Object ** objS = readobjects(filename);
	

	//delete object sets
	for (int i = 0; i < DataNum; i++)
		delete objS[i];
	delete[] obj_order;

	rt->load_root();
	s.push(rt->root_ptr);
	k = 0;
	while (!s.empty())
	{
		node = s.top();
		s.pop();
		if (node->level != 0)
		{
			for (int i = node->num_entries - 1; i >= 0; i--)
			{
				s.push(node->entries[i].get_son());
				node->entries[i].son_ptr = NULL;
			}
		}
		else
		{
			for (int i = 0; i < node->num_entries; i++)
			{
				node->entries[i].ptr = result[k];
				//cout << node->entries[i].son << " " << result[k] << endl;
				k++;
			}
			char * buffer = new char[BLOCK_SIZE];
			node->write_to_buffer(buffer);
			rt->file->write_block(buffer, node->block);
			delete[] buffer;
		}
		delete node;
	}
	rt->root_ptr = NULL;
	*/
	printf("%f %f %f\n",IOread,IOwrite, compdists);
	//delete[] result;
	return begin;

	
}

double distl(double* point, double* bounces, int n_p)
{
	double d = 0;
	for (int j = 0; j < m; j++) {
		double d2 = 0;
		for (int i = 0; i < n_p; i++) {
			double d1 = 0;
			if ((bounces[2 * (j+i*m)] - point[i*m+j]) * (bounces[2 * (j + i * m) + 1] - point[i * m + j]) > 0) 
				d1 = min(abs(bounces[2 * (j + i * m)] - point[i * m + j]), abs(bounces[2 * (j + i * m) + 1] - point[i * m + j]));
			/*
			if (querym[j]) {
				cout << "dimension " << j << " querym " << querym[j] << " dis " << d1 << endl;
				cout << "d(q,p) " << point[i * m + j] << endl;
				cout << "d(o,p) " << bounces[2 * j + 1]  << endl;

			}
			*/
			d2 = max(d2, d1);
		}
		d += d2* querym[j];
	}
	return d;
}

int trav()
{
	
	rt->load_root();
	vector<NEntry> v;
	NEntry n;
	RTNode* node;
	n.node = rt->root_ptr;
	n.flag = false;
	v.push_back(n);
	double d = 0;
	int qs = 0;
	while (!v.empty())
	{
		qs++;
		n = v.back();
		node = n.node;
		v.pop_back();
		if (node->level != 0)
		{
			cout << "containinng " << node->num_entries << endl;
			for (int i = 0; i < node->num_entries; i++) {
					NEntry ne;
					ne.node = node->entries[i].get_son();
					for (int k = 0; k < 15 - node->level; k++) cout << " ";
					for (int k = 0; k < node->dimension*2; k++) cout << node->entries[i].bounces [k]<< " ";
					cout << endl;
					ne.flag = true;
					v.push_back(ne);
					node->entries[i].son_ptr = NULL;
			}
		}
		delete node;
	}	
	rt->root_ptr = NULL;
	cout << "Total" << qs << " nodes" << endl;
	
	return qs;
}

vector<Object*> rangequery(Object* q, double radius)
{
	vector<Object*> result;
	
	double* point = new double[pn*m];
	for (int i = 0; i < pn*m; i++) point[i] = q->distance(pivott[i/m],i%m);
	rt->load_root();
	vector<NEntry> v;
	NEntry n;
	RTNode* node;
	n.node = rt->root_ptr;
	n.flag = false;
	v.push_back(n);
	double d=0;
	int qs = 0;
	while (!v.empty())
	{
		qs++;
		n = v.back();
		node = n.node;
		v.pop_back();
		if (node->level != 0)
		{
			for (int i = 0; i < node->num_entries; i++)	{
				d = distl(point,node->entries[i].bounces, pn);
				//d = 0;
				if (d<=radius+0.00000001){
					//cout << "=========this is ok" << endl;
					NEntry ne;
					ne.node = node->entries[i].get_son();
					ne.flag = true;
					v.push_back(ne);
					node->entries[i].son_ptr = NULL;
				}
				else {
					//cout << "===========fail" << endl;
				}
			}
		}
		else
		{
			for (int ii = 0; ii < node->num_entries; ii++){
				d = distl(point, node->entries[ii].bounces, pn);
				//d = 0;
				if (d <= radius+0.0000001) {
					Object* o = getobject(node->entries[ii].ptr);
					d = 0;
					for (int i = 0; i < m; i++) if (querym[i] > 0.000001) d+=q->distance(o, i)*querym[i];
					if (d - radius < 0.00000001)
					{
						NEntry ne;
						ne.node = node;						
						result.push_back(o);
						//cout<<"r= "<<radius<<" pn= "<<pn << " dis= " << d <<" diff "<<d-radius<<" bounddis= "<< distl(point, node->entries[ii].bounces, pn)<< " object : "; o->outnode();
						//cout <<"query= " ; q->outnode();
						/*
						for (int j = 0; j < pn; j++)

						{
							for (int jj = 0; jj < m; jj++)
								if (querym[jj]) {
									cout << " pivot " << j << " dis o" << o->distance(pivott[j], jj)<<endl;
									cout << " pivot " << j << " dis q" << q->distance(pivott[j], jj)<<endl;
								}
						}
						*/
					}
					else delete o;					
				}
			}
		}
		delete node;
	}
	rt->root_ptr = NULL;
//	cout << " visited " << qs << " nodes" << endl;
	delete[] point;
	return result;
}
bool Obj_Comp(const Object* a, const Object* b) {
	if (a->dist < b->dist)    return true;
	return false;
}
vector<Object*> kNNquery(Object* q, int k)
{
	vector<Object*> answer;
	double kdist = MAXREAL;

	double* point = new double[pn * m];
	for (int i = 0; i < pn * m; i++) point[i] = q->distance(pivott[i / m], i % m);
	
	rt->load_root();
	priority_queue<KEntry> queue;
	priority_queue<double> distque;
	KEntry ke;
	ke.id = rt->root;
	ke.isobject = false;
	ke.key = 0;
	queue.push(ke);
	RTNode* node;
	int ptr;
	while (!queue.empty())
	{
		ke = queue.top();
		queue.pop();
		if (ke.key > kdist)
			break;
		if (ke.isobject)
		{
			Object* o = getobject(ke.id);
			double temp=0;			
			for (int i = 0; i < m; i++)if (querym[i] > 0.000001) temp += q->distance(o, i)*querym[i];
			if (temp <= kdist)
			{
				o->dist = temp;
				answer.push_back(o);
				distque.push(temp);				
				sort(answer.begin(), answer.end(),Obj_Comp);
				int num = answer.size();				
				if (distque.size() >= k) {
					while (distque.size() > k) distque.pop();
					kdist = distque.top();
					while (kdist < answer[num - 1]->dist) {
						num--;
						answer.pop_back();
					}
				}
				
			}
			else 
				delete o;
		}
		else
		{
			double temp;
			node = new RTNode(rt, ke.id);
			if (node->level != 0)
			{
				for (int i = 0; i < node->num_entries; i++)
				{
					temp = distl(point, node->entries[i].bounces, pn);						
					if (temp <= kdist)
					{
						KEntry e;
						e.id = node->entries[i].son;
						e.key = temp;
						e.isobject = false;
						queue.push(e);
					}
				}
			}
			else
			{
				for (int i = 0; i < node->num_entries; i++)
				{
					temp = distl(point, node->entries[i].bounces, pn);
					if (temp <= kdist)
					{
						KEntry e;
						e.id = node->entries[i].ptr;
						e.key = temp;
						e.isobject = true;
						queue.push(e);
					}
				}
			}
			delete node;
		}
	}
	while (!distque.empty()) distque.pop();
	//cout << "farthest " << kdist << endl;
	delete[] point;
	return answer;
}

clock_t updateIndex(char* index_name, char* filename, int n_p)
{
	clock_t begin;
	begin = clock();
	IOread = IOwrite = compdists = 0;
	for (int ii = DataNum - DataNum / 10; ii < DataNum; ii++) {
		
		rt->load_root();
		vector<Object*> tmpres;
		tmpres.clear();
		//cout  << endl << endl;
		//cout << "query is " ;
		//q->outnode();
		//cout << "*********";
		tmpres = rangequery(os[ii], 0.00000000001);
		//cout << "=========================== result rnn=" << radius[i] << endl; 
		for (int jj = 0; jj < tmpres.size(); jj++) {
			Object* n = tmpres[jj];
			delete n;
			tmpres[jj] = NULL;
		}
		tmpres.clear();
		Entry* d = new Entry(pn*m, NULL);
		d->son = -1;
		for (int i = 0; i < n_p; i++)
		
			for (int j = 0; j < m; j++){
			d->bounces[2 * (i*m+j)] = os[ii]->distance(pivott[i], j);
			d->bounces[2 * (i * m + j) + 1] = d->bounces[2 * (i * m + j)];
		}

		rt->delete_entry(d);
		delete rt->root_ptr;
		rt->root_ptr = NULL;
	}
	cout << "=========== Delete over" << endl;
	for (int ii = DataNum - DataNum / 10; ii < DataNum; ii++) {

		Entry* d = new Entry(pn * m, NULL);
		d->son = -1;
		for (int i = 0; i < n_p; i++)

			for (int j = 0; j < m; j++) {
				d->bounces[2 * (i * m + j)] = os[ii]->distance(pivott[i], j);
				d->bounces[2 * (i * m + j) + 1] = d->bounces[2 * (i * m + j)];
			}
		rt->load_root();
		rt->insert(d);
		delete rt->root_ptr;
		rt->root_ptr = NULL;
	}

	begin = clock() - begin;

	return begin;


}


void main(int argc, char** argv)
{
	clock_t begin, buildEnd, queryEnd;
	double buildComp, queryComp;
	ifstream in1;
	in1.open("data/par.txt");
	BLOCK_SIZE =8192;

	char * datafile = new char[100];
	char * indexfile = "RR-tree-index";
	FILE * f = fopen("ans-rrt-8k.txt", "w");
	char* querydata = new char[100];
	pn = 1;
	
	in1 >> datafile >> querydata;

	ifstream in;
	in1.close();

	begin = bulidIndex(indexfile, datafile, pn);
	//trav();
	buildEnd = clock() - begin;
	buildComp = compdists;
	

	
	Object* q = new Object();
	double io = 0;
	double dists = 0;
	double pf = 0;
	double rad;

	draf->cache = NULL;
	
	//return;
	// ============================= update===================
	if(1==0){
		fprintf(f, "building...\n");
		fprintf(f, "finished... %f build time\n", (double)buildEnd / CLOCKS_PER_SEC);
		fprintf(f, "finished... %f distances computed\n", buildComp);
		fprintf(f, "finished... %f IO times\n", IOread + IOwrite);
	double nsamp = DataNum / 10;
	buildEnd = updateIndex(indexfile, datafile, pn);
	buildComp = compdists;
	fprintf(f, "building...\n");
	fprintf(f, "finished... %f build time\n", (double)buildEnd / CLOCKS_PER_SEC / nsamp);
	fprintf(f, "finished... %f distances computed\n", buildComp / nsamp);
	fprintf(f, "finished... %f IO times\n", (IOread + IOwrite) / nsamp);
	for (int i = 0; i < DataNum; i++)
		delete os[i];
	delete[] os;
	return;
	}
	// ============================= update end===================

	//=========== multi querytest ==========
	{
		in1.open(querydata);
		int quepos[200];
		int qcount;
		double quem = 0;
		for (int i = 0; i < m; i++) {
			in1 >> querym[i];
			quem += querym[i];
		}
		int quemk, quemr;
		double* radius;
		int* kvalues;

		in1 >> quemk;
		kvalues = new int[quemk];
		for (int i = 0; i < quemk; i++) in1 >> kvalues[i];
		in1 >> quemr;
		float notd;
		for (int i = 0; i < quemr; i++) in1 >> notd;
		in1 >> qcount;
		for (int i = 0; i < qcount; i++) {
			in1 >> quepos[i];
			quepos[i] %= DataNum;
		}
		in1.close();

		//qcount = 5;
		for (int i = 0; i < quemk; i++) {
			
			dists = 0;
			pf = 0;
			rad = 0;
			IOread = 0;
			double time = 0;
			for (int j = 0; j < qcount; j++)
			{
				compdists = 0;
				q = os[quepos[j]];

				vector<Object*> tmpres;
				tmpres.clear();
				//cout  << endl << endl;
				//cout << "query is " ;
				//q->outnode();
				//cout << "*********";
				begin = clock();				
				tmpres= kNNquery(q, kvalues[i]);
				time += (clock() - begin)*1.0 / CLOCKS_PER_SEC;
				
				//cout << "=========================== result knn=" << kvalues[i] << endl; 
				float max = 0;
				int num = tmpres.size();
				fprintf(f, "k\t%d", num);
				for (int jj = 0; jj < tmpres.size(); jj++) {
					fprintf(f, "\t%d\t\%f", tmpres[jj]->id, tmpres[jj]->dist);
					Object* n = tmpres[jj];					
					//double d = 0;
					//for (int i = 0; i < m; i++) d += q->distance(n, i) * querym[i];
				//	cout << "dis " << d<<" ; ";
					//n->outnode();
					delete n;
					tmpres[jj] = NULL;
				}
				fprintf(f, "\n");
				tmpres.clear();
				dists += compdists;
			}
			
			queryComp = dists;
			fprintf(f, "t\t%f\n", (double)time  / qcount);						
			fflush(f);
		}
		
	}
	for (int i = 0; i < DataNum; i++)
		delete os[i];
	delete[] os;
	//system("pause");
	//=========== multi querytest end ==========
}


