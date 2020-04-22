#pragma once
#ifdef _MSC_VER
#include <conio.h>
#include <windows.h>

#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/wait.h>  
#include <unistd.h>    
#include <string.h>  
#include <stdlib.h>

#endif
#include <stdio.h>
#include <iostream>
using namespace std;

#ifdef _MSC_VER
#define MAX 1024
#define DW_MAXIMUM_SIZE_HIGH 20
#define DW_MAXIMUM_SIZE_LOW 0
void Child();
void Parent(char *);

#else 
#define SHMSIZE 20
#define UNIQENUMBER 2009
void client(char*, int);

#endif