#ifndef CODEGEN_H
#define CODEGEN_H
#define BUFSIZE 1024
#define STMTLEN 128
#define FORLEN 128

#include "symTable.h"
#include "semantic.h"



extern FILE* outFp ;
extern char* outFile ;


typedef enum
{
	EXPR, CONSTANT, FUNC, VAR
	
} factorType ;


typedef struct labelStack
{
	int stack[ BUFSIZE ] ;
	int size ;
	
} labelStack ;


typedef struct tempList
{
	char* list[ BUFSIZE ] ;
	int size ;	
	
} tempList ;


typedef struct forBuffer
{
	char incBuf[ FORLEN ][ STMTLEN ] ;
	int top ;
	
} forBuffer ;

typedef struct funcInfo
{
	TYPE retType ;
	TYPE paramType[PARALEN] ;
	int paramLen ;
	
} funcInfo ;


char buffer[BUFSIZE] ;
extern tempList bufList ;
extern forBuffer forBuf ;

/** for recursive function **/ 
char recursiveBuf[BUFSIZE] ;

/*** general code gen ***/
void genProgram() ;
void genMethodBegin(char*, char*, char) ; 
void genMethodEnd(int) ;
char typeSelector(TYPE) ;


void genPrintBegin() ;
void genPrintEnd(TYPE) ;
void genVariable(TYPE, char*) ;
void genConstant(char*);
void genStatement(char*, char*) ;
void genType(TYPE) ;
void genOperator(TYPE, char, TYPE) ;
void genPreOp(char*) ;
void genModule() ;
void genStore(TYPE) ;
void genLoad(TYPE, int) ;
void genVarRef(char*) ;
void genRead(int*, char*) ;
void genRelOp(TYPE, char*, TYPE) ;
void genLessOp() ;
char* jmpDecider(char*) ;


/*** tools ***/
void storeInstruction(char*) ;
void genExpression() ;
void genToList(char*, ...) ;


/*** For statement ***/
void genExpr2Tmp() ;
void genStmt2Tmp(char*) ;
void genTmpExpr() ;
void genStore2Tmp(TYPE) ;
void genType2Tmp(TYPE) ;


/*** If else ***/
void genIfBegin(int) ;
void genElseBegin(int) ;
void genExit(int) ;


/*** Procedure ***/
void genFunction(char*) ;
funcInfo lookUpFunc(char*) ;
char* getFuncID(char*) ;
void genReturn(TYPE) ;


/*** Type conversion ***/
void addConversion2Buf() ;

#endif