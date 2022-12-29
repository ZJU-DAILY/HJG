#ifndef __Object
#define __Object

#include <string>
using namespace std;

#define MAX(x, y) ((x>y)? (x): (y))
extern int m;
extern int* metricm;
extern double* metricmaxdis;
class Object
{
public:
	int id;
	int sized, sizes;
	int* size;
	double* datad;
	char* datas;

	//tmpsize
	int istravel;
	double* ansres;
	double dist;


	void ini(string str);
	void outnode();
	Object();
	Object(Object* o);
	~Object();
	int write_to_buffer(char* buffer);
	int read_from_buffer(char* buffer);
	int getsize();

	double distance(Object* other, int m);

	bool operator < (const Object* a) const
	{
		if (dist - a->dist < -0.000000001)
			return false;
		else
			return true;
	}

};

#endif