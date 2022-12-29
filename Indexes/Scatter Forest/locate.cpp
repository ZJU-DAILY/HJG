#include <string>
#include <cstring>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#define NUUM 2000000
using namespace std;
string loc[NUUM];
int fgg[NUUM];
int stack[NUUM],top=0;
int num,dim;
double mm;
int pnum=0;
double alpha;
ifstream ain1;
void readm(){ //re-read the file	
	string s;
	cin>>s;
	ain1.open(s+".txt", ifstream::in);	
	ain1>>dim>>num>>top;
	for(int i=0;i<num;i++){
		getline(ain1, loc[i]);
	}
	ain1.close();
}

void op(){
	
	ofstream out1;
	out1.open("test.txt");
	int p;
	
	for(int i=0;i<10000;i++){
		for(int ii=0;ii<10;ii++)
		out1<<i+ii*1000<<" ";			
		out1<<endl;		 
	}	
	out1.close();		
}
int main(){	
	op();
}
