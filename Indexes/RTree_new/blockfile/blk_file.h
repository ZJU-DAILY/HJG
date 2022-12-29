/*blk_file.h:
  this file specifies the class BlockFile, a block file simulator
*/
#ifndef __BLK_FILE
#define __BLK_FILE

//----------------------------------------------------------
#include <stdio.h>
#include "../func/gendef.h"
//----------------------------------------------------------
class BlockFile
{
public:
   FILE* fp;			// os file pointer
   char* filename;		 
   int blocklength;	    // length of a block
   int act_block; 	    // block # of fp's position (fp can stay at block boundaries)
   int number;		    // total # of blocks
   bool new_flag;		// specifies if this is a new file

     //=====================================================
   BlockFile(char* name, int b_length);
   			        // Filename und Blocklaenge (fuer neue Files)
   ~BlockFile();

   void put_bytes(char* bytes,int num)
		{ fwrite(bytes,num,1,fp); 
	      /*char *add;
		  add = bytes;
	      for (int i = 0; i < num; i ++) 
		  { fputc(*add, fp); add ++; }*/}

   void get_bytes(char* bytes,int num)	     
		{ fread(bytes,num,1,fp); 
       	  /*char *add;
		  add = bytes;
	      for (int i = 0; i < num; i ++) 
		  { *add = fgetc(fp); add ++; }*/}

   void fwrite_number(int num);	

   int fread_number();		

   void seek_block(int bnum)    
		{ fseek(fp,(bnum-act_block)*blocklength,SEEK_CUR); }

   void read_header(char * header);

   void set_header(char* header);
   					
   bool read_block(Block b,int i);	

   bool write_block(Block b,int i);

   int append_block(Block b);	

   bool delete_last_blocks(int num);

   bool file_new()			
		{ return new_flag; }

   int get_blocklength()	
		{ 
	       return blocklength; }

   int get_num_of_blocks()	
		{ return number; }
};

//-------------------------------------------------------------------------
class CachedBlockFile : public BlockFile
{
public:
   enum uses {free,used,fixed};
   int ptr;		        // current position in cache
   int cachesize;		// //the number of blocks kept in memory
   int page_faults;     

   int *cache_cont;	    // array of the indices of blocks that are in cache
   uses *fuf_cont; 		// indicator array that shows whether one cache block is free, used or fixed
   int  *LRU_indicator; // indicator that shows how old (unused) is a page in the cache
   bool  *dirty_indicator;  // indicator that shows if a page has been modified

   char **cache;   		// Cache

	//=====================================================

   CachedBlockFile(char* name, int blength, int csize);
   					
   ~CachedBlockFile();

   int next();		

   int in_cache(int index);	// liefert Pos. im Cache oder -1

   bool read_block(Block b,int i);

   bool write_block(Block b,int i);

   bool fix_block(int i);

   bool unfix_block(int i);

   void unfix_all();			

   void set_cachesize(int s);

   void flush();
};


#endif // __BLK_FILE
