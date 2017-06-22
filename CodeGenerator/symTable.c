#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "symTable.h"
#include "semantic.h"

#define printBlank(n) {\
	for(int i=1; i<=n; i++) \
		printf(" ") ; \
}	



int paramReg = 0 ;
int curScope = 0 ;
stack symStack ;


/*** Symbol table construction ***/

void symStackInitializer(int capacity)
{
	symStack.tableStack = (symTable*)malloc( sizeof(symTable)*capacity ) ;
	symStack.size = 0 ;
	symStack.capacity = capacity ;
	symStack.nextRegister = 1 ;
}


void pushSymTable()
{
	if( symStack.size == symStack.capacity )
	{
		symStack.capacity <<= 1 ;
		symStack.tableStack = (symTable*)realloc( symStack.tableStack, sizeof(symTable)*symStack.capacity ) ;
	}
	
	symTable newTable ; 
	newTable.level = curScope ;
	newTable.size = 0 ;
	newTable.capacity = 1 ;
	newTable.entrys = (entry*)malloc( sizeof(entry) ) ;
	symStack.tableStack[ symStack.size++ ] = newTable ;	
}


void popSymTable()
{
	if( symStack.size == 0 )
		return ;
	
	int registerUsed = 0 ;
	symTable* curTable = &symStack.tableStack[ symStack.size-1 ] ;
	for(int i=0; i<curTable->size; i++)
		registerUsed += ( curTable->entrys[i].kind == variable ) ;	
	symStack.nextRegister -= registerUsed ;

	symStack.tableStack[ symStack.size-1 ].entrys = NULL ;	
	symStack.size -- ;	
}


int isInteger(char* num)
{
	for(int i=0; i<strlen(num); i++)
		if( !isdigit(num[0]) )
			return -1 ;
	return atoi( num ) ;	
}


Type dimTypeGetter(TYPE basicType, char* ID)
{
	Type dimType ;
	dimType.basicType = basicType ;
	
	char* endStr ;	
	char* pch = strtok_r( ID, "[", &endStr ) ;	
	strncpy( ID, pch, strlen(pch) ) ;
	
	pch = strtok_r( NULL, "[", &endStr ) ;
	int len = 0 ;
	while( pch )
	{
		pch[ strlen(pch)-1 ] = '\0' ;		

		int dim ;
		if( (dim = isInteger(pch)) <= 0 )
		{
			char error[BUFLEN] ;
			sprintf( error, "invalid array declaration of %s", ID ) ;	
			semanticError( error ) ;
		}	

		dimType.dimension[ len++ ] = dim ;
		pch = strtok_r( NULL, "[", &endStr ) ;
	}	
	
	dimType.dimension[ len ] = '\0' ;
	dimType.dimLen = len ;
		
	return dimType ;
}


void variableInserter(char* IDlist, KindType kind, TYPE basicType, int initialNum)
{
	char* endStr ;
	char* ID ;
	char* pch = strtok_r( IDlist, ",", &endStr ) ;
	
	while( pch )
	{		
		char* copyStr = (char*)malloc( sizeof(char)*(strlen(pch)+1) ) ;
		strcpy( copyStr, pch ) ;
		attribute attr ;
		
		Type dimType = dimTypeGetter( basicType, copyStr ) ;
		
		if( kind == variable )
		{		
			ID = copyStr ;
			attr.flag = -1 ;
			dimChecker( ID, dimType, initialNum ) ;
		}
		else if( kind == constant )
		{
			char* idEndStr ;
			ID = strtok_r( copyStr, "=", &idEndStr ) ;
			char* attrValue = strtok_r( NULL, "=", &idEndStr ) ;

			typeChecker( ID, basicType, attrValue ) ;

			attrDecider( &attr, dimType.basicType, attrValue ) ;		
		}
		
		insertToSymTable( ID, kind, dimType, &attr ) ;
		
		pch = strtok_r( NULL, ",", &endStr ) ;
	}	
}


void attrDecider(attribute* attr, TYPE basicType, char* attrValue) 
{
	switch( basicType ) 
	{
		case Int:
			attr->flag = 1 ; 
			attr->attr.intValue = atoi( attrValue ) ;
			break ;
			
		case Double: 
		case Float: 
		{	
			if( strstr( attrValue, "e" ) || strstr( attrValue, "E" ) ) /* scientfic */
			{	
				attr->flag = 4 ; 
				strcpy( attr->attr.scientValue, attrValue ) ;	
			}
			else
			{	
				attr->flag = 2 ; 
				attr->attr.floatValue = atof( attrValue ) ;
			}
			break ;
		}	
		
		case String:
			attr->flag = 3 ;
			strcpy( attr->attr.stringValue, attrValue ) ;
			break ;
		
		case Bool:
			attr->flag = 5 ;
			attr->attr.boolValue = (strcmp(attrValue, "true") == 0) ;
			break ;
			
		default:
			fprintf(stderr, "invalid type\n") ;
			exit(0) ;			
	} ;
}



