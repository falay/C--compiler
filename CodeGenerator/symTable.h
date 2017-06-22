#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <math.h>
#define NAMELEN 32
#define DIMLEN 256
#define PARALEN 256
#define BUFLEN 256
#define DECL 1
#define DEFINE 2

extern int curScope ;


/* structures */

typedef enum
{
	function, parameter, variable, constant
	
} KindType ;


typedef enum
{
	Int, Double, Float, String, Bool, Void, None
	
} TYPE ;


typedef struct Type
{
	TYPE basicType ;
	int dimension[DIMLEN] ;	
	int dimLen ;
	
} Type ;


typedef struct param
{
	Type paramList[PARALEN] ; 
	int len ; /* for paramList */	
} Param ;


typedef struct attribute
{
	int flag ; /* 0: paramList, 1: int , 2: float, 3: string, 4: scientfic, 5: bool, -1: no attribute */
	
	union
	{
		Param param ;
		int intValue ;
		float floatValue ;
		char stringValue[PARALEN] ;
		char scientValue[PARALEN] ;
		int boolValue ;
		
	} attr ;
	
} attribute ;


typedef struct entry
{
	char name[NAMELEN] ;
	KindType kind ;
	Type symbolType ;	
	attribute attr ;
	int Register ;	
	
} entry ;


typedef struct symTable
{
	entry* entrys ;
	int level ;
	int size ;
	int capacity ;
	
} symTable ;


typedef struct stack
{
	symTable* tableStack ;
	int size ;
	int capacity ;
	int nextRegister ;
	
} stack ;




extern stack symStack ;


/* for symbol table */
void symStackInitializer(int) ;
void pushSymTable() ;
void popSymTable() ;
void enterScope() ;
void exitScope(int) ;
void attributeAssigner(entry*, attribute) ;
int insertToSymTable(char*, KindType, Type, attribute*) ;
void reStartParamReg() ;
void symTablePrinter() ;
char* concateString(int, ...) ;
void variableInserter(char*, KindType, TYPE, int) ;
Type dimTypeGetter(TYPE, char*) ;
void attrDecider(attribute*, TYPE, char*) ; 
TYPE converter(char*) ;
char* anti_converter(TYPE) ;
void printType(Type) ;
void functionInserter(TYPE, char*, char*, int) ;
void paramInserter(char*) ;
int attrEquivalent(attribute*, attribute*) ;
int typeEquivalent(Type*, Type*) ;
int funcDefined(char*, Type, attribute*) ;
void symStackFreer(stack*) ; 




#endif