#include "object.h"
#include <stdio.h>
#include <memory.h>
#include <cstring>

#include <math.h>
#include <sstream>
#include <iostream>




Object::Object()
{
	datad = NULL;
	size = NULL;
	datas = NULL;
	ansres = NULL;	
	visited = false;
	metric_f = NULL;
	edges_no = NULL;
	edges_same = NULL;
	dist = 0;
}
void Object::ini(string str) {
	stringstream sstr1(str);
//	cout << "we " << str << endl;
	string s1;
	double d1;	
	sized = 0;
	sizes = 0;
	ansres = new double[m];

	size = new int[m * 2];
//	cout << "m " << m << endl;
	for (int i = 0; i < m; i++) {	
		switch (metricm[i]) {
			case 0:
			case 3:				
			case 1:
			case 2:
			case 4:
			case 5:
				sstr1 >> s1;				
			//	cout << "1 " << s1 << endl;
				sized ++;
				for (int k = 0; s1[k]; k++) if (s1[k] == ',')sized++;									
				break;
			case 6:
				sstr1 >> s1;
			//	cout << "2 " << s1 << endl;
				sizes += s1.length()+1;
				break;
			default:
				cout << "Object reading data error!!!! " << endl;
		}
	}	
	datad = new double[sized];
	datas = new char[sizes];
	ansres = new double[m];

	//cout << "sized sizes " << sized << " " << sizes << endl;
	stringstream sstr2(str);
	sizes = 0;
	sized = 0;
	for (int j = 0; j < m; j++) {		
		switch (metricm[j]) {		
		case 0:			
		case 3:			
		case 1:
		case 2:
		case 4:
		case 5:
		{
			size[j * 2] = sized;
			sstr2 >> s1;		
			sized++;
			for (int k = 0; s1[k]; k++)
				if (s1[k] == ',') {
					s1[k] = ' ';
					sized++;
				}
			size[j * 2+1] = sized;
			stringstream sstr3(s1);
			for (int i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				sstr3 >> datad[i];
			}			
			break;		
		}
		case 6:
			size[j * 2] = sizes;
			sstr2 >> s1;
			sizes += s1.length()+1;
			size[j * 2+1] = sizes;
			for (int i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				datas[i]=(char)s1[i- size[j * 2]];
			}
			break;
		}			
		ansres[j] = -1;
	}
}

int Object::count()
{
	int s = 0;
	e_s3* p = all_e;
	while (p != NULL) {		
		e_s2* p1 = p->first_level;
		while (p1 != NULL) {
			e_s1* p2 = p1->first_edge;
			while (p2 != NULL) {
				s++;
				p2 = p2->next_edge;
			}			
			p1 = p1->next_level;
		}
		p = p->next_space;
	}
	return s;
}
int Object::count(int i)
{
	int s = 0;
	e_s3* p = all_e;
	while (p != NULL && p->metric_num != i)  p = p->next_space;
	if(p!=NULL){
		e_s2* p1 = p->first_level;
		while (p1 != NULL) {
			e_s1* p2 = p1->first_edge;
			while (p2 != NULL) {
				s++;
				p2 = p2->next_edge;
			}
			p1 = p1->next_level;
		}		
	}
	return s;
}