void functionInserter(TYPE basicType, char* funcName, char* paramList, int funcFlag)
{
	/* function type */
	Type funcType ;
	funcType.basicType = basicType ;
	funcType.dimLen = 0 ;

	/* function attributes: parameters */
	attribute attr ;
	attr.flag = 0 ;
	attr.attr.param.len = 0 ;
	
	char* endStr ;
	char* pch = strtok_r( paramList, ",", &endStr ) ;
	while( pch )
	{
		char* end ;
		char* tmpPch = strdup( pch ) ;
		char* paraBasicType = strtok_r( tmpPch, " ", &end ) ;
		char* IDdim = strtok_r( NULL, " ", &end ) ;
		Type paraType = dimTypeGetter( converter(paraBasicType), IDdim ) ;		
		attr.attr.param.paramList[ attr.attr.param.len ++ ] = paraType ;

		pch = strtok_r( NULL, ",", &endStr ) ;
	}
	
	if( funcFlag == DECL )
	{	
		funcDecl decl ;
		decl.index = insertToSymTable( funcName, function, funcType, &attr ) ;
		if( decl.index < 0 )
			return ;
		decl.flag = 0 ;
		insertToFuncList( decl ) ;
	}
	else
	{
		int index ;
		if( (index = funcDefined( funcName, funcType, &attr )) < 0 )
		{
			funcDecl def ;
			def.index = insertToSymTable( funcName, function, funcType, &attr ) ;
			if( def.index < 0 )
				return ;			
			def.flag = 1 ;
			insertToFuncList( def ) ;	
		}	
		else
			funcList.List[index].flag = 1 ;	
	}	
}



int funcDefined(char* funcName, Type funcType, attribute* attr)
{
	symTable* globalTable = &symStack.tableStack[0] ;
	for(int i=0; i<globalTable->size; i++)
	{
		if( strcmp(globalTable->entrys[i].name, funcName) == 0 && 
			globalTable->entrys[i].kind == function &&
			typeEquivalent( &globalTable->entrys[i].symbolType, &funcType ) &&
			attrEquivalent( &globalTable->entrys[i].attr, attr )
		  )
			return i ;
	}	
	
	return -1 ;
}





int typeEquivalent(Type* A, Type* B)
{
	if( A->basicType != B->basicType || A->dimLen != B->dimLen )
		return 0 ;
	
	for(int i=0; i<A->dimLen; i++)
		if( A->dimension[i] != B->dimension[i] )
			return 0 ;
	
	return 1 ;
}


int attrEquivalent(attribute* A, attribute* B) // for function attribute only
{
	if( A->flag != 0 || B->flag != 0 || A->attr.param.len != B->attr.param.len )
		return 0 ;
	
	for(int i=0; i<A->attr.param.len; i++)
		if( !typeEquivalent( &A->attr.param.paramList[i], &B->attr.param.paramList[i] ) )
			return 0 ;
	
	return 1 ;	
}


void paramInserter(char* paramList)
{
	if( paramList == NULL )
		return ;
	
	char* endStr ;
	char* pch = strtok_r( paramList, ",", &endStr ) ;
	while( pch )
	{
		attribute attr ;
		attr.flag = -1 ;
		
		char* end ;
		char* tmpPch = strdup( pch ) ;
		char* paraBasicType = strtok_r( tmpPch, " ", &end ) ;
		char* IDdim = strtok_r( NULL, " ", &end ) ;
		Type paraType = dimTypeGetter( converter(paraBasicType), IDdim ) ;		
			
		insertToSymTable( IDdim, parameter, paraType, &attr ) ;
		
		pch = strtok_r( NULL, ",", &endStr ) ;
	}
	
	free( paramList ) ;
}


int insertToSymTable(char* name, KindType kind, Type symbolType, attribute* attr)
{
	symTable* topTable = &symStack.tableStack[symStack.size-1] ;
	
	if( topTable->size == topTable->capacity )
	{
		topTable->capacity <<= 1 ;
		topTable->entrys = (entry*)realloc( topTable->entrys, sizeof(entry)*topTable->capacity ) ;
	}	
	
	/* checking duplication */
	for(int i=0; i<topTable->size; i++)
		if( strcmp( topTable->entrys[i].name, name ) == 0 )
		{	
			char error[BUFLEN] ;
			sprintf( error, "identifier %s has been defined", name ) ;
			semanticError( error ) ;	
			return -1 ;
			//exit(0) ;
		}	
	
	entry newEntry ;
	strcpy( newEntry.name, name ) ;
	newEntry.kind = kind ;
	newEntry.symbolType = symbolType ;
	newEntry.attr = *attr ;	
	
	/** register mapping **/
	if( kind == parameter )
		newEntry.Register = paramReg ++ ;	
	
	topTable->entrys[ topTable->size++ ] = newEntry ;
	
	return topTable->size - 1 ;
}


void reStartParamReg()
{
	paramReg = 0 ;
}



