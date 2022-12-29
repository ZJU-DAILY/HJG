#ifndef __CACHE
#define __CACHE

#include "f_def.h"
//----------------------------------------------------------------
class Cache;
class BlockFile;

class Cacheable   //inherit this class if you wish to design an external
                  //memory structure that can be cached
{
public:
	BlockFile *file;
	Cache *cache;
};

//----------------------------------------------------------------
class Cache
{
public:
   enum uses {free,used,fixed};	// for fuf_cont
   int ptr;		        //current position in cache
   int cachesize;		//the number of blocks kept in memory
   int blocklength;
   int page_faults;
   int *cache_cont;	    // array of the indices of blocks that are in cache
   Cacheable **cache_tree;  // array of ptrs to the correspondent Cacheables where the blocks belong to
   uses *fuf_cont; 		//indicator array that shows whether one cache block is free, used or fixed
   int  *LRU_indicator; //indicator that shows how old (unused) is a page in the cache
   bool *dirty_indicator;  //indicator that shows if a cache page has been written
	   
   char **cache;   		// Cache

	//=====================================================
   
   int next();		

   int in_cache(int index, Cacheable *rt);

   Cache(int csize, int blength);

  

   ~Cache();

   bool read_block(Block b, int i, Cacheable *rt);

   bool write_block(Block b, int i, Cacheable *rt);

   bool fix_block(int i, Cacheable *rt);

   bool unfix_block(int i, Cacheable *rt);

   void unfix_all();

   void set_cachesize(int s);

   void flush();		
   void clear();	
};


#endif // __CACHE
