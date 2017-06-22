#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symTable.h"

#define BOOLNUM 6 

extern int hasSemanticError ;

typedef struct funcDecl
{
	int index ; /* The index of the function in the global symbol table */
	int flag ; /* 0 for declared but not defined, 1 for defined */
	
} funcDecl ;


typedef struct funcDeclList
{
	funcDecl* List ;
	int size ;
	int capacity ;
	
} funcDeclList ;


typedef struct Expr
{
	char* lexVal ;
	TYPE type ;
	
} Expr ;



extern funcDeclList funcList ;


/* for semantic check */
void funcDeclListInitializer(int) ;
void insertToFuncList(funcDecl) ;
void semanticError(char*) ;
void funcMapChecker() ;
int existenceChecker(char*, KindType) ;
TYPE funcChecker(char*, char*) ;
Param typeDecider(char*) ;
Type typeFinder(char*) ;
Type typeLookup(char*, int) ;
void paramCompartor(char*, Param, Param) ;
int typeCoercison(Type*, Type*) ;
void checkRetType(char*, TYPE, TYPE) ;
void typeChecker(char*, TYPE, char*) ;
int stripInitializer(TYPE, char*) ;
void dimChecker(char*, Type, int) ;

int varExistenChecker(char*, int) ;
void statementChecker(char*, char*) ;
TYPE typeEvaluater(char*) ;
TYPE typeGeneralizer(TYPE, TYPE) ;
TYPE printChecker(char*) ;
void readChecker(char*) ;
int booleanChecker(char*) ;
void jumpChecker(int, char*) ;
void forLoopChecker(char*) ;
TYPE boolOperandAnalyser(char*) ;
int isBoolean(char*) ;
void intialChecker(char*, TYPE) ;
TYPE ifElseReturnTypeChecker(TYPE, TYPE) ;
void moduleChecker(char*, char*) ;
void opChecker(char*, char*) ;


#endif