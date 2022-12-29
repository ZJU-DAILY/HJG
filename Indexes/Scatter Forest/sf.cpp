#include <stdio.h>
#include <memory.h>


#include <sstream>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include "sf.h"
#include "bpnode.h"
#include "bptree.h"

Treearr::Treearr()
{
	len = 0;
	tot = 0;
	tree = -1;
	metric = -1;
	datas = NULL;
	datad = NULL;
}

Treearr::Treearr(Object* o, int m) {
	//o->outnode();
	len = 0;
	tree = -1;
	tot = 0;
	metric = m;
	datas = NULL;
	datad = NULL;
	switch (metricm[m]) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		//		cout<<"Object size " << o->size[m * 2 + 1] - o->size[m * 2] << endl;
		datad = new float[o->size[m * 2 + 1] - o->size[m * 2]];
		len = o->size[m * 2 + 1] - o->size[m * 2];
		//cout << "Creating Mnode 0/1 " << sizeof(datad[0]) << " " << sizeof(float)  << endl;
		for (int i = o->size[m * 2]; i < o->size[m * 2 + 1]; i++)datad[i - o->size[m * 2]] = o->datad[i];
		//cout << "Creating Mnode 0/1 " << sizeof(datad) << " " << sizeof(float) << " " << datad[0] << endl;
		break;
	case 6:
		datas = new char[o->size[m * 2 + 1] - o->size[m * 2]];

		len = o->size[m * 2 + 1] - o->size[m * 2];
		for (int i = o->size[m * 2]; i < o->size[m * 2 + 1]; i++)datas[i - o->size[m * 2]] = o->datas[i];

		break;
	}	
}

void Treearr::outnode()
{
	cout <<"Tot "<<tot<<" Tree "<< tree << " metric " << metricm[metric] << " len " << len << " *";
	switch (metricm[metric]) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		for (int k = 0; k < len; k++) cout<<datad[k]<<"*";
		break;
	case 6:
		cout << datas << "*";
		break;
	}
	cout << endl;
}

Treearr::~Treearr() {
	if (metric < -1 || metric>m) {
		cout<<"===== Treearr error ======="<<endl;
	}
	if (len > 0) {
		switch (metricm[metric]) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			if (datad != NULL) delete[] datad;
			break;
		case 6:
			if (datas != NULL) delete[] datas;
			break;
		}
	}
}

void SF::readfrom(string s)
{
	IOread++;
	ifstream in1;
	in1.open(s);
	int k;
	in1 >> k;
	getline(in1, s);
	for (int i = 0; i < k; i++) {
		Treearr* tmp = new Treearr();
		in1 >> tmp->tot;
		in1 >> tmp->tree;
		in1 >> tmp->metric;
		in1 >> tmp->len;
		switch (metricm[tmp->metric]) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			tmp->datad = new float[tmp->len];
			for (int k = 0; k < tmp->len; k++) in1 >>tmp->datad[k];
			break;
		case 6:			
			tmp->datas = new char[tmp->len];
			//in1 >> tmp->datas[0];
			for (int k = 0; k < tmp->len; k++) in1>> tmp->datas[k];
			break;
		}
		arr.push_back(tmp);
	}
	in1.close();
	
}

void SF::writeto(string s)
{
	IOwrite++;
	ofstream ou1;
	ou1.open(s);
	ou1 << arr.size()<<endl;
	for (int j = 0; j < arr.size(); j++) {
		ou1 << arr[j]->tot << " ";
		ou1 << arr[j]->tree << " ";
		ou1 << arr[j]->metric << " ";
		ou1 << arr[j]->len;
		switch (metricm[arr[j]->metric]) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			for (int k = 0; k < arr[j]->len; k++) ou1 <<" "<< arr[j]->datad[k];			
			break;
		case 6:
			ou1 << " ";
			for (int k = 0; k < arr[j]->len; k++) ou1 << arr[j]->datas[k];		
			break;
		}
		ou1 << endl;
	}
	ou1.close();
}
SF::SF() {

}

SF::~SF() {
	for (int i = 0; i < arr.size(); i++) delete arr[i];
	arr.clear();
}

