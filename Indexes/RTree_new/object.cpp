#include "object.h"
#include <stdio.h>
#include <memory.h>
extern double compdists;
#include <math.h>
#include <sstream>
#include <iostream>
#include "basemetric.h"
#ifndef MIN
#define MIN(x, y) ((x<y)? (x): (y))
#define MAX(x, y) ((x>y)? (x): (y))
#endif
int ** table1 = NULL;
extern int func;

Object::Object()
{
	datad = NULL;
	size = NULL;
	datas = NULL;
	ansres = NULL;

	istravel = 0;
	dist = 0;
}
void Object::ini(string str) {
	stringstream sstr1(str);
	//	cout << "we " << str << endl;
	string s1;
	double d1;
	sized = 0;
	sizes = 0;

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
			sized++;
			for (int k = 0; s1[k]; k++) if (s1[k] == ',')sized++;
			break;
		case 6:
			sstr1 >> s1;
			//	cout << "2 " << s1 << endl;
			sizes += s1.length() + 1;
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
			size[j * 2 + 1] = sized;
			stringstream sstr3(s1);
			for (int i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				sstr3 >> datad[i];
			}
			break;
		}
		case 6:
			size[j * 2] = sizes;
			sstr2 >> s1;
			sizes += s1.length() + 1;
			size[j * 2 + 1] = sizes;
			for (int i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				datas[i] = (char)s1[i - size[j * 2]];
			}
			break;
		}
		ansres[j] = -1;
	}
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
				cout << datad[i] << ",";
			}
			cout << " # ";
			break;
		case 6:
			for (int i = size[j * 2]; i < size[j * 2 + 1]; i++) {
				cout << datas[i];
			}
			cout << " # ";
			break;
		}
	}
	cout << endl;
	/*
	cout<< " disres :";
	for (int i = 0; i < m; i++)
		cout << " " << ansres[i];
	cout << endl<<endl;
	*/
}

Object::Object(Object* o)
{
	char* buf = new char[o->getsize() + 2];
	o->write_to_buffer(buf);
	read_from_buffer(buf);
	delete[]buf;

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
	ansres = NULL;
	datas = NULL;
	size = NULL;

}
int Object::getsize()
{
	//return size * sizeof(double) + 2 * sizeof(int);

	int i = 0;
	i += sizeof(id);
	i += sizeof(sized);
	i += sizeof(sizes);
	//	i += sizeof(dist);
	//	i += sizeof(istravel);
	i += sized * sizeof(double);
	i += sizes * sizeof(char);
	//	i += m * sizeof(double);
	i += 2 * m * sizeof(int);

	return i;
}
int Object::read_from_buffer(char* buffer)
{
	int i;
	memcpy(&id, buffer, sizeof(id));
	i = sizeof(id);

	memcpy(&sized, &buffer[i], sizeof(sized));
	i += sizeof(sized);

	memcpy(&sizes, &buffer[i], sizeof(sizes));
	i += sizeof(sizes);

	//	ansres = new double[m];
	//	memcpy(ansres, &buffer[i], m * sizeof(double));
	//	i += m * sizeof(double);

	size = new int[2 * m];
	memcpy(size, &buffer[i], 2 * m * sizeof(int));
	i += m * 2 * sizeof(int);

	datad = new double[sized];
	memcpy(datad, &buffer[i], sized * sizeof(double));
	i += sized * sizeof(double);
	//	cout << "start i " << i << endl;

	datas = new char[sizes];
	memcpy(datas, &buffer[i], sizes * sizeof(char));
	i += sizes * sizeof(char);
	//cout << "end i " << i <<" "<<getsize()<< endl;
	return i;
}
int Object::write_to_buffer(char* buffer)
{
	int i;

	memcpy(buffer, &id, sizeof(id));
	i = sizeof(id);

	memcpy(&buffer[i], &sized, sizeof(sized));
	i += sizeof(sized);

	memcpy(&buffer[i], &sizes, sizeof(sizes));
	i += sizeof(sizes);

	//	memcpy(&buffer[i], ansres, m * sizeof(double));
	//	i += m * sizeof(double);

	memcpy(&buffer[i], size, 2 * m * sizeof(int));
	i += 2 * m * sizeof(int);

	memcpy(&buffer[i], datad, sized * sizeof(double));
	i += sized * sizeof(double);

	memcpy(&buffer[i], datas, sizes * sizeof(char));
	i += sizes * sizeof(char);
	//cout << "end i " << i << " " << getsize() << endl;
	return i;
}

double Object::distance(Object* other, int  m)
{
	//cout<<endl << "==========  distance " <<m<< endl;
	double dis = 0;
	compdists++;
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

	return dis *1.0/ metricmaxdis[m];
}