void Object::outnode()
{
	cout << " id " << id;
	cout << " data :";
	for (int j = 0; j < m; j++) {
		switch (metricm[j]) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			for (int i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				cout<<datad[i]<<",";
			}
			cout << " # ";
			break;		
		case 6:
			for (int i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				cout <<datas[i];
			}
			cout << " # ";
			break;
		}
	}
	cout << endl;
	return;
	e_s3* p = all_e;
	while (p != NULL) {
		cout << "===== metric: " << p->metric_num << endl;
		e_s2* p1 = p->first_level;		
		while (p1 != NULL) {
			cout << "=====     level: " << p1->level << endl;
			e_s1* p2 = p1->first_edge;
			while (p2 != NULL) {
				cout <<"            id "<<p2->obj->id<<" dis "<< p2->dis<< " level " << p2->obj->level<<endl;
				p2 = p2->next_edge;

			}
			cout << endl;
			p1 = p1->next_level;

		}

		p = p->next_space;

	}
}
Objectarr::Objectarr()
{
	n = 0;
	m = 0;
}
Objectarr::~Objectarr()
{
	if (arr != NULL)
		delete[] arr;
}
void Objectarr::readfile(string filedata)
{
	ifstream in1;
	ofstream ou1, ou2, ou3;
	n = 0;
	m = 10000;
	string str;
	{
		char sk1[10000];
		sprintf(sk1, "%s", filedata.c_str());
		in1.open(sk1);
		
	}
	//in1.open("testmulti.txt");
	//getline(in1, str);
//	ou1.open("tmp.txt");
	in1 >> n >> m;
//	n = 20;
	metricm = new int[m];
	metricmaxdis = new double[m];
	for (int i = 0; i < m; i++) in1 >> metricm[i];
	for (int i = 0; i < m; i++) in1 >> metricmaxdis[i];
	arr = new Object * [n + 1];
	cout << n << " " << m << endl;	
	getline(in1, str);
	int maxsize = 0;
	for (int i = 0; i < n; i++) {
		getline(in1, str);
		//cout << "trying " << str << endl;
		arr[i] = new Object();
//		cout << "new finish " << str << endl;
		arr[i]->ini(str);
		arr[i]->id = i;		
		//arr[i]->outnode();		
	}
	metricmeandis=new double[m];
	for (int j = 0; j < m; j++) {
		metricmeandis[j] = 0;
		for (int i = 0; i < 1000; i++) {
			int p = rand()%n;
			int q = rand() % n;
			metricmeandis[j] += arr[p]->distance(arr[q], j);
		}
		metricmeandis[j]/= metricmaxdis[j]*1000;
	}
	cout << "Max size is "<<maxsize << endl;
	ou1.close();
}

double Object::distance(Object* other, int  m)
{
	//cout<<endl << "==========  distance " <<m<< endl;
	double dis = 0;
	switch (metricm[m]) {
	case 0:
	case 1:
	case 4: {
		for (int i = size[m * 2]; i < size[m * 2 + 1]; i++) {
			dis += abs(datad[i] - other->datad[i]);
		}
		break; }
	case 5: {
		double sa1 = 0, sa2 = 0, sa3 = 0;

		for (int i = size[m * 2]; i < size[m * 2 + 1]; i++) {
			sa1 += datad[i] * datad[i];
			sa2 += other->datad[i] * other->datad[i];
			sa3 += datad[i] * other->datad[i];
		}
		sa1 = pow(sa1, 0.5);
		sa2 = pow(sa2, 0.5);
		if (sa1 * sa2 == 0) cout << "Error!!!" << endl;
		dis = sa3 / (sa1 * sa2);
		if (dis > 1) {
			dis = 0.99999999999999999;
		}
		dis = abs(acos(dis) * 180 / 3.1415926);
		//cout << "dis " << dis << endl;

		break; }
	case 2: {

		for (int i = size[m * 2]; i < size[m * 2 + 1]; i++) {
			dis += abs(datad[i] - other->datad[i]) * abs(datad[i] - other->datad[i]);
		}
		dis = pow(dis, 0.5);
		break;
	}
	case 3: {
		int st = size[m * 2];
		dis += trans(datad[st], other->datad[st]);
		if (size[m * 2 + 1] - st > 1) dis = dis * 24 * 3600 + trans2(datad[st + 1], other->datad[st + 1]);
		dis = abs(dis);
		break;
	}

	case 6: {
		int n = 0;
		int m = 0;
		for (int i = 0; datas[i]; i++) n++;
		for (int i = 0; other->datas[i]; i++) m++;
		if (n == 0) return m;
		if (m == 0) return n;
		int** table1 = new int* [n + 1];
		for (int i = 0; i < n + 1; i++)
			table1[i] = new int[m + 1];
		for (int i = 0; i <= n; i++) table1[i][0] = i;
		for (int j = 0; j <= m; j++) table1[0][j] = j;
		for (int i = 1; i <= n; i++) {
			for (int j = 1; j <= m; j++) {
				int cost = (datas[i - 1] == other->datas[j - 1]) ? 0 : 1;	// ith character of s, jth character of t
				table1[i][j] = 1 + MIN(table1[i - 1][j], table1[i][j - 1]);
				table1[i][j] = MIN(table1[i - 1][j - 1] + cost, table1[i][j]);
			}
		}
		dis = table1[n][m];
		for (int i = 0; i < n + 1; i++)
			delete[] table1[i];
		delete[]table1;
		table1 = NULL;
		break;
	}
	}

	return dis * 1.0 / metricmaxdis[m];
}
Object::~Object()
{

	if (ansres != NULL)
		delete[]ansres;
	if (datad != NULL)
		delete[] datad;
	if (datas != NULL)
		delete[] datas;
	if (size != NULL)
		delete[] size;
	datad = NULL;
	delete edges_no;
	delete edges_same;
	edges_no = NULL;
	edges_same = NULL;
	datad = NULL;
	ansres = NULL;
	datas = NULL;
	size = NULL;
	if (all_e != NULL) {
		e_s3* p = all_e;		
		while (p != NULL) {
			e_s2* p1 = p->first_level;
			while (p1 != NULL) {
				e_s1* p2 = p1->first_edge;
				while (p2 != NULL) {
					e_s1* q2 = p2;
					p2 = p2->next_edge;
					delete q2;
				}
				p1->first_edge = NULL;
				e_s2* q1 = p1;
				p1 = p1->next_level;
				delete q1;
			}
			p->first_level = NULL;
			e_s3* q = p;			
			p = p->next_space;
			delete q;
		}
		all_e = NULL;
	}
}

