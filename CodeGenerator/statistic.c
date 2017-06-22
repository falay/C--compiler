#include "statistic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

table IDtable ;

void tableInitializer(int capacity)
{
	IDtable.data = (pair*)malloc( sizeof(pair)*capacity ) ;
	IDtable.size = 0 ;
	IDtable.capacity = capacity ;
}


void tableInserter(char* ID)
{
	for(int i=0; i<IDtable.size; i++)
	{
		if( strcmp(IDtable.data[i].identifier, ID) == 0 )
		{	
			IDtable.data[i].frequency ++ ;
			return ;
		}	
	}	
	
	if( IDtable.size == IDtable.capacity )
	{
		IDtable.capacity <<= 1 ;
		IDtable.data = (pair*)realloc( IDtable.data, sizeof(pair)*IDtable.capacity ) ;
	}
		
	pair newID ;
	newID.identifier = (char*)malloc( sizeof(char)*(strlen(ID)+1) ) ;	
	strcpy( newID.identifier, ID ) ;
	newID.frequency = 1 ;		
	IDtable.data[ IDtable.size++ ] = newID ; 
}

void tablePrinter()
{
	printf("frequencies of identifiers:\n") ;
	for(int i=0; i<IDtable.size; i++)
	{	
		printf("%s\t%d", IDtable.data[i].identifier, IDtable.data[i].frequency) ;	
		if( i < IDtable.size-1 )
			printf("\n") ;
	}	
}

void tableFreer(table* IDtable)
{
	free( IDtable->data )  ;
		
	IDtable->size = IDtable->capacity = 0 ;
	IDtable->data = NULL ;
}
