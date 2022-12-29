#include "gadget.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*****************************************************************
prints an err msg 

para:
- msg: 
- exit: true if you want to exit the program

Coded by Yufei Tao, 4 aug 08
*****************************************************************/

void error(char *_msg, bool _exit)
{ 
	printf(_msg); 

	if (_exit) 
		exit(1); 
}

/*****************************************************************
compares two float numbers

para:
- v1:
- v2:

return:
- -1: v1 smaller
  0: tie
  1: v2 smaller

Coded by Yufei Tao, 31 july 08
*****************************************************************/

int compfloats(float _v1, float _v2)
{
	int ret = 0; 

	if (_v1 - _v2 < -FLOATZERO)
		ret = -1;
	else if (_v1 - _v2 > FLOATZERO)
		ret = 1;
	else ret = 0;

	return ret;
}

/*****************************************************************
checks if v is a power of 2

para:
- v

return:
- true or false

Coded by Yufei Tao, 31 july 08
*****************************************************************/

bool is_pow_of_2(int _v)
{
	int x = (int) (log( (float) _v) / log(2.0));
	int y = (int) pow(2.0, x);

	return (_v == y);
}

/*****************************************************************
get the part of a path after the last blackslash.

e.g., given path = "./ex/ex/ex.h", return "ex.h"

para
- path:			the given path. 
- (out) fname:	the returned part (usually a file name)

Coded by Yufei Tao, 4 aug 08
*****************************************************************/

void getFNameFromPath(char *_path, char *_fname)
{
	int i;
	int len = strlen(_path);
	int pos = -1;

	for (i = len - 1; i >= 0; i --)
	{
		if (_path[i] == '/')
		{
			pos = i;
			break;
		}
	}

	pos ++;

	for (i = pos; i <= len; i ++)
	{
		_fname[i - pos] = _path[i];
	}

	_fname[i - pos] = '\0';
}

/*****************************************************************
this function gets the part of the given path up to the last folder.
e.g, given ./ex/ex/1.zip, the function returns ./ex/ex/

para:
- path
- (out) folder: 

Coded by Yufei Tao, 7 aug 08
*****************************************************************/

void get_leading_folder(char *_path, char *_folder)
{
	int len = strlen(_path);
	int pos = -1;

	for (int i = len - 1; i >= 0; i --)
	{
		if (_path[i] == '/')
		{
			pos = i; 
			break;
		}
	}

	int i=0;
	for (int i = 0; i <= pos; i ++)
	{
		_folder[i] = _path[i];
	}

	_folder[i] = '\0';
}

/*****************************************************************
just gets n tabs on the screen.

para:
- n

Coded by Yufei Tao, 7 aug 08
*****************************************************************/

void blank_print(int _n)
{
	for (int i = 0; i < _n; i ++)
		printf("   ");
}

/*****************************************************************
Coded by Yufei Tao, 7 aug 08
*****************************************************************/

float l2_dist_int(int *_p1, int *_p2, int _dim)
{
	float ret = 0;
	for (int i = 0; i < _dim; i ++)
	{
		float dif = (float) (_p1[i] - _p2[i]);
		ret += dif * dif;
	}

	return ret;
}

/*----------------------------------------------------------------
DESCRIPTION:
Get the next word from a string.

PARA:

_s (in/out):		the string. At finish, _s will point to the
                    char right after the word returned.
_w (out):			the word returned.


RETURN:
the character. 

KNOWN ISSUES:
None.

AUTHOR:
Yufei Tao

LAST MODIFIED:
5 Mar. 2009.
----------------------------------------------------------------*/

void getnextWord(char **_s, char *_w)
{
	while (**_s == ' ')
	{
		(*_s)++;
	}

	while (**_s != ' ' && **_s != '\0')
	{
		*_w = **_s;
		(*_s)++; _w++;
	}

	*_w = '\0';
}

/*----------------------------------------------------------------
DESCRIPTION:
Get the next non-white character from a string.

PARA:
_s (in/out):		the string. when the function finishes, _s
					will point to the character after the one
					returned.

RETURN:
the character. 

KNOWN ISSUES:
None.

AUTHOR:
Yufei Tao

LAST MODIFIED:
5 Mar. 2009.
----------------------------------------------------------------*/

char getnextChar(char **_s)
{
	char ret;

	while (**_s == ' ')
	{
		(*_s)++;
	}

	ret = **_s;
	(*_s)++;

	return ret;
}

/*****************************************************************
print the last m bits of an integer n

-para-
n
m		

-by- 
yf tao

-last touch-
19 aug 09
*****************************************************************/

void printINT_in_BIN(int _n, int _m)
{
	int		i		= -1;
	int		mask	= -1;
	int		n		= -1;

	mask = 1 << _m;
	mask --;
	
	n = _n & mask;

	mask = 1 << (_m - 1);

	for (i = 0; i < _m; i ++)
	{
		if (mask & n)
			printf("1");
		else
			printf("0");

		mask >>= 1;
	}
	printf("\n");
}