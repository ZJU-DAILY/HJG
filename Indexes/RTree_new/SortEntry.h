#ifndef __SORTENTRY
#define __SORTENTRY
#include <string>
#include <iostream>
using namespace std;
#include"gadget\gadget.h"

extern int MAXLEVEL;

class TEntry
{
public:
 int id;
 double key;

TEntry()
{
id =-1;
key = -1;
}

 TEntry(const TEntry & se)
	{
		id = se.id;
		key = se.key;
	}

 bool operator < (const TEntry &a) const 
	{
		if(key - a.key<-0.000000001)
			return true;
		else if(fabs(key - a.key)<0.000000001)
			return id<a.id;
		else 
			return false;
	 }
};

class KEntry
{
public:
	int id;
	double key;
	bool isobject;
	int level;
	int pivot;

	KEntry()
	{
		id = -1;
		key = -1;
		level = MAXLEVEL + 1;
	}

	KEntry(const KEntry & se)
	{
		id = se.id;
		key = se.key;
		isobject = se.isobject;
		level = se.level;
		pivot = se.pivot;
	}

	bool operator < (const KEntry &a) const
	{
		if (key - a.key<-0.000000001)
			return false;
		else if (fabs(key - a.key) < 0.000000001)
		{
			if (level == a.level&&!isobject)
			{
				return pivot >a.pivot;
			}
			else
				return level<a.level;
		}			
		else
			return true;
	}
};

class HEntry
{
public:
	unsigned * key;
	int keysize;
	int id;
	HEntry(unsigned * a, int s, int i)
	{
		key = a;
		keysize = s;
		id = i;
	}

	HEntry()
	{
		key = NULL;
	}

	~HEntry()
	{
		key = NULL;
		//delete[] key;
		//key = NULL;
	}

	HEntry(const HEntry &a)
	{
		keysize = a.keysize;
		key = a.key;
		id = a.id;
		//key = new int[keysize];
		//memcpy(key,a.key, keysize * sizeof(int));
	}

	bool operator < (const HEntry &a) const 
	{
		bool flag = true;
		for(int i =0;i<keysize;i++)
		{
			if(key[i] < a.key[i])
			{
				return flag;
			}
			else if(key[i]>a.key[i])
			{
				flag = false;
				return flag;
			}		
		}
		return id<a.id;
	 }

	bool operator > (const HEntry &a) const 
	{
		bool flag = false;
		for(int i =0;i<keysize;i++)
		{
			if(key[i] < a.key[i])
			{
				break;
			}
			if(key[i]>a.key[i])
			{
				flag = true;
				break;
			}
		}
		return id>a.id;
	 }

	void print()
	{
		for(int i =0;i<keysize;i++)
		{
			cout<<key[i]<<" ";
		}
	}
};

class IntArray
{
public:
	unsigned * key;
	int keysize;
	int edist;
	IntArray(unsigned * a, int s)
	{
		key = a;
		a = NULL;
		keysize = s;
	}

	IntArray()
	{
		key = NULL;
	}

	~IntArray()
	{
		key = NULL;
		//delete[] key;
		//key = NULL;
	}

	IntArray(const IntArray &a)
	{
		keysize = a.keysize;
		key = a.key;
		edist = a.edist;
		//key = new int[keysize];
		//memcpy(key,a.key, keysize * sizeof(int));
	}

	bool operator < (const IntArray &a) const 
	{
		bool flag = true;
		for(int i =0;i<keysize;i++)
		{
			if(key[i] < a.key[i])
			{
				break;
			}
			if(key[i]>a.key[i])
			{
				flag = false;
				break;
			}
		}
		return flag;
	 }

	bool operator > (const IntArray &a) const 
	{
		bool flag = false;
		for(int i =0;i<keysize;i++)
		{
			if(key[i] < a.key[i])
			{
				break;
			}
			if(key[i]>a.key[i])
			{
				flag = true;
				break;
			}
		}
		return flag;
	 }

	void print()
	{
		for(int i =0;i<keysize;i++)
		{
			cout<<key[i]<<" ";
		}
	}
};

#endif