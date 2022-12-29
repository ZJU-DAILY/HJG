#define _CRT_SECURE_NO_WARNINGS


#include <time.h>
#include <vector>
#include<algorithm>
#include <cmath>
#include <map>
#include<queue>
#include<cstdlib>
#include <iostream>
#include <sstream>
#include "./iopack/blk_file.h"
#include "./iopack/cache.h"
#include "./iopack/gadget.h"
#include "./iopack/RAF.h"
#include "basemetric.h"
#include "object.h"
#include "bptree.h"
#include "sf.h"
using namespace std;
float eps = 0.0000000001;
double compdists;
double IOread = 0;
double IOwrite = 0;
int bolcksize;
RAF* Obj_f;
RAF* Index_f;
Cache* Obj_c;
Cache* Index_c;

vector<SF*> sforest;
vector<Object*> anslist;


int f=0;
int n, m;
int npiv,nsf;
Objectarr* record = new Objectarr();
double* querym;
int* metricm;
float* metricmaxdis;
float* metricmeandis;
int st = 0;
Treearr* findnear(bpnode* q);
void update(int metric);
void delpiv(int metric);
vector<Object*> multirange_q(Object* que, float r);
vector<Object*> multiknn_q(Object* que, int k);
/*

void outvec(SF* p) {
	for (int i = 0; i < p->arr.size(); i++)
		if(p->arr[i]->tree!=NULL) p->arr[i]->tree->bfs("");
}
void delobj(Object *p) {
	for (int i = 0; i < m; i++) {
	//for (int i = 1; i < 2; i++) {
		if (p->loc[i] == NULL) cout << "!! fault " << endl;
		else p->loc[i]->delnode();
		delete p->loc[i];
		p->loc[i] = NULL;
	}
}

*/
void outp(int i) {
	return;
	cout << "a new round bfs beg" << endl;
	for (int k = 0; k < sforest[i]->arr.size(); k++) {
		sforest[i]->arr[k]->outnode();
		//continue;
		
		bptree* p = new bptree();
		p->loadroot(sforest[i]->arr[k]->tree);
		p->bfs(to_string(i) + " " + to_string(k) + " out_p ");
		
		delete p;
	}
	cout << "bfs fini" << endl;
}