vector<AnsLeaf*> SF::rnn(Object* que, int metric, float r,int fgg) {
	vector<AnsLeaf*> tmp;
	int m = arr.size();
	bpnode* q = new bpnode(metric, que);
	//cout << "============================ Range " << r << " metric " << metric << " query : " ;
//	que->outnode();
	for (int i = 0; i < m; i++) {
		vector <bptree*> queue;
		bptree* pt = new bptree();
		pt->loadroot(arr[i]->tree);
		queue.push_back(pt);
		//pt->outnode("new ");
	//	cout << "size " << m << endl; //============================================================
		float dis = baseme(q->datad, arr[i]->datad, q->datas, arr[i]->datas,metric, q->len);		
		//cout << "dis " << metric << " " << dis << endl;
		while (queue.size() > 0) {
			pt = queue[queue.size() - 1];
			queue[queue.size() - 1] = NULL;
			queue.pop_back();
			//pt->outnode("this");
			//	cout << "delpiv 1" << endl;
			//cout << "leafs " << pt->leafnum << endl; //============================================================
			for (int i = 0; i < pt->leafnum; i++) {
				bpnode* pson = pt->son[i];
				if (pson->disl > dis + r || pson->disr < dis - r) continue;
				//cout<<r<<'\t'<<dis<<'\t'<<pson->disl<<'\t'<<pson->disr<< endl; //============================================================
				if (pt->isleaf == 1) {
					float dis1 = baseme(q->datad, pson->datad, q->datas, pson->datas,metric, q->len);
					if (dis1 <= r) {
						AnsLeaf* qq = new AnsLeaf(pson->objloc, q->metric, dis1);
						qq->loc = pt->block;
						tmp.push_back(qq);
					//	cout << "Finded "<<dis1 << endl;
					/*	Object* q = Obj_f->get_object(pson->objloc);
						 ";
						//q->outnode();
						delete q;
						*/
					}
				}
				else {
					bptree* p = new bptree();
					//cout << "doing 1 " << pt->son[i]->son << endl;
					p->loadroot(pt->son[i]->son);
					if (fgg == 1) {
						if (p->fa != pt->block) {
							p->fa = pt->block;
							p->dirty = 1;
						}
					}
					queue.push_back(p);
				}
			}
			delete pt;
		}		
	}
	delete q;
//	cout << "======== RNN range = "<< r <<" size = " << tmp.size() << endl;
	return tmp;
}

AnsLeaf::AnsLeaf(int i, int j, float k) {
	metric = j;
	obj = i;
	dis = k;
	o = NULL;
}
AnsLeaf::~AnsLeaf() {
}
float disref(float disl,float disr,float dis) {
	float aa = disl - dis;
	float ab = disr - dis;	
	if (aa * ab <= 0) return 0;
	else return min(abs(aa), abs(ab));
}

vector<AnsLeaf*> SF::knn(Object* que, int metric, int k) {
	int m = arr.size();
	vector<AnsLeaf*> tmp;
	tmp.clear();
	vector<AnsLeaf*> res;
	res.clear();
	bpnode* q = new bpnode(metric, que);
	struct cmp {
		bool operator() (bptree* a, bptree* b) {
			if (a->dis > b->dis) return 1;
			else return 0;
		}
	};
	struct cmp1 {
		bool operator() (float a, float b) {
			if (a < b) return 1;
			return 0;
		}
	};

	priority_queue<bptree*, vector<bptree*>, cmp> candlist;
	priority_queue<float, vector<float>, cmp1> reslist;
	
	for (int i = 0; i < m; i++) {		
		bptree* pt = new bptree();
		pt->loadroot(arr[i]->tree);		
		float tmpdis = baseme(q->datad, arr[i]->datad, q->datas, arr[i]->datas,metric, q->len);		
		float disl = pt->son[0]->disl;
		float disr = pt->son[pt->leafnum-1]->disr;
		pt->tmpdis = tmpdis;
		pt->dis = disref(disl,disr,pt->tmpdis);
		candlist.push(pt);
	}

	while (candlist.size() > 0) {
		bptree* p = candlist.top();
		candlist.pop();
		if (reslist.size() >= k) {
			while (reslist.size() > k) {//cout << "poping " << reslist.top() << endl;
				reslist.pop();
			}
			if (p->dis > reslist.top()) {
				delete p;
				continue;
			}
		}
		for (int i = 0; i < p->leafnum; i++) {
			bpnode* pson = p->son[i];
			float dis= disref(pson->disl, pson->disr, p->tmpdis);
			if (reslist.size() >= k) {
				if (dis > reslist.top()) continue;
			}
			if (p->isleaf == 1) {				
				float dis1 = baseme(q->datad, pson->datad, q->datas, pson->datas, metric, q->len);
				AnsLeaf* qq = new AnsLeaf(pson->objloc, q->metric, dis1);
				qq->loc = p->block;
				tmp.push_back(qq);
				reslist.push(dis1);				
			}
			else {
				bptree* p1 = new bptree();
				//cout << "doing 1 " << pt->son[i]->son << endl;
				p1->loadroot(p->son[i]->son);
				p1->tmpdis = p->tmpdis;
				p1->dis = dis;
				candlist.push(p1);				
			}
		}
		delete p;			
	}
	delete q;

	while (reslist.size() > k) reslist.pop();
	float dis = reslist.top();
	for (int i = 0; i < tmp.size(); i++) {
		AnsLeaf* p = tmp[i];
		if (p->dis <= dis) {
			res.push_back(p);
			  /*
			Object* q = Obj_f->get_object(p->obj);
			
			q->outnode();
			delete q;
			  */
			//cout << "Finded " << p->dis << " ";
		}
		else {
			delete tmp[i];
		}
		tmp[i] = NULL;
	}
	tmp.clear();
	//cout << "knn dis = " << dis << " num " << res.size() << endl;
	return res;
}
