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
#include "basemetric.h"
#include "object.h"

using namespace std;
double eps = 0.000000001;

int* toplevel;
vector<Object*> anslist;
vector<Object*> *level_f;


int* objloc;

int max_n=20;
int max_l = 15;
double epsi = 1;
int n, m;
int npiv,nsf;
Objectarr* record;
double* querym;
int* metricm;
double* metricmaxdis;
double* metricmeandis;
clock_t stt, ed;
int st = 0;

double tnn[200];
vector<Object*> knn_single(Object* q, int metric, int knn); 
vector<Object*> knn_multi(Object* q, int knn);
vector<Object*> knn_multi1(Object* q, int knn);

void update(Object* q,int typ);
int main() {
	string filedata, querydata;
	ifstream in1;
	in1.open("data/par.txt");
	int build = 0;
	in1 >> filedata >> querydata>>build;
	in1.close();
	tnn[0] = 0;
	tnn[1] = 0;

//	srand((int)time(NULL));
	srand(0);
	record = new Objectarr();	
	record->readfile(filedata);	
	// for (int i = 0; i < n / 100; i++) record->arr[i]->outnode();
	querym = new double[m];

	if (build == 0) {
		record->arr[0]->edges_no = new int[m];
		record->arr[0]->edges_same = new int[m];
		level_f = new vector<Object*>[m];		
		for (int i = 0; i < m; i++) {
			level_f[i].clear();
			level_f[i].push_back(record->arr[0]);
			querym[i] = 0;
			record->arr[0]->edges_no[i] = 0;
			record->arr[0]->edges_same[i] = 0;
		}
		int tot = 0;
		ofstream ouf;
		clock_t stt11, ed11;
		stt11 = clock();
		ouf.open("Build-time-TBG.txt");
		for (int i = 1; i < n; i++) {
			if (i % 100 == 0) {				
				cout << i << endl;
				cout <<" ======== t1 = " << tnn[0] * 0.001 << " t2 = " << tnn[1] * 0.001 << endl;
			}
			if (i % 100 == 0) {
				int level_n = 15;
				int a[100];
				for (int ii = 0; ii <= level_n; ii++) a[ii] = 0;
				int maxi = 0,posi;
				for (int ii = 0; ii <= i; ii++) {
					int etot = record->arr[ii]->count();					
					if (etot > maxi) {
						maxi = etot;
						posi = ii;
					}
					a[record->arr[ii]->level] ++;
				}		
				while (a[level_n] == 0 && level_n > 0)level_n--;
				for (int ii = 0; ii <= level_n; ii++)
					cout << a[ii] << endl;
				cout << "edges sum: " << tot * 2 / i << " level_f " << level_f[0].size() << " ======== t1 = " << tnn[0] * 0.001 << " t2 = " << tnn[1] * 0.001 << endl;
				cout << "========== edges max: " << maxi << endl;
				record->arr[posi]->outnode();
				for (int ii = 0; ii < m; ii++) {
					cout << "=========          metric : " << ii << " edges " << record->arr[posi]->count(ii)<<endl;
				}
				for (int ii = 0; ii < m; ii++) {
					cout << "metric m: " << ii<< " node: ";
					for (int j = 0; j < level_f[ii].size(); j++) {
						cout << " " << level_f[ii][j]->id;
					}
					cout << endl;
				}
			}
			vector<Object*> tmpobj;
			tmpobj.clear();
			record->arr[i]->level = 0;
			record->arr[i]->edges_no = new int[m];
			record->arr[i]->edges_same = new int[m];
			for (int j = m-1; j >=st; j--) {
				record->arr[i]->edges_no[j] = 0;
				record->arr[i]->edges_same[j] = 0;
				stt = clock();
				vector<Object*> tmp = knn_single(record->arr[i], j, max_n);		
				while (tmp.size() > max_n) tmp.pop_back();
				ed = clock();
				tnn[0] += ed - stt;
				int s = 0;
				for (int k = 0; k < tmp.size(); k++) {					
					if (tmp[k]->metric_f == NULL) {
						tmpobj.push_back(tmp[k]);
						s++;
						tmp[k]->metric_f = new bool[m];
						for (int ii = 0; ii < m; ii++)tmp[k]->metric_f[ii] = 0;
					}
					tmp[k]->metric_f[j] = 1;
				}				
				tmp.clear();

			}
			int tottmp = 0;
			for (int k = 0; k < tmpobj.size(); k++)if (tmpobj[k]->metric_f != NULL) {
				Object* p = record->arr[i];
				Object* q = tmpobj[k];
				int flg=0;
				for (int j = 0; j < m; j++) if(q->metric_f[j]==1){
					double dis = p->distance(q, j);
					//cout << "metric " << j << " id1 " << p->id << " id2 " << q->id << " dis " << dis << endl;
					p->add_e(q, dis, j,q->level);
					q->add_e(p, dis, j,p->level);
					tot++;
					tottmp++;
					if (q->level == p->level ) {
						q->edges_same[j]++;
						p->edges_same[j]++;
						if (q->edges_same[j] >= max_l * max_n) flg = 1;
					}
					else if (q->level == p->level+1) {
						q->edges_no[j]++;
						if (q->edges_no[j] >= max_l * max_n) flg = 2;
					}
					else if (q->level == p->level - 1) {
						p->edges_no[j]++;						
					}
				}
				for (int ii = 0; ii < m; ii++)q->metric_f[ii] = 0;
				delete[] q->metric_f;
				q->metric_f = NULL;
				stt = clock();
				if (flg > 0) update(q, flg);
				ed = clock();
				tnn[1] += ed - stt;
			}
			//cout <<"added " << tottmp << endl;
			tmpobj.clear();
		}
		ed11 = clock();
		ouf << (ed11 - stt11) / 1000.0 << endl;
		string indexname = "";
		{
			int k = 0;
			while (filedata[k] != '.') { indexname += filedata[k]; k++; }

			indexname += ".ind";
		}
		ofstream ouindex;
		ouindex.open(indexname);
		for (int i = 0; i < m; i++) {			
			ouindex << level_f[i].size();
			for (int j = 0; j < level_f[i].size(); j++) {				
				ouindex<<" " << level_f[i][j]->id;
			}
			ouindex << endl;
		}
		ouindex << n << endl;
		for (int i = 0; i < n; i++) {
			int ne = 0;
			e_s3* p = record->arr[i]->all_e;
			while (p != NULL) {
				ne++;
				p = p->next_space;
			}
			ouindex << i << " " << record->arr[i]->level << " " << ne ;

			p = record->arr[i]->all_e;
			while (p != NULL) {
				ouindex<<" " << p->metric_num;
				e_s2* p1 = p->first_level;
				int nl = 0;
				while (p1 != NULL) {
					nl++;
					p1 = p1->next_level;
				}
				ouindex << " " << nl;
				p1 = p->first_level;				
				while (p1 != NULL) {
					ouindex <<" "<< p1->level;
					int nd = 0;
					e_s1* p2 = p1->first_edge;
					while (p2 != NULL) {
						nd++;
						p2 = p2->next_edge;
					}
					ouindex << " " << nd;
					p2 = p1->first_edge;
					while (p2 != NULL) {
						nd++;
						ouindex << " " << p2->obj->id<<" "<<p2->dis;
						p2 = p2->next_edge;
					}
					p1 = p1->next_level;
				}
				p = p->next_space;
			}
			ouindex << endl;
		}
	}
	else {
		string indexname = "";
		{
			int k = 0;
			while (filedata[k] != '.') { indexname += filedata[k]; k++; }
			indexname += ".ind";
		}
		ifstream inindex;
		inindex.open(indexname);

		level_f = new vector<Object*>[m];
		for (int i = 0; i < m; i++) {
			level_f[i].clear();
			int num;
			inindex >> num;
			for (int j = 0; j < num; j++) {
				int id;
				inindex >> id;
				level_f[i].push_back(record->arr[id]);
			}
		}

		inindex >> n;
		int tot = 0;
		for (int i = 0; i < n; i++) {
			/*if (i % 100 == 1) {
				int level_n = 15;
				cout << i << endl;
				int a[100];
				for (int ii = 0; ii <= level_n; ii++) a[ii] = 0;
				for (int ii = 0; ii <= i; ii++) a[record->arr[ii]->level] ++;
				while (a[level_n] == 0 && level_n > 0)level_n--;
				for (int ii = 0; ii <= level_n; ii++)
					cout << a[ii] << endl;
				cout << "edges sum: " << tot * 2 / i << " level_f " << level_f[0].size() << " ======== t1 = " << tnn[0] * 0.001 << " t2 = " << tnn[1] * 0.001 << endl;
			}
			*/
			int id;
			inindex >> id;
			Object* q = record->arr[id];
			q->all_e = NULL;
			inindex >> q->level;
			int ne;
			inindex >> ne;
			for (int i1 = 0; i1 < ne; i1++) {
				e_s3* p = new e_s3;
				p->next_space = q->all_e;
				q->all_e = p;
				int nl;
				p->first_level = NULL;
				inindex >> p->metric_num >> nl;
				for (int i2 = 0; i2 < nl; i2++) {
					e_s2* p1 = new e_s2;
					p1->next_level = p->first_level;
					p->first_level = p1;
					p1->first_edge = NULL;
					int nd;
					inindex >> p1->level >> nd;
					for (int i3 = 0; i3 < nd; i3++) {
						e_s1* p2 = new e_s1;
						p2->next_edge = p1->first_edge;
						p1->first_edge = p2;
						int id2;
						inindex >> id2 >> p2->dis;
						p2->obj = record->arr[id2];
						if (id2 < i && p2->obj->level != p1->level) {
							cout << "error !!!!!!! o1: " << q->id << " o2: " << id2 << " real level: " << p2->obj->level << " edge level" << p1->level << endl;
							system("pause");
						}
						tot++;
					}
				}
			}
		}
		for (int i = 0; i < m; i++) {
			cout<<"metric m: "<<i;
			for (int j = 0; j < level_f[i].size(); j++) {
				cout << " " << level_f[i][j]->level;				
			}
			cout << endl;
		}
	}

	if(build==1){
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
		double notd;
		for (int i = 0; i < quemr; i++) in1 >> notd;
		in1 >> qcount;
		for (int i = 0; i < qcount; i++) {
			in1 >> quepos[i];
			quepos[i] %= n;
		}
		in1.close();
		clock_t begin, endt;
		begin = clock();
		ofstream ou1;
		ou1.open("ans-tbg.txt");
		cout << qcount << " " << quemk << endl;
		for (int i = 0; i < quemk; i++) {		
				double tim = 0;
				begin = clock();
				for (int j = 0; j < qcount; j++) {
					//cout << "========= query is ";
					//record->arr[quepos[j]]->outnode();
					begin = clock();
					vector<Object*> tmpres = knn_multi(record->arr[quepos[j]], kvalues[i]);
					//cout << "Finded " << tmpres.size() << endl;
					tim += clock() - begin;
					int k = tmpres.size();
					double max1 = 0;
					
					for (int jj = 0; jj < tmpres.size(); jj++) {
						//cout << "dis " << tmpres[jj]->dist << " o: ";
						//tmpres[jj]->outnode();
						max1 = MAX(tmpres[jj]->dist,max1);						
					}
					ou1 << "k " << max1 << " "<<tmpres.size();
					for (int jj = 0; jj < tmpres.size(); jj++) {						
						ou1 << " " << tmpres[jj]->id;
						tmpres[jj] = NULL;
					}
					ou1 <<endl;
					tmpres.clear();					
				}
				ou1<< "t\t" << (tim) * 1.0 / CLOCKS_PER_SEC / qcount<<endl;				
			}
		//system("pause");
		ou1.close();

	}
	return 0;
	Object* que = record->arr[12];
	/*
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
	*/
	return 0;
}
vector<Object*>  update_edge(Object* q) {
	vector<Object*> tmp;
	tmp.clear();

	e_s3* p = q->all_e;
	while (p != NULL) {
		e_s2* p1 = p->first_level;
		int j = p->metric_num;
		while (p1 != NULL) {
			e_s1* p2 = p1->first_edge;
			while (p2 != NULL) {				
				Object* o = p2->obj;				
				//cout << "before remove: " << o->count();
				o->remove_e(q, p2->dis, j, q->level);	
				o->add_e(q, p2->dis, j, q->level+1);		
				//cout << " ; After remove: " << o->count()<<endl;
				if (p1->level == q->level + 2) {					
					if (o->edges_no[j] >= max_l * max_n)o->fgg = 2;						
					if (o->visited == false) {
						tmp.push_back(o);
						o->visited = true;
					}
				}
				else if (p1->level == q->level + 1) {				
					if (o->edges_same[j] >= max_l * max_n) {
						if (o->visited == false) {
							o->fgg = 1;
							tmp.push_back(o);
							o->visited = true;
						}
					}
					q->edges_same[j]++;
				}
				else if (p1->level == q->level) {
					q->edges_same[j]--;
					q->edges_no[j]++;
				}
				else if (p1->level == q->level-1) {					
					q->edges_no[j]--;
				}				
				p2 = p2->next_edge;				
			}			
			p1 = p1->next_level;
		}		
		p = p->next_space;
	}
	for (int i = 0; i < tmp.size(); i++) {
		tmp[i]->visited = false;
	}
	return tmp;
}
void  split_node(Object* q) {
	vector<Object*> tmp;
	tmp.clear();
	e_s3* p = q->all_e;
	while (p != NULL) {
		e_s2* p1 = p->first_level;
		while (p1 != NULL && p1->level != q->level - 1) p1 = p1->next_level;
		if(p1!=NULL&&p1->first_edge!=NULL){			
			e_s1* p2 = p1->first_edge;
			e_s1* tmpp = p2;
			double min = p2->dis;
			e_s1* tmpm = p2;
			double max = p2->dis;				
			while (p2 != NULL) {
				if ( p2->dis < min) {
					min = p2->dis;
					tmpp = p2;
				}
				if (p2->dis > max) {
					max = p2->dis;
					tmpm = p2;
				}
				p2 = p2->next_edge;
			}
			if (tmpp->obj->visited == false) {
				tmp.push_back(tmpp->obj);
				tmpp->obj->visited = true;
			}				
			if (tmpm->obj->visited == false) {
				tmp.push_back(tmpm->obj);
				tmpm->obj->visited = true;
			}
		}		
		p = p->next_space;		
	}

	for (int i = 0; i < tmp.size(); i++) {
		vector<Object*> tmp1 = update_edge(tmp[i]);
		tmp[i]->level++;
		tmp1.clear();
		tmp[i]->visited = false;
	}
	tmp.clear();
}
void check_f(int metric, Object* q) {
	return;
	e_s3* p = q->all_e;
	vector<Object*> tmp;
	tmp.clear();	
	while (p != NULL && p->metric_num != metric) p = p->next_space;
	if(p!=NULL){
		e_s2* p1 = p->first_level;
		while (p1 != NULL) {
			e_s1* p2 = p1->first_edge;
			while (p2 != NULL) {
				p2->obj->visited = true;
				tmp.push_back(p2->obj);
				p2 = p2->next_edge;
			}
			p1 = p1->next_level;
		}
		for (int i = 0; i < level_f[metric].size(); i++) {
			if (level_f[metric][i]->visited == true) {
				level_f[metric][i] = level_f[metric][level_f[metric].size()-1];
				level_f[metric].pop_back();
				i--;
			}
		}
		for (int i = 0; i < tmp.size(); i++) tmp[i]->visited = false;		
		tmp.clear();
	}
}
void update(Object* q,int typ) {
	q->fgg = 0;
	vector<Object*> tmp;
	tmp.clear();
	
	if (typ == 2) {		
		split_node(q);
	}
	tmp = update_edge(q);
	q->level++;
	
	for (int i = 0; i < tmp.size(); i++) {
		if(tmp[i]->fgg>0) update(tmp[i], tmp[i]->fgg);
	}
	e_s3* p = q->all_e;
	while (p != NULL) {
		e_s2* p1 = p->first_level;
		while (p1 != NULL && p1->level < q->level) p1 = p1->next_level;
		if (p1 == NULL) {
			int flg = 0;
			for (int i = 0; i < level_f[p->metric_num].size(); i++) {
				if (level_f[p->metric_num][i]->id == q->id) {
					flg = 1;				
					if (flg == 1) break;
				}				
			}
			if(flg==0){
				level_f[p->metric_num].push_back(q);
				check_f(p->metric_num, q);
			}
		}
		p = p->next_space;
	}
}