int main() {
	string filedata, querydata;
	ifstream in1;
	in1.open("data/par.txt");
	in1 >> filedata >> querydata;

	//freopen("ans.txt", "w", stdout);

//	srand((int)time(NULL));
	srand(0);
	int buffer_size = 32;
	bolcksize = 8192;
	Obj_f = new RAF();
//	Obj_c = new Cache(buffer_size, bolcksize);
	Obj_c = NULL;
	Obj_f->init("SFObj-Block1",8192, NULL);
	Objectarr* record = new Objectarr();
	// cout << "come on 1" << endl;
	record->readfile(filedata);
	//m = 3;
	//st = 3;
	in1.close();
	querym = new double[m];
	for (int i = 0; i < m ; i++) querym[i] = 0;

//	for (int i = m - 2; i < m; i++) querym[i] = 1;
	f = 0;
	int* objloc = new int[n + 1];
//	Obj_c = NULL;
	char* buffer = new char[Obj_f->file->blocklength];
	Obj_f->file->append_block(buffer);
	for (int i = 0; i < n; i++) {	objloc[i] = Obj_f->add_object(record->arr[i]);	}

	Obj_c = new Cache(buffer_size, 8192);
	Obj_c->clear();
	//Obj_c = NULL;
	//Obj_f->init_restore("Obj-Block", Obj_c);
	
	
	//cout << "block end : " << Obj_f->file->get_num_of_blocks() << endl;
	/*for (int i = max(n / 50 - 2, 0); i < n / 40; i++) {
	for (int i = 0; i < 10; i++) {
		Object* q = Obj_f->get_object(objloc[i]);
		q->outnode();
		delete q;
	}
		*/   	 
	
	
	Index_f = new RAF();
	//Index_c = new Cache(buffer_size, bolcksize);
	Index_c = NULL;
	Index_f->init("SFIndex-Block1", bolcksize, Index_c);
	char* buf = new char[Index_f->file->get_blocklength()];
	Index_f->file->append_block(buf);
	npiv = 1;
	for (int i = 0; i < m; i++) {
		SF *tmp = new SF();		
		sforest.push_back(tmp);			
		Object* q = record->arr[0];
		Treearr* tmpp;
		tmpp = new Treearr(q, i);
		//tmpp->outnode();
		bptree* p = new bptree();
		p->write_to_buffer(buffer);
		tmpp->tree = Index_f->file->append_block(buffer);
		sforest[i]->arr.push_back(tmpp);
		delete p;
		//cout << "trying " << sforest[i]->arr[0]->tree->data->data[0] << endl;
	}
	
	//outp(0);
	//return 0;
	//=============  insert N nodes  =====================
	
	//st = 5;
	//n = 6;
	//f = 1;
	//cout << m << " is m" << endl;
	cout << "building beg" << endl;
	for (int j = st; j < m; j++) {
		for (int i = 0; i < n; i++) {
			//cout << m << " is m" << endl;
		//	cout << "doing " <<j<<" "<< i << endl;
			bpnode* q = new bpnode(j, record->arr[i]);
			q->objloc = objloc[i];
			//compdists = 0;
			Treearr* tmp = findnear(q); 
			tmp->tot++;
			//cout <<i<< " dcmp "<<compdists << endl;;
			//q->dis = q->data->distom[j];
			q->disl = q->dis;
			q->disr = q->dis;			

			bptree* p = new bptree();
			//cout << "doing 1 " << j << " " << i << endl;
			//q->outnode("que ");
			p->loadroot(tmp->tree);
			p->insert(q);			
			delete p;
			//cout << "doing 2 " << j << " " << i << endl;
			
			
			int tmpnp = round(pow(i, PIVOTN));
			if (tmpnp > sforest[j]->arr.size()) {
				compdists = 0;
				//outp(j);
				update(j);
				
				//outp(j);
				//cout << "dcmp " << compdists << endl;
			}
			//outp(j);
		}	
		//outp(j);
		//update(j);
	//	outp(j);
		//delpiv(j);
		//outp(j);
		//=============  RNN Query  ==============
		//vector<AnsLeaf*> tmpresr = sforest[j]->rnn(record->arr[20], j,8,0);
		//=============  KNN Query  ==============
		//vector<AnsLeaf*> tmpresk = sforest[j]->knn(record->arr[20], j, 12);
	}	

	//outp(3);
	
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
		float* radius;
		int* kvalues;

		in1 >> quemk;
		kvalues = new int[quemk];
		for (int i = 0; i < quemk; i++) in1 >> kvalues[i];

		in1 >> quemr;
		radius = new float[quemr];
		for (int i = 0; i < quemr; i++) in1 >> radius[i];

		in1 >> qcount;
		for (int i = 0; i < qcount; i++) {
			in1 >> quepos[i];
			quepos[i] %= n;
		}
		in1.close();
		clock_t begin, endt;
		begin = clock();
		ofstream ou1;
		ou1.open("ans-sf-8k.txt");

		for (int i = 0; i < quemk; i++) {
				compdists = 0;
				IOread = 0;
				//Index_c->clear();
				begin = clock();
				for (int j = 0; j < qcount; j++) {
					vector<Object*> tmpres = multiknn_q(record->arr[quepos[j]], kvalues[i]);
					int k = tmpres.size();
					for (int jj = 0; jj < tmpres.size(); jj++) {
						delete tmpres[jj];
						tmpres[jj] = NULL;
					}
				}
				endt = clock();
				ou1 << "knn=" << kvalues[i] << "\t" << (endt - begin) * 1.0 / CLOCKS_PER_SEC / qcount;
				ou1 << "\t" << compdists * 1.0 / qcount;
				ou1 << "\t" << IOread * 1.0 / qcount << endl;
			}
		
		

		ou1.close();

	}
	return 0;


	Object* que = record->arr[12];
	//=============  Multi Range Query  ==============
	//multirange_q(record->arr[12], 40);
	//multirange_q(que, 10);
	//multiknn_q(que, 30);

	//=============  Query Test ==============
	//m = 4;

	for (int i = n / 6; i < n; i += n / 6) {
		//	for (int i = 0; i < n / 20; i++) {
		que = record->arr[i];
		//cout << "query ";
		//que->outnode();
		compdists = 0;
		vector<Object*> tmps = multirange_q(que, 0.2);
		cout << "Multi Range size " << tmps.size() << endl;
		for (int i = 0; i < tmps.size(); i++) {
			delete tmps[i];
			tmps[i] = NULL;
		}
		cout << "dcmp " << compdists << endl;
	}

	for (int i = n / 6; i < n; i += n / 6) {
		compdists = 0;
		vector<Object*> tmps = multiknn_q(que, 20);
		cout << "Multi KNN size " << tmps.size() << endl;
		cout << "dcmp " << compdists << endl;
		for (int i = 0; i < tmps.size(); i++) {
			delete tmps[i];
			tmps[i] = NULL;
		}

	}
	int knum = 0;
	//=============  delete N/2 nodes  ==============
	for (int i = max(n / 50 - 2, 0) + 1; i < n / 10; i++){
		//cout<<endl << "======== Deleting ";
		//record->arr[i]->outnode();
		knum++;
		for (int j = st; j < m; j++) {
			vector<AnsLeaf*> tmp = sforest[j]->rnn(record->arr[i], j,eps,1);
			for (int ii = 0; ii < tmp.size(); ii++) {
				if (objloc[i] == tmp[ii]->obj) {
					bptree* ptt = new bptree();
					ptt->loadroot(tmp[ii]->loc);					
					ptt->delnode(tmp[ii]->obj);
					delete ptt;					
				};
				delete tmp[ii];
				tmp[ii] = NULL;
			}			
			tmp.clear();			

			int tmpnp = round(pow(n-knum, PIVOTN));
			if (tmpnp < sforest[j]->arr.size()) {
				compdists = 0;
				delpiv(j);
				//outp(j);
				//cout << "dcmp " << compdists << endl;
			}		
		}
	}

	return 0;
}

