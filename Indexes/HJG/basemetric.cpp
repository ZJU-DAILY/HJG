
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cmath>
#include "basemetric.h"
using namespace std;
double trans(double ddd1, double ddd2) {
	int dd1 = (int)ddd1;
	int dd2 = (int)ddd2;
	int y2, m2, d2;
	int y1, m1, d1;

	m1 = ((dd1 % 10000) / 100 + 9) % 12;
	y1 = dd1 / 10000 - m1 / 10;
	d1 = 365 * y1 + y1 / 4 - y1 / 100 + y1 / 400 + (m1 * 306 + 5) / 10 + (dd1 % 100 - 1);

	m2 = ((dd2 % 10000) / 100 + 9) % 12;
	y2 = (dd2 / 10000) - m2 / 10;
	d2 = 365 * y2 + y2 / 4 - y2 / 100 + y2 / 400 + (m2 * 306 + 5) / 10 + (dd2 % 100 - 1);

	return d1 - d2;
}
double trans2(double ddd1, double ddd2) {
	int dd1 = (int)ddd1;
	int dd2 = (int)ddd2;
	int y1 = dd1 / 10000, m1 = (dd1 / 100) % 100, d1 = dd1 % 100;
	int y2 = dd2 / 10000, m2 = (dd2 / 100) % 100, d2 = dd2 % 100;
	return ((y1 - y2) * 60 + m1 - m2) * 60 + d1 - d2;
}

double baseme(double* a1, double* a2, char* s1, char* s2, int m,int size) {
	double sum = 0;
	switch (metricm[m]) {

	case 0:
	case 1:
	case 4: {
		for (int i = 0; i < size; i++)
			sum += abs(a1[i] - a2[i]);
		break;
		break; }
	case 5: {
		double sa1 = 0, sa2 = 0, sa3 = 0;
		for (int i = 0; i < size; i++) {
			sa1 += a1[i] * a1[i];
			sa2 += a2[i] * a2[i];
			sa3 += a1[i] * a2[i];
		}
		sa1 = pow(sa1, 0.5);
		sa2 = pow(sa2, 0.5);
		if (sa1 * sa2 == 0) cout << "Error!!!" << endl;
		sum = sa3 / (sa1 * sa2);
		if (sum > 1) {
			sum = 0.99999999999999999;
		}
		sum = abs(acos(sum) * 180 / 3.1415926);
		//cout << "dis " << dis << endl;
		break; }
	case 2:
		for (int i = 0; i < size; i++)
			sum += abs(a1[i] - a2[i])* abs(a1[i] - a2[i]);
		sum = pow(sum, 0.5);
		break;
	case 3:
		sum += trans(a1[0], a2[0]);
		if (size> 1) sum = sum * 24 * 3600 + trans2(a1[1], a2[1]);
		sum = abs(sum);
		break;
	case 6:
	{
		int n = 0;
		int m = 0;
		for (int i = 0; s1[i]; i++) n++;
		for (int i = 0; s2[i]; i++) m++;
		if (n == 0) return m;
		if (m == 0) return n;
		int** table1 = new int* [n + 1];
		for (int i = 0; i < n + 1; i++)
			table1[i] = new int[m + 1];
		for (int i = 0; i <= n; i++) table1[i][0] = i;
		for (int j = 0; j <= m; j++) table1[0][j] = j;
		for (int i = 1; i <= n; i++) {
			for (int j = 1; j <= m; j++) {
				int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;	// ith character of s, jth character of t
				table1[i][j] = 1 + MIN(table1[i - 1][j], table1[i][j - 1]);
				table1[i][j] = MIN(table1[i - 1][j - 1] + cost, table1[i][j]);
			}
		}
		sum = table1[n][m];
		for (int i = 0; i < n + 1; i++)
			delete[] table1[i];
		delete[]table1;
		table1 = NULL;		
		break;
	}

	default:
		cout << "Metic Fail " << endl;
		break;
	}
	return sum*1.0/metricmaxdis[m];
}