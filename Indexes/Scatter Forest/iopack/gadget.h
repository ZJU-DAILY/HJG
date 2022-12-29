#ifndef __GADGET_H
#define __GADGET_H

#include <stdio.h>


//----- typedefs -----
	
typedef FILE * FILEptr;
typedef void * voidptr;

//----- macros -----

#define min(a, b)	(((a) < (b)) ? (a) : (b))
#define max(a, b)	(((a) > (b)) ? (a) : (b))
#define FLOATZERO	1e-6
#define MAXREAL		1e20
//#define MAXINT      1000
//#define EPS 14
//#define BITS 10

//----- functions -----

void	blank_print			(int _n);	
int		compfloats			(float _v1, float _v2);
void	error				(char *_msg, bool _exit);
void	get_leading_folder	(char *_path, char *_folder);				
void	getFNameFromPath	(char *_path, char *_fname);
char	getnextChar			(char **_s);
void	getnextWord			(char **_s, char *_w);
bool	is_pow_of_2			(int _v);
float	l2_dist_int			(int *_p1, int *_p2, int _dim);		
void	printINT_in_BIN		(int _n, int _m);							





#endif