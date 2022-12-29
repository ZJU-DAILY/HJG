#ifndef _RAF
#define _RAF
class BlockFile;
class Cache;
class Object;
#include "blockfile\cache.h"

class RAF: public Cacheable
{
public:
	int num_obj; // the total number of objects
	int last_ptr;
	void init(char *_fname, int _b_length, Cache *_c);
	void init_restore(char *_fname, Cache *_c);
	int add_object(Object* obj);
	int* buid_from_array(Object** objset, int * order);
};

#endif