void symTablePrinter()
{
	symTable* topTable = &symStack.tableStack[ symStack.size-1 ];
	if( topTable->size == 0 )
		return ;

	printf("=======================================================================================\n");
	// Name [29 blanks] Kind [7 blanks] Level [7 blank] Type [15 blanks] Attribute [15 blanks]
	printf("Name"); printBlank(29); printf("Kind"); printBlank(7); printf("Level"); printBlank(7); printf("Type"); printBlank(15); printf("Attribute"); printBlank(15); printf("\n") ;
	printf("---------------------------------------------------------------------------------------\n");
	
	
	
	for(int i=0; i<topTable->size; i++)
	{
		/* Name */
		int len = strlen(  topTable->entrys[i].name ) ;
		printf( "%s", topTable->entrys[i].name ); printBlank(29-len) ;
		
		/* Kind */
		switch( topTable->entrys[i].kind )
		{
			case function:
				printf("function") ;
				break ;
				
			case parameter:
				printf("parameter") ;
				break ;
				
			case variable:
				printf("variable") ;
				break ;
				
			case constant:
				printf("constant") ;
				break ;
				
			default:
				fprintf(stderr, "invalid kind\n") ;
				exit(0) ;
		} ;
		int alignLen = (topTable->entrys[i].kind == parameter)? 0 : 1 ;
		printBlank(7+alignLen);
		
		/* Level */
		printf("%d", topTable->level) ;
		if( topTable->level > 0 )
			printf("(local)") ;
		else
			printf("(global)") ;
		
		printBlank(7);
		
		/* Type */
		switch( topTable->entrys[i].symbolType.basicType )
		{
			case Int:
				printf("int") ;
				break ;
			
			case Double: 
				printf("double") ;
				break ;
			
			case Float:
				printf("float") ;
				break ;
			
			case String:
				printf("string") ;
				break ;
			
			case Bool:
				printf("bool") ;
				break ;
			
			case Void:
				printf("void") ;
				break ;
			
			default:
				break ;
				//fprintf(stderr, "invalid type!!!\n") ;
				//exit(0) ;				
		} ;
		for(int j=0; j<topTable->entrys[i].symbolType.dimLen; j++)
			printf("[%d]",topTable->entrys[i].symbolType.dimension[j]) ;
		printBlank(15);
		
		/* Attribute */				
		switch( topTable->entrys[i].attr.flag )
		{
			case 0:
			{
				int totalLen = topTable->entrys[i].attr.attr.param.len ;
				for(int j=0; j<totalLen; j++)
				{	
					printType( topTable->entrys[i].attr.attr.param.paramList[j] ) ;
					if( j < totalLen - 1 )
						printf(",") ;
				}	
				break ;	
			}
			
			case 1:
				printf("%d", topTable->entrys[i].attr.attr.intValue) ;
				break ;
					
			case 2:
				printf("%f", topTable->entrys[i].attr.attr.floatValue) ;
				break ;
			
			case 3:
				printf("%s", topTable->entrys[i].attr.attr.stringValue) ;
				break ;
				
			case 4:
				printf("%s", topTable->entrys[i].attr.attr.scientValue) ;
				break ;				
				
			case 5:
			{	
				int value = topTable->entrys[i].attr.attr.boolValue ;
				if( value ) printf("true") ;
				else		printf("false") ;
				break ;					
			}
			
			default:
				break ;
		} ;
		printBlank(15);
		
		printf("\n") ;
	}	
	
	printf("======================================================================================\n");
	printf("\n\n") ;
}


void enterScope()
{
	curScope ++ ;
	
	/* Push a new symbol table onto the stack */
	pushSymTable() ;
} 


void exitScope(int Opt_Symbol) 
{
	curScope -- ;
	
	if( Opt_Symbol )
		symTablePrinter() ;
	
	/* Pop the current top symbol table */
	popSymTable() ;
}

char* concateString(int numOfArgs, ...)
{
	va_list varList ;
	va_start( varList, numOfArgs ) ;
		
	char* result = strdup( va_arg( varList, char* ) );
	int totalLen = strlen( result ) ;
	
	for(int i=2; i<=numOfArgs; i++)
	{
		char* arg = va_arg( varList, char* ) ;
			
		totalLen += strlen( arg ) ;
		result = (char*)realloc( result, sizeof(char)*totalLen ) ;
		strcat( result, arg ) ;
	}	
	
	va_end( varList ) ;
	
	return result ;
}

TYPE converter(char* type)
{
	switch( type[0] )
	{
		case 'i':
			return Int ;
		case 'd':
			return Double ;
		case 'f':
			return Float ;
		case 's':
			return String ;
		case 'b':
			return Bool ;
		case 'v':
			return Void ;
	} ;
}

char* anti_converter(TYPE type)
{
	switch( type )
	{
		case Int:
			return "int" ;
		case Double:
			return "double" ;
		case Float:
			return "float" ;
		case String:
			return "string" ;
		case Bool:
			return "bool" ;
		case Void:
			return "void" ;
		default:
			return "none" ;
	} ;
}

void printType(Type type)
{
	printf("%s", anti_converter(type.basicType)) ;
	
	for(int i=0; i<type.dimLen; i++)
		printf("[%d]", type.dimension[i]) ;
}


void symStackFreer(stack* symStack) 
{	
	free(symStack->tableStack) ;
	symStack->size = symStack->capacity = 0 ;
}