Treearr* findnear(bpnode* q) {
	int j = q->metric;
	int k = -1;
	float dis = -1;
	//cout << j << " is j" << endl;
	//outp(j);
	for (int i = 0; i < sforest[j]->arr.size(); i++) {
		
		//sforest[j]->arr[i]->outnode();
		//q->outnode("que ");
		float tmp = baseme(sforest[j]->arr[i]->datad,q->datad, sforest[j]->arr[i]->datas,q->datas, j,q->len);
		if (tmp < dis || dis == -1) {
			dis = tmp;
			k = i;
		}
	}	
	q->dis = dis;
	return sforest[j]->arr[k];
}
void update(int metric) {

	int flag = 0;
//	cout << "updating 1"<< endl;

	SF* tmp = sforest[metric];
	int m = tmp->arr.size();
	bptree** candlist = new bptree * [m];

	//candlist.empty();
	for (int i = 0; i < m; i++) {
		if (tmp->arr[i]->tree<0) continue;
		bptree* p = new bptree();
		p->loadroot(tmp->arr[i]->tree);		
		p = p->lastnode();
		//p->outnode("candlist ");
		candlist[i] = p;
	}
	int k = 0;

	for (int i = 1; i < m; i++) {
		bptree* p = candlist[i];
		bptree* q = candlist[k];
		if (p->son[p->leafnum - 1]->disr > q->son[q->leafnum - 1]->disr) k = i;
	}
	tmp->arr[k]->tot--;
	Treearr* tmpp = new Treearr();
	//cout << "updating 2" << endl;
	{
	bptree* p = candlist[k];
//	cout << "adding  ";
	//p->son[p->leafnum - 1]->outnode("new pivot");

	bptree* p1 = p->prenode(p->son[p->leafnum - 1]->objloc);
	//p1->outnode("p1 ");
	candlist[k] = p1;
	
	bpnode* q = new bpnode();
	q->copy(p->son[p->leafnum - 1]);
	//q->outnode("======== ");
	tmpp->len=q->len;
	tmpp->metric = q->metric;
	tmpp->datad = q->datad;
	tmpp->datas = q->datas;
	//cout << "========= "<<tmpp->metric<<" "<<q->metric<<" tmpp ";
	//tmpp->outnode();
	q->datad = NULL;
	q->datas = NULL;
	q->len = 0;
	q->metric = -1;
	delete q;
	//cout << "updating 3" << endl;
	q = new bpnode();
	q->copy(p->son[p->leafnum - 1]);
	bptree* ptree = new bptree();
	char* buffer = new char[Obj_f->file->blocklength];
	ptree->write_to_buffer(buffer);
	tmpp->tree = Index_f->file->append_block(buffer);
	
	tmp->arr.push_back(tmpp);	
	tmp->arr[m]->tot++;
	delete ptree;
	//cout << "updating 5" << endl;
	p->delnode(p->son[p->leafnum - 1]->objloc);
	//p->outnode("p ");
//	p1->outnode("p1 ");
	if (p != p1) {
	//	p->outnode("p ");
	//	p1->outnode("p1 ");
		delete p;
	}

	bptree* ptt = new bptree();

	ptt->loadroot(tmpp->tree);
	ptt->insert(q);
	delete ptt;
	}
	//cout << "updating 4" << endl;
	for (int i = 0; i < m; i++) {
		while (candlist[i] != NULL) {
		
			bptree* p1 = candlist[i];
			//p1->outnode("updating 5 prenode p ");
			bpnode* p1s = p1->son[p1->leafnum - 1];
			float dis = baseme(p1s->datad, tmpp->datad, p1s->datas, tmpp->datas, metric, tmpp->len);
			//cout << "dis "<<dis << endl;
			//p1s->outnode("p1s ");
			//cout << "tmpp ";
			//tmpp->outnode();
			if (p1s->disl > dis) {
				tmp->arr[i]->tot--;
				tmp->arr[m]->tot++;
				bptree* p11 = p1->prenode(p1s->objloc);
				candlist[i] = p11;
				bpnode* q = new bpnode();
				q->copy(p1s);
				//p1s->outnode("!!! p1s ");
				
				q->disl = dis;
				q->disr = dis;
				q->dis = dis;
				//q->outnode("!!! q ");
				bptree* ptt = new bptree();
				ptt->loadroot(tmpp->tree);
				ptt->insert(q);
				//q->outnode("change ");
				//ptt->bfs("check ");
				//cout << "updating 4.5" << endl;
				delete ptt;
				//cout << "updating 4.6" << endl;	
				
				p1->delnode(p1s->objloc);
				if(p1!=p11) {
					//p1->outnode("deleting p1 ");
					//p11->outnode("p11 ");
					delete p1;
				}
			}
			else {
				delete p1;
				candlist[i] = NULL;
			}
		}
	}
	//cout << "updating 6" << endl;
	for (int i = tmp->arr.size() - 1; i >= 0; i--) {
		int flg = 0;
		if (tmp->arr[i]->tree >= 0) {
			bptree* p = new bptree();
			p->loadroot(tmp->arr[i]->tree);
			if (p->leafnum > 0) flg=1;
			delete p;		
		}
		if (flg == 1) continue;
		delete tmp->arr[i];
		tmp->arr[i] = NULL;
		tmp->arr.erase(tmp->arr.begin() + i);
	}
	for (int i = 0; i < m; i++) {
		delete candlist[i];
		candlist[i] = NULL;
	}
	delete[] candlist;

}
void delpiv(int metric) {
	//cout << "deleting when there is " << npiv << endl;
	//find a candidate pivot
	SF* tmp = sforest[metric];
	int m = tmp->arr.size();

	//candlist.empty();
	int k = -1;
	float min = -1;
	for (int i = 0; i < m; i++) {		
		float dis = -1;
		for (int j = 0; j < m; j++)
			if (i == j) continue;
			else {
				
				float tmpdis = baseme(tmp->arr[i]->datad, tmp->arr[j]->datad, tmp->arr[i]->datas, tmp->arr[j]->datas, metric, tmp->arr[i]->len);
				
				if (tmpdis < dis || dis == -1) {
					dis = tmpdis;
				}
			}
		
		if (dis <= min || min < 0) {
			min = dis;
			if (dis < min||min<0) {
				k = i;
			}
			else if (k == -1 || tmp->arr[i]->tot < tmp->arr[k]->tot)	k = i;
		}
		//cout << dis<<" "<<min << endl;
	}

	vector <bptree*> queue;
	bptree* pt = new bptree();
	pt->loadroot(tmp->arr[k]->tree);

	queue.push_back(pt);
	//cout << "removing "<<min<<" ";
	//tmp->arr[k]->outnode();
	tmp->arr.erase(tmp->arr.begin() + k);
	while (queue.size() > 0) {
		pt = queue[queue.size() - 1];
		queue[queue.size() - 1] = NULL;
		queue.pop_back();
		//pt->outnode("this");
		if (pt->isleaf == 1) {
		//	cout << "delpiv 1" << endl;
			for (int i = 0; i < pt->leafnum; i++) {
				bpnode* q = new bpnode();
				q->copy(pt->son[i]);
				Treearr* tmp = findnear(q);
				tmp->tot++;				
				q->disl = q->dis;
				q->disr = q->dis;
				bptree* p = new bptree();
				//cout << "doing 1 " << j << " " << i << endl;
				p->loadroot(tmp->tree);
				p->insert(q);
				delete p;
			}
		}
		else {
			//cout << "delpiv 2" << endl;
			for (int i = 0; i < pt->leafnum; i++) {				
				bptree* p = new bptree();

				//cout << "doing 1 " << pt->son[i]->son << endl;
				p->loadroot(pt->son[i]->son);
				queue.push_back(p);				
			}
		}
		delete pt;
		
	}
	queue.clear();
}
bool Obj_Comp(const AnsLeaf* a, const AnsLeaf* b) {
	if (a->obj < b->obj)    return true;
	return false;
}
bool Ans_Comp(const AnsLeaf* a, const AnsLeaf* b) {
	if (a->dis < b->dis)    return true;
	return false;
}
vector<Object*> multirange_q(Object* queobj, float r) {
//	for (int i = 0; i < m; i++) cout <<i<<" "<< querym[i] << " querym " << endl;
	vector<AnsLeaf*>* mulpri_a = new vector<AnsLeaf*>[m];
	vector<AnsLeaf*> allobj;
	vector<Object*> candlist;
	double totm = 0;
	for (int i = 0; i < m; i++)  totm+= querym[i] ;
	for (int i = 0; i < m; i++) if (querym[i] >0.000001) {
		//bpnode* q = new bpnode(i, queobj);
		vector<AnsLeaf*> tmp = sforest[i]->rnn(queobj, i, r/ totm,0);		
	//	sort(tmp.begin(), tmp.end(), Ans_Comp);
		mulpri_a[i].swap(tmp);
		for (int j = 0; j < mulpri_a[i].size(); j++) {
			allobj.push_back(mulpri_a[i][j]);
		}
	}
	sort(allobj.begin(), allobj.end(), Obj_Comp);
	for (int i = 0; i < allobj.size(); i++) {
		if (i == 0 || allobj[i]->obj != allobj[i - 1]->obj) {
			Object* q = Obj_f->get_object(allobj[i]->obj);
			q->ansres = new float[m];
			for (int j = 0; j < m; j++) if (querym[j] >0.00001)q->ansres[j] = r + 1;
			allobj[i]->o = q;
			candlist.push_back(q);
		}
		else allobj[i]->o = allobj[i - 1]->o;
		allobj[i]->o->ansres[allobj[i]->metric] = allobj[i]->dis;
		//cout << "allo " << allobj[i]->dis << endl;
	}
	allobj.clear();
	//	cout << "multi search end" << endl;
	int maxi = 0;
	float* tmparr = new float[m];
	for (int i = 0; i < m; i++)if (querym[i] >0.000001) {
		int k = mulpri_a[i].size();
		if (k > maxi) maxi = k;
		tmparr[i] = r  / totm;
	}
	float distot = 0;
	int j = 0;

	//for (int i = 0; i < m; i++) cout << mulpri_a[i].size() << " ";	cout << " thjis is" << endl;

	while (distot < r && j < maxi) {
		distot = 0;
		for (int i = 0; i < m; i++)if (querym[i] >0.0000001) {
			int k = j;
			if (k >= mulpri_a[i].size()) continue;
			mulpri_a[i][k]->o->ansres[i] = mulpri_a[i][k]->dis * querym[i];
			//cout << "mulpre " << mulpri_a[i][k]->dis << endl;
		}
		j++;
	}

	//cout << " Test Multi " << candlist.size() << endl;
	for (int jj = 0; jj < candlist.size(); jj++) {
		Object* tmp = candlist[jj];
		if (tmp == NULL) continue;
		float dis = 0;
		for (int i = 0; i < m; i++)if (querym[i] >0.0000001) {
			int k = j;
			dis += min(tmparr[i], tmp->ansres[i]);
			//	cout << tmparr[i] << " " << tmp->ansres[i] << " ";
				//			cout<<tmp->treenode->data->ansres[i]<<" ";
		}
		//cout << endl;
		//cout<<endl << "judge 1.5 " << dis << endl;
		//dis = 0; // **********************
		if (dis > r) {
			delete candlist[jj];
			candlist[jj] = NULL;
			continue;
		}
		dis = 0;
		for (int i = 0; i < m; i++) if (querym[i] >0.0000001) {
			//tmp->ansres[i] = r + 1;  // **********************
			if (tmp->ansres[i] > r) {
				bpnode* que = new bpnode(i, queobj);
				bpnode* kk = new bpnode(i, tmp);
				tmp->ansres[i] = que->distance(kk) * querym[i];
				delete que;
				delete kk;
			}
			dis += tmp->ansres[i];
		}
		tmp->dist = dis;
		if (dis > r) {
			delete candlist[jj];
			candlist[jj] = NULL;
		}
	}

	//cout << " Test Multi 2 " << candlist.size() << endl;
	for (int i = 0; i < m; i++) if (querym[i] >0.000001) {
		for (int jj = 0; jj < mulpri_a[i].size(); jj++) {
			mulpri_a[i][jj]->o = NULL;
			delete mulpri_a[i][jj];
		}
		mulpri_a[i].clear();
	}
	delete[] mulpri_a;
	delete[] tmparr;


	vector<Object*> mulres;
	int tots = 0;
	for (int jj = 0; jj < candlist.size(); jj++) {
		Object* q = candlist[jj];
		if (q == NULL) continue;
		else {
		//	cout << "finded in Multi RNN " << tots++ << " dis "<<q->dist<<" ";
		//	q->outnode();
			mulres.push_back(q);
			candlist[jj] = NULL;
		}
	}
	candlist.clear();
	return mulres;
}