void Object::add_e(Object* o, double dis, int metric,int level1)
{	
	/*if (o->id == 135) {
		cout << "=========================!!!!!!!!!!!!!!!!!!!! insert id: " << o->id << " metric: " << metric << " level: " << level1 <<" real level: "<<o->level<< endl;
		system("pause");
	}
	*/
	e_s3* p = all_e;
	while (p != NULL && p->metric_num != metric) p = p->next_space;
	if (p == NULL) {
		p = new e_s3;
		p->metric_num = metric;
		p->first_level = NULL;
		p->next_space = all_e;
		all_e = p;
	}
	e_s2* p1 = p->first_level;
	while (p1 != NULL && p1->level != level1) p1 = p1->next_level;
	if (p1 == NULL) {
		p1 = new e_s2;
		p1->level = level1;
		p1->first_edge = NULL;
		p1->next_level = p->first_level;
		p->first_level = p1;
	}
	e_s1* p2 = p1->first_edge;
	while (p2 != NULL && p2->obj != o) p2 = p2->next_edge;
	if (p2 == NULL) {
		p2 = new e_s1;
		p2->dis = dis;
		p2->obj = o;
		p2->next_edge = p1->first_edge;
		p1->first_edge = p2;
	}
	if (level == level1) edges_same[metric] ++;
	else if (level == level1 + 1)edges_no[metric] ++;
}

void Object::remove_e(Object* o, double dis, int metric,int level1)
{
	e_s3* p = all_e;
	while (p != NULL && p->metric_num != metric) p = p->next_space;
	if (p == NULL)return;		
	e_s2* p1 = p->first_level;
	while (p1 != NULL && p1->level != level1) p1 = p1->next_level;
	if (p1 == NULL) return;
	e_s1* p2 = p1->first_edge;
	if (p2->obj == o) {
		p1->first_edge = p2->next_edge;
		delete p2;
	}
	else {
		while (p2->next_edge != NULL && p2->next_edge->obj != o) {
			p2 = p2->next_edge;
		}
		if (p2->next_edge != NULL) {
			e_s1* p3 = p2->next_edge;
			p2->next_edge = p3->next_edge;
			delete p3;
		}
		/*else {
			cout << "=========================!!!!!!!!!!!!!!!!!!!! delete fault " << o->id << " metric " << metric << " level " << level1 << endl;
			system("pause");
		}
		*/
	}
	if (level == level1) edges_same[metric] --;
	else if (level == level1 + 1)edges_no[metric] --;
}






















