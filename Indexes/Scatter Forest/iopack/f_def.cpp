/* this file contains the functions useful for b-tree */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "f_def.h"
#include "gadget.h"

//-----------------------------------------------
//does the same thing as "fopen", but displays an error message if failed

//fname: 
//c: 'r' for reading, and 'w' for writing

FILE *openfile(char *_fname, char _c)
{
	FILE *fp = NULL;

	if (_c == 'r')
		fp = fopen(_fname, "r");
	else
	if (_c == 'w')
		fp = fopen(_fname, "w");
	else
		error("error in the 2nd parameter in calling opefile", true);

	if (!fp)
	{
		printf("could not open file %s\n", _fname);
		exit(1);
	}

	return fp;
}