vector<Object*> multiknn_q(Object* queobj, int k) {
	k++;
	vector<AnsLeaf*>* mulpri_a = new vector<AnsLeaf*>[m];

	vector<AnsLeaf*> allobj;
	vector<Object*> candlist;

	struct cmp1 {
		bool operator() (float a, float b) {

			if (a > -0.5 && a < b) return 1;
			return 0;
		}
	};
	priority_queue<float, vector<float>, cmp1> reslist;
	int ord = -1;
	for (int i = 0; i < m; i++) if (querym[i] >0.00001) {
		if (ord == -1 || metricmeandis[i]*querym[i]>metricmeandis[ord]*querym[ord]) ord = i;
	}
	{
		//bpnode* q = new bpnode(i, queobj);
		vector<AnsLeaf*> tmp = sforest[ord]->knn(queobj, ord,k);
		//cout << "dcmp " << compdists << endl;
		//cout << "KNN " << i << " size " << tmp.size() << endl;
		//sort(tmp.begin(), tmp.end(), Ans_Comp);
		mulpri_a[ord].swap(tmp);
		for (int j = 0; j < mulpri_a[ord].size(); j++) {
			allobj.push_back(mulpri_a[ord][j]);
		}
	}
	sort(allobj.begin(), allobj.end(), Obj_Comp);

	for (int i = 0; i < allobj.size(); i++) {
		if (i == 0 || allobj[i]->obj != allobj[i - 1]->obj) {
			Object* q = Obj_f->get_object(allobj[i]->obj);
			q->ansres = new float[m];
			for (int j = 0; j < m; j++) q->ansres[j] = -1;
			allobj[i]->o = q;
			candlist.push_back(q);
		}
		else allobj[i]->o = allobj[i - 1]->o;
		allobj[i]->o->ansres[allobj[i]->metric] = allobj[i]->dis;
	}
	allobj.clear();
	//	cout << "multi search end" << endl;

	int maxi = 0;
	float* tmparr = new float[m];
	for (int i = 0; i < m; i++)if (querym[i] >0.000001) {
		int k = mulpri_a[i].size();
		if (k > maxi) maxi = k;
		if (k > 0) tmparr[i] = mulpri_a[i][k - 1]->dis*querym[i];
		else tmparr[i] = -1;

	}

	int j = 0;

	//for (int i = 0; i < m; i++) cout << mulpri_a[i].size() << " ";	cout << " thjis is" << endl;

	while (j < maxi) {
		for (int i = 0; i < m; i++)if (querym[i] >0.000001) {
			int k = j;
			if (k >= mulpri_a[i].size()) continue;
			mulpri_a[i][k]->o->ansres[i] = mulpri_a[i][k]->dis*querym[i];
		}
		j++;
	}

	for (int jj = 0; jj < candlist.size(); jj++) {
		Object* tmp = candlist[jj];
		if (tmp == NULL) continue;
		float dis = 0;
		for (int i = 0; i < m; i++) if (querym[i] >0.000001) {
			float dd = tmp->ansres[i];
			if (tmp->ansres[i] < -0.5) dd = tmparr[i];
			if (dd < -0.5) continue;
			dis += dd;
		}
		//cout<< "judge 1.5 " << dis << endl;
		float dk = reslist.top();
		dis = 0;
		for (int i = 0; i < m; i++) if (querym[i] >0.0000001) {
			if (tmp->ansres[i] < -0.5) {
				bpnode* que = new bpnode(i, queobj);
				bpnode* kk = new bpnode(i, tmp);
				tmp->ansres[i] = que->distance(kk)*querym[i];
				delete que;
				delete kk;
			}
			dis += tmp->ansres[i];
			//cout << tmp->ansres[i] << " ";
		}
		reslist.push(dis);
	//	cout << "========== Try ing :"<< reslist.top()<<" Dis : " << dis << " Obj :"<<endl; 
		//	tmp->outnode();

		while (reslist.size() > k) {
			//	cout << "hei " << reslist.size() << " " << dis << endl;
			reslist.pop();
		}
		delete candlist[jj];
		candlist[jj] = NULL;
	}

	for (int i = 0; i < m; i++) if (querym[i] == 1) {
		for (int jj = 0; jj < mulpri_a[i].size(); jj++) {
			mulpri_a[i][jj]->o = NULL;
			delete mulpri_a[i][jj];
		}
		mulpri_a[i].clear();
	}
	delete[] mulpri_a;
	delete[] tmparr;
	candlist.clear();
	//cout << "======== waiting k " << reslist.size() << endl;

	candlist = multirange_q(queobj, reslist.top());
	//cout << "======== waiting rnn " << candlist.size() << endl;
	vector<Object*> mulres;

	while (reslist.size() > 0) reslist.pop();
	for (int jj = 0; jj < candlist.size(); jj++) {
		reslist.push(candlist[jj]->dist);
		while (reslist.size() > k) reslist.pop();
	}
	float knnq = reslist.top();
	for (int jj = 0; jj < candlist.size(); jj++) if (candlist[jj]->dist <= knnq) {
	//	cout <<knnq<< " finded in KNN " << mulres.size() << " "<< candlist[jj]->dist<<" ";
	//	candlist[jj]->outnode();
		mulres.push_back(candlist[jj]);
		candlist[jj] = NULL;
	}
	else {
		delete candlist[jj];
		candlist[jj] = NULL;
	}
	candlist.clear();
	//cout << "farthest " << knnq << endl;
	return mulres;



	//=================================================================

}

