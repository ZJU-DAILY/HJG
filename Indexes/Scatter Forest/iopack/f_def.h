/* this file contains necessary definitions for the file simulator */
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

#ifndef _F_DEF
#define _F_DEF

//----------------------------------------------------------

#define BFHEAD_LENGTH (sizeof(int)*2)    //file header size

#define TRUE 1
#define FALSE 0

#define SEEK_CUR 1
#define SEEK_SET 0
#define SEEK_END 2

//----------------------------------------------------------
typedef char Block[];
//----------------------------------------------------------

//----------------------------------------------------------


#endif