struct cmpnodedissmall
{
	bool operator() (Object* a, Object* b)
	{
		if (a->dist == b->dist && a->level < b->level) return true;
		return a->dist < b->dist; //smaller
	}
};

struct cmpnodedis
{
	bool operator() (Object* a, Object* b)
	{		
		return a->dist > b->dist; //min heap
	}
};
struct cmplevel
{
	bool operator() (Object* a, Object* b)
	{
		return a->level < b->level; //max heap
	}
};
vector<Object*> knn_single(Object* q, int metric,int knn) {
	vector<Object*> tmp;
	vector<Object*> ans;
	priority_queue<double> que;
	while (que.size() > 0) que.pop();
	priority_queue<Object*, vector<Object*>, cmplevel> quelevel;
	priority_queue<Object*, vector<Object*>, cmpnodedis> tmpobjs;
	while (quelevel.size() > 0) quelevel.pop();
	for (int i = 0; i < level_f[metric].size(); i++) {
		quelevel.push(level_f[metric][i]);
		level_f[metric][i]->dist = 0;
		level_f[metric][i]->visited = true;
	}
	double min;
	//cout << "quelevel.size = "<<quelevel.size()<<endl;
	int sum = 0;
	int sum1 = 0;
	tmp.clear();
	while (!quelevel.empty()) {		
		while (!tmpobjs.empty()) tmpobjs.pop();		
		int k = quelevel.top()->level;
		priority_queue<double> distlist;
		while (!quelevel.empty()&&quelevel.top()->level == k) {
			tmpobjs.push(quelevel.top());
			quelevel.pop();
		}		
		
		while (!tmpobjs.empty()) {
			Object* o = tmpobjs.top();
			tmp.push_back(o);
			tmpobjs.pop();			
			if (distlist.size() < knn||o->dist <= distlist.top()) {				
				o->dist= o->distance(q, metric);
				distlist.push(o->dist);
				while (distlist.size() > knn) distlist.pop();
				if (distlist.size() < knn || o->dist <= distlist.top()) {
					e_s3* p = o->all_e;
					while (p != NULL && p->metric_num != metric) p = p->next_space;
					if(p!=NULL&&p->metric_num==metric){
						e_s2* p1 = p->first_level;
						while (p1 != NULL&&p1->level!=o->level) p1 = p1->next_level;
						if (p1 != NULL) {
							e_s1* p2 = p1->first_edge;
							while (p2 != NULL) {
								sum1++;
								if (p2->obj->visited == false) {
									tmpobjs.push(p2->obj);
									p2->obj->visited = true;
									p2->obj->dist = abs(p2->dis - o->dist);
								}
								p2 = p2->next_edge;
							}
						}
					}
					//cout << "tmpk-dist " << o->dist << " distilist " << distlist.top() << endl;					
				}
			}
		}
	//	cout <<"quelevel "<<quelevel.size()<<" level "<< k<< " distlist.size = " << distlist.size() << endl;
		min = distlist.top();
		while (!distlist.empty()) distlist.pop();
		for (int i = 0; i < tmp.size(); i++) {
			tmp[i]->visited = false;
			if (tmp[i]->dist <= min) {
				sum++;
				que.push(tmp[i]->dist);
				ans.push_back(tmp[i]);
				Object* o = tmp[i];
				e_s3* p = o->all_e;
				while (p != NULL && p->metric_num != metric) p = p->next_space;
				if (p != NULL&&p->metric_num==metric) {
					e_s2* p1 = p->first_level;
					while (p1 != NULL){
						if (p1->level < o->level) {
							e_s1* p2 = p1->first_edge;
							while (p2 != NULL) {
								sum1++;
								if (p2->obj->visited == false) {	
									quelevel.push(p2->obj);
									p2->obj->visited = true;
									p2->obj->dist = abs(p2->dis - o->dist);
								}
								p2 = p2->next_edge;
							}
						}
						p1 = p1->next_level;
					}					
				}
			}
		}		
	//	cout << "new quelevel " << quelevel.size() << " level " << k << " distlist.size = " << distlist.size() << endl;
		tmp.clear();
		if (que.size() >= knn && que.top() <= 0.000001) {
			int flg = 0;

			while (!quelevel.empty()) {
				quelevel.top()->visited = false;
				quelevel.pop();
			}
			break;
		}
	}
	while (!que.empty() > knn) que.pop();
	min = que.top();
	while (!que.empty()) que.pop();
	for (int i = 0; i < ans.size(); i++) if (ans[i]->dist <= min) tmp.push_back(ans[i]);
	sort(tmp.begin(),tmp.end(), cmpnodedissmall());
	if(min<0.000001){
		Object* o = tmp[0];
		tmp.clear();
		tmp.push_back(o);
	}
	//for (int i = 0; i < MIN(5,tmp.size()); i++) cout << tmp[i]->dist << " ";
	//cout << endl;
	ans.clear();
	return tmp;
}

