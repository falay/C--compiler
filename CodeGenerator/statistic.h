#ifndef STATISTIC_H
#define STATISTIC_H

typedef struct pair
{
	char* identifier ;
	int frequency ;
	
} pair ;

typedef struct table
{
	pair* data ;
	int size ;
	int capacity ;	
} table ;

extern table IDtable ;

void tableInserter(char*) ;
void tableInitializer(int) ;
void tablePrinter() ;
void tableFreer(table*) ;

#endif