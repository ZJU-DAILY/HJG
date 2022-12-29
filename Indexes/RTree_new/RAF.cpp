#include "RAF.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fstream>
#include ".\object.h"
#include ".\blockfile\blk_file.h"
extern int BLOCK_SIZE;


void add_fname_ext(char * _fname)
{
	strcat(_fname, ".raf");
}

void RAF::init(char *_fname, int _b_length, Cache *_c)
{
	char *fname = new char[strlen(_fname) + 10]; //allow 9 characters for extension name
	strcpy(fname, _fname);
	add_fname_ext(fname);

	FILE *fp = fopen(fname, "r");
	if (fp)
	{
		fclose(fp);
	//	printf("The file \"%s\" exists. Replace? (y/n)", fname);
	//	char c = getchar(); getchar();
	//	if (c != 'y' && c != 'Y') 
	//		error("", true);
		remove(fname);
	}

    file = new BlockFile(fname, _b_length);
    cache = _c;

	delete []fname;
}

void RAF::init_restore(char *_fname, Cache *_c)
{
	char *fname = new char[strlen(_fname) + 10]; //allow 9 characters for extension name
	strcpy(fname, _fname);
	add_fname_ext(fname);

	//--=== testing if the file exists ===--
	FILE *fp = fopen(fname, "r");
	if (!fp)
	{
		printf("RAF file %s does not exist\n", fname);
		delete []fname;
		//error("", true);
		system("pasue");
	}

	fclose(fp);
	//--=== done testing the file existence ===--

    file = new BlockFile(fname, BLOCK_SIZE);
    cache = _c;

	delete [] fname;
}

int* RAF::buid_from_array(Object ** objset, int * order)
{
	char * buffer;
	int * ptrset = new int[num_obj];

	int i = 0;
	int j = 0;
	bool head = true;
	
	while(j<num_obj)
	{
		buffer = new char[file->blocklength];
		if (head)
		{
			i = 2*sizeof(int);
			head = false;
		}
		else
			i=0;
		while(i+objset[order[j]]->getsize()<file->blocklength && j< num_obj)
		{
			//printf("%d %d %f\n",j, objset[order[j]]->id, objset[order[j]]->data[0]);
			ptrset[j] = file->number * file->blocklength +i;
			objset[order[j]]->write_to_buffer(&buffer[i]);
			i += objset[order[j]]->getsize();
			j++;
			if(j>=num_obj)
				break;
		}
		file->append_block(buffer);
		delete[] buffer;
		buffer = NULL;
	}
	return ptrset;
}

int RAF::add_object(Object* obj) {
	char* buffer = new char[file->blocklength];
	int i = last_ptr, sizeo = obj->getsize();
	int fnum = last_ptr / file->blocklength;
	//cout << "trying " << last_ptr << " fnum " << fnum << endl;
//	cout << "number " << file->number << endl;
	if (last_ptr % file->blocklength + sizeo < file->blocklength) {
		i = last_ptr;
		file->read_block(buffer, fnum);
		obj->write_to_buffer(&buffer[last_ptr % file->blocklength]);
		file->write_block(buffer, fnum);		
	}
	else {
		i = (fnum + 1) * file->blocklength;
		obj->write_to_buffer(buffer);
		file->append_block(buffer);
	}
	last_ptr = i + sizeo;
	delete[] buffer;
	buffer = NULL;
	return i;
}