vector<Object*> knn_multi(Object* q, int knn) {
	vector<Object*> tmp;
	vector<Object*> ans;
	priority_queue<double> que;
	while (que.size() > 0) que.pop();
	priority_queue<Object*, vector<Object*>, cmplevel> quelevel;
	priority_queue<Object*, vector<Object*>, cmpnodedis> tmpobjs;
	while (quelevel.size() > 0) quelevel.pop();
	for (int j = 0; j < m; j++) if (querym[j] > 0.00001) 
		for (int i = 0; i < level_f[j].size(); i++) if(level_f[j][i]->visited ==false){
			quelevel.push(level_f[j][i]);
			level_f[j][i]->dist = 0;
			for(int k = 0; k < m; k++)level_f[j][i]->ansres[k] = 0;
			level_f[j][i]->visited = true;
		}
	double min;	
	int tot = 0, tot1 = 0;
	while (!quelevel.empty() > 0) {
	//	cout << "to " << tot << " " << tot1;
	//	if (que.size() > 0)cout << " " << que.top();
		//cout << endl;
		tmp.clear();
		while (!tmpobjs.empty()) tmpobjs.pop();
		int k = quelevel.top()->level;
		while (!quelevel.empty() && quelevel.top()->level == k) {
			Object* o = quelevel.top();
			o->dist = 0;
		//	for (int j = 0; j < m; j++) if (querym[j] > 0.00001) o->dist += o->ansres[j] * querym[j];
			tmpobjs.push(o);
			quelevel.pop();
		}
		//cout << "working level " << k << endl;
		//if (!quelevel.empty()) cout << "suppose next level " << quelevel.top()->level << endl;
		priority_queue<double> distlist;
		while (!tmpobjs.empty()) {
			Object* o1 = tmpobjs.top();			
			//if (o1->level > k) cout << "!!!!!!!! o1 " << o1->level  << endl;
			tmp.push_back(o1);
			tmpobjs.pop();
			int flg = 0;		
			if (distlist.size() < knn || o1->dist <= distlist.top() * epsi) {
				o1->dist = 0;
				for (int j = 0; j < m; j++)if (querym[j] > 0.000001) {
					o1->dist += o1->ansres[j] * querym[j];
				}
				if (distlist.size() < knn || o1->dist <= distlist.top() * epsi) flg = 1;
			}
			if (flg == 1) {
				flg = 0;
				o1->dist = 0;
				for (int j = 0; j < m; j++)if (querym[j] > 0.000001) {
					o1->ansres[j] = o1->distance(q, j);
					o1->dist += o1->ansres[j] * querym[j];
				}
				distlist.push(o1->dist);
				//if (tmpd > o1->dist)cout << " Finding error " << tmpd << " " << o1->dist << endl;
				while (distlist.size() > knn) distlist.pop();
				if (distlist.size() < knn || o1->dist <= distlist.top() * epsi) flg = 1;
			}

			if(flg==1){				
				e_s3* p =o1->all_e;
				while (p != NULL) {
					int j = p->metric_num;
					if (querym[j] > 0.0001) {
						e_s2* p1 = p->first_level;
						while (p1 != NULL&& p1->level != o1->level) p1 = p1->next_level; 
						if (p1!=NULL) {
							e_s1* p2 = p1->first_edge;
							while (p2 != NULL) {
								Object* o = p2->obj;
								if (o->level > k) cout << "o1 " << o1->level << " o " << o->level << endl;
								o->ansres[j] = max(p2->obj->ansres[j], abs(p2->dis - o1->ansres[j]));
								if (o->visited == false) {
								
									o->visited = true;
									o->dist = o->ansres[j];
									tmpobjs.push(o);
								}										
								p2 = p2->next_edge;
							}							
						}
					}
					p = p->next_space;
				}				
			}
		}
		min = distlist.top();
		//cout << "level is " << k << " size is " << distlist.size() << endl;
		while (distlist.size() > 0) distlist.pop();
		
		for (int i = 0; i < tmp.size(); i++) {
			Object* o1 = tmp[i];
			o1->visited = false;
			if (o1->dist <= min) {
				que.push(o1->dist);
				ans.push_back(o1);				
				e_s3* p = o1->all_e;
				while (p != NULL) {
					int j = p->metric_num;
					if (querym[j] > 0.00001) {						
						e_s2* p1 = p->first_level;
						while (p1 != NULL) {
							if (p1->level < o1->level) {
								
								e_s1* p2 = p1->first_edge;
								while (p2 != NULL) {
									Object* o = p2->obj;
									if(o->level>=k)cout << "o1 " << o1->level << " smaller o " << o->level << endl;
									o->ansres[j] = max(o->ansres[j], abs(p2->dis - o1->ansres[j]));
									if (o->visited == false) {										
										quelevel.push(o);			
										tot1++;
										o->visited = true;
									}									
									p2 = p2->next_edge;
								}
							}							
							p1 = p1->next_level;
						}
					}					
					p = p->next_space;
				}
			}

			for (int j = 0; j < m; j++)o1->ansres[j] = 0;
		}
	}
	tmp.clear();
	while (que.size() > knn) que.pop();
	min = que.top();
	while (que.size() > 0) que.pop();
	for (int i = 0; i < ans.size(); i++) if (ans[i]->dist <= min) tmp.push_back(ans[i]);
	ans.clear();
	return tmp;
}

vector<Object*> knn_multi1(Object* q, int knn) {
	vector<Object*> tmp;
	vector<Object*> ans;
	priority_queue<double> que;
	while (que.size() > 0) que.pop();
	priority_queue<Object*, vector<Object*>, cmplevel> quelevel;
	priority_queue<Object*, vector<Object*>, cmpnodedis> tmpobjs;
	while (quelevel.size() > 0) quelevel.pop();
	for (int j = 0; j < m; j++) if (querym[j] > 0.00001)
		for (int i = 0; i < level_f[j].size(); i++) if (level_f[j][i]->visited == false) {
			quelevel.push(level_f[j][i]);
			level_f[j][i]->dist = 0;
			for (int k = 0; k < m; k++)level_f[j][i]->ansres[k] = 0;
			level_f[j][i]->visited = true;
		}
	double min;
	int tot = 0, tot1 = 0;
	while (!quelevel.empty() > 0) {
		//	cout << "to " << tot << " " << tot1;
		//	if (que.size() > 0)cout << " " << que.top();
			//cout << endl;
		tmp.clear();
		while (!tmpobjs.empty()) tmpobjs.pop();
		int k = quelevel.top()->level;
		while (!quelevel.empty() && quelevel.top()->level == k) {
			Object* o = quelevel.top();
			o->dist = 0;
			//	for (int j = 0; j < m; j++) if (querym[j] > 0.00001) o->dist += o->ansres[j] * querym[j];
			tmpobjs.push(o);
			quelevel.pop();
		}
		//cout << "working level " << k << endl;
		//if (!quelevel.empty()) cout << "suppose next level " << quelevel.top()->level << endl;
		priority_queue<double> distlist;
		while (!tmpobjs.empty()) {
			Object* o1 = tmpobjs.top();
			//if (o1->level > k) cout << "!!!!!!!! o1 " << o1->level  << endl;
			tmp.push_back(o1);
			tmpobjs.pop();
			int flg = 0;
			if (distlist.size() < knn || o1->dist <= distlist.top() * epsi) {
				o1->dist = 0;
				for (int j = 0; j < m; j++)if (querym[j] > 0.000001) {
					o1->dist += o1->ansres[j] * querym[j];
				}
				if (distlist.size() < knn || o1->dist <= distlist.top() * epsi) flg = 1;
			}
			if (flg == 1) {
				flg = 0;
				o1->dist = 0;
				for (int j = 0; j < m; j++)if (querym[j] > 0.000001) {
					o1->ansres[j] = o1->distance(q, j);
					o1->dist += o1->ansres[j] * querym[j];
				}
				distlist.push(o1->dist);
				//if (tmpd > o1->dist)cout << " Finding error " << tmpd << " " << o1->dist << endl;
				while (distlist.size() > knn) distlist.pop();
				if (distlist.size() < knn || o1->dist <= distlist.top() * epsi) flg = 1;
			}

			/*
			flg = 1;
			for (int j = 0; j < m; j++)if (querym[j] > 0.000001) {
				o1->ansres[j] = o1->distance(q, j);
				o1->dist += o1->ansres[j] * querym[j];
			}
			distlist.push(o1->dist);
			while (distlist.size() > knn) distlist.pop();
			*/


			if (flg == 1) {
				e_s3* p = o1->all_e;
				while (p != NULL) {
					int j = p->metric_num;
					if (querym[j] > 0.0001) {
						e_s2* p1 = p->first_level;
						while (p1 != NULL && p1->level != o1->level) p1 = p1->next_level;
						if (p1 != NULL) {
							e_s1* p2 = p1->first_edge;
							while (p2 != NULL) {
								Object* o = p2->obj;
								if (o->level > k) cout << "o1 " << o1->level << " o " << o->level << endl;
								o->ansres[j] = max(p2->obj->ansres[j], abs(p2->dis - o1->ansres[j]));
								if (o->visited == false) {

									o->visited = true;
									o->dist = o->ansres[j];
									tmpobjs.push(o);
								}
								p2 = p2->next_edge;
							}
						}
					}
					p = p->next_space;
				}
			}
		}
		min = distlist.top();
		//cout << "level is " << k << " size is " << distlist.size() << endl;
		while (distlist.size() > 0) distlist.pop();

		for (int i = 0; i < tmp.size(); i++) {
			Object* o1 = tmp[i];
			o1->visited = false;
			if (o1->dist <= min) {
				que.push(o1->dist);
				ans.push_back(o1);
				e_s3* p = o1->all_e;
				while (p != NULL) {
					int j = p->metric_num;
					if (querym[j] > 0.00001) {
						e_s2* p1 = p->first_level;
						while (p1 != NULL) {
							if (p1->level < o1->level) {

								e_s1* p2 = p1->first_edge;
								while (p2 != NULL) {
									Object* o = p2->obj;
									if (o->level >= k)cout << "o1 " << o1->level << " smaller o " << o->level << endl;
									o->ansres[j] = max(o->ansres[j], abs(p2->dis - o1->ansres[j]));
									if (o->visited == false) {
										quelevel.push(o);
										tot1++;
										o->visited = true;
									}
									p2 = p2->next_edge;
								}
							}
							p1 = p1->next_level;
						}
					}
					p = p->next_space;
				}
			}

			for (int j = 0; j < m; j++)o1->ansres[j] = 0;
		}
	}
	tmp.clear();
	while (que.size() > knn) que.pop();
	min = que.top();
	while (que.size() > 0) que.pop();
	for (int i = 0; i < ans.size(); i++) if (ans[i]->dist <= min) tmp.push_back(ans[i]);
	ans.clear();
	return tmp;
}