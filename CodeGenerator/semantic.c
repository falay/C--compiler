#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "semantic.h"
#include <math.h>

funcDeclList funcList ;

int hasSemanticError = 0 ;
int linenum ;  


/*** Semantic analysis ***/

void semanticError(char* errorMesg)
{
	//fprintf( stderr, "##########Error at Line #%d: ERROR MESSAGE.##########\n", linenum ) ;
	//fprintf( stderr, "%s\n\n", errorMesg ) ;
	//hasSemanticError = 1 ;
	;
}


void funcDeclListInitializer(int capacity)
{
	funcList.List = (funcDecl*)malloc( sizeof(funcDecl)*capacity ) ;
	funcList.capacity = capacity ;
	funcList.size = 0 ;
}


void insertToFuncList(funcDecl decl)
{
	if( funcList.size == funcList.capacity )
	{
		funcList.capacity <<= 1 ;
		funcList.List = (funcDecl*)realloc( funcList.List, sizeof(funcDecl)*funcList.capacity ) ;
	}	
	
	funcList.List[ funcList.size++ ] = decl ;	
}


void funcMapChecker()
{
	symTable* topTable = &symStack.tableStack[0];
	
	for(int i=0; i<funcList.size; i++)
		if( !funcList.List[i].flag )
		{	
			int index = funcList.List[i].index ;
			char error[BUFLEN] ;
			sprintf( error, "function %s has no matched definition", topTable->entrys[index].name ) ;
			semanticError( error ) ;
		}
}




int existenceChecker(char* name, KindType kind)
{
	symTable* curTable = &symStack.tableStack[0];
	
	for(int i=0; i<curTable->size; i++)
		if( strcmp(curTable->entrys[i].name, name) == 0 && curTable->entrys[i].kind == kind )
			return i ;
			
	char error[BUFLEN] ;
	sprintf( error, "%s has not been declared", name ) ;
	semanticError( error ) ;
	return -1; 			
}


TYPE funcChecker(char* name, char* parameter)
{
	symTable* curTable = &symStack.tableStack[0] ;
	
	int index ;
	if( (index = existenceChecker(name, function)) < 0 )
		return None ;
	
	/* analyse the parameters */
	Param param = typeDecider( parameter ) ;
	paramCompartor( name, param, curTable->entrys[index].attr.attr.param ) ;
			
	return curTable->entrys[index].symbolType.basicType ;
}


Param typeDecider(char* parameter)
{
	char* endStr ;
	char* arg = strtok_r(parameter, ",", &endStr) ;
	Param param ;
	param.len = 0 ;
	
	while( arg )
	{
		if( strstr( arg, "+" ) == NULL && strstr( arg, "-" ) == NULL && strstr( arg, "*" ) == NULL &&
			strstr( arg, "/" ) == NULL && strstr( arg, "%" ) == NULL )
			param.paramList[ param.len++ ] = typeFinder( arg ) ;
			
		else
		{
			Type Basic ;
			Basic.basicType = typeEvaluater( arg ) ;
			Basic.dimLen = 0 ;
			param.paramList[ param.len++ ] = Basic ;		
		}	
		arg = strtok_r(NULL, ",", &endStr) ;
	}	
	
	return param ;
}


Type typeFinder(char* argument)
{
	Type argType ;
	argType.dimLen = 0 ;
	char* funcName ;

	if( strstr(argument,"\"") )
		argType.basicType = String ;
	
	else if( strcmp(argument, "true") == 0 || strcmp(argument, "false") == 0 )
		argType.basicType = Bool ;
	
	else if( isdigit(argument[0]) || argument[0] == '-' )
	{	
		if( strstr(argument,"E") || strstr(argument,".") ) 
			argType.basicType = Double ;
		else
			argType.basicType = Int ;
	}
	
	/*** functions ***/	
	else if( (funcName = strstr(argument, "(")) != NULL )
	{
		if( isalnum( *(funcName-1) ) )
		{
			char* funcID = strtok(argument, "(") ;
			symTable* curTable = &symStack.tableStack[0] ;
	
			int index ;
			if( (index = existenceChecker(funcID, function)) < 0 )
				argType.basicType = None ;
			else		
				argType.basicType = curTable->entrys[index].symbolType.basicType ;
		}	
	}	

	/*** identifiers ***/
	else
	{
		int argLen = 0 ;
		char* endStr ;	
		char* pch = strtok_r( argument, "[", &endStr ) ;
		char* ID = strdup( pch ) ;
		pch = strtok_r( NULL, "[", &endStr ) ;
		while( pch )
		{
			argLen ++ ;
			pch = strtok_r( NULL, "[", &endStr ) ;
		}	

		argType = typeLookup( ID, argLen ) ;
	}	
	
	return argType ;
}


/* int a[2][3][4]; f(a[2][3]) then argLen = 2 -> type = int[4] */
Type typeLookup(char* ID, int argLen) 
{
	Type argType ;
	argType.dimLen = 0 ;	
	
	int curSymTableIndex = symStack.size-1 ;
	while( curSymTableIndex >= 0 )
	{
		symTable* curTable = &symStack.tableStack[ curSymTableIndex ] ;

		for(int i=0; i<curTable->size; i++)
		{
			if( strcmp( ID, curTable->entrys[i].name ) == 0 && (curTable->entrys[i].kind == variable || curTable->entrys[i].kind == parameter || curTable->entrys[i].kind == constant) )
			{
				if( argLen > curTable->entrys[i].symbolType.dimLen )
				{
					char error[BUFLEN] ;
					sprintf( error, "invalid dimension usage of %s", ID ) ;
					semanticError( error ) ;	
					break ;
				}	

				for(int j=argLen; j<curTable->entrys[i].symbolType.dimLen; j++)
					argType.dimension[ argType.dimLen++ ] = curTable->entrys[i].symbolType.dimension[j] ;
				
				argType.basicType = curTable->entrys[i].symbolType.basicType ;
				
				return argType ;
			}	
		}	
	
		curSymTableIndex -- ;
	}	

	
	argType.basicType = None ;
	char error[BUFLEN] ;
	sprintf( error, "%s has not been declared", ID ) ;
	semanticError( error ) ;
	
	
	return argType ;
}





int typeCoercison(Type* arg, Type* decl)
{
	if( arg->dimLen != decl->dimLen )
		return 0 ;
	
	if( 
		!(
		(arg->basicType == decl->basicType) || 
		(arg->basicType == Int && decl->basicType == Float) ||
		(arg->basicType == Int && decl->basicType == Double) ||	
		(arg->basicType == Double && decl->basicType == Float) ||	
		(arg->basicType == Float && decl->basicType == Double) 	
		)
	)	
		return 0 ;
	
	for(int i=0; i<arg->dimLen; i++)
		if( arg->dimension[i] != decl->dimension[i] )
			return 0 ;
	
	return 1 ;
}


void paramCompartor(char* funcName, Param argParam, Param declParam)
{
	char error[BUFLEN] ;
	sprintf( error, "The parameters of function %s does not match.", funcName ) ;
	
	if( argParam.len != declParam.len )
	{	
		semanticError( error ) ;	
		return ;
	}	
	
	for(int i=0; i<argParam.len; i++)
		if( !typeCoercison( &argParam.paramList[i], &declParam.paramList[i] ) )
		{
			semanticError( error ) ;	
			return ;			
		}				
}

void checkRetType(char* func, TYPE decl, TYPE ret)
{
	Type Decl ;
	Type Ret ;

	Decl.basicType = decl ;
	Ret.basicType = ret ;
	Decl.dimLen = Ret.dimLen = 0 ;

	if( !typeCoercison( &Ret, &Decl ) )
	{
		char error[BUFLEN] ;
		sprintf( error, "The return type of function %s does not match.", func ) ;	
		semanticError( error ) ;
	}	
}


TYPE ifElseReturnTypeChecker(TYPE A, TYPE B)
{
	if( A == B )
		return A ;

	if( A == Void || B == Void )
		return Void ;

	return None ;
}


void typeChecker(char* ID, TYPE basicType, char* value)
{
	Type declType ;
	declType.basicType = basicType ;
	declType.dimLen = 0 ;

	Type initiType ;
	initiType.dimLen = 0 ;

	printf("\t") ; /*** incomprehensible bug ***/
	if( isBoolean(value) && booleanChecker(strdup(value)) ) 
		initiType.basicType = Bool; 
	else 	
		initiType.basicType = typeEvaluater(strdup(value)) ;	

	if( !typeCoercison( &initiType, &declType ) )
	{
		char error[BUFLEN] ;
		sprintf( error, "The initializer type of %s does not match.", ID ) ;	
		semanticError( error ) ;	
	}	
}


/* strip a[2][3][4]={11,5,8,7,9} into a[2][3][4] and return 5 (the number of initializers)*/
int stripInitializer(TYPE basicType, char* initialStmt) 
{
	if( strstr(initialStmt, "=") == NULL )
		return 0 ;

	char* endStr ; 
	char* IDdim = strtok_r( initialStmt, "=", &endStr ) ;
	char* initializers = strtok_r( NULL, "=", &endStr ) ;

	char* pch = strtok_r( initializers, ",", &endStr ) ;
	int numOfInit = 0 ;
	while( pch )
	{
		typeChecker( IDdim, basicType, pch ) ;

		numOfInit ++ ;

		pch = strtok_r( NULL, ",", &endStr ) ;	
	}	

	initialStmt = IDdim ;

	return numOfInit ;
}


void dimChecker(char* ID, Type dimType, int initNum)
{
	int dimSize = 1 ;
	for(int i=0; i<dimType.dimLen; i++)
		dimSize *= dimType.dimension[i] ;

	if( dimSize < initNum )
	{
		char error[BUFLEN] ;
		sprintf( error, "The initializing size of %s does not match.", ID ) ;	
		semanticError( error ) ;		
	}	
}

/*** leftOrRight = 0 -> lhs, 1 -> rhs ***/
int varExistenChecker(char* name, int leftOrRight)
{
	int curSymTableIndex = symStack.size-1 ;

	while( curSymTableIndex >= 0 )
	{	
		symTable* curTable = &symStack.tableStack[ curSymTableIndex ];

		char* endStr ;	
		char* ID = strtok_r( name, "[", &endStr ) ;
		
		for(int i=0; i<curTable->size; i++)
			if( strcmp(curTable->entrys[i].name, ID) == 0 )
			{	
				/** checking constant assignment **/
				if( leftOrRight == 0 && curTable->entrys[i].kind == constant )
				{
					char error[BUFLEN] ;
					sprintf( error, "try to modifiy the constant %s", ID ) ;
					semanticError( error ) ;
					return -1; 		
				}	
				return i ;
			}

		curSymTableIndex -- ;
	}		
		
	char error[BUFLEN] ;
	sprintf( error, "%s has not been declared", name ) ;
	semanticError( error ) ;
	return -1; 					
}



TYPE typeGeneralizer(TYPE a, TYPE b)
{
	if( a == String || a == Bool || b == String || b == Bool )
		return None ;
	
	if( a == b )
		return a ;
	
	if( a == Int && ( b == Double || b == Float ) )
		return Double ;
	
	if( b == Int && ( a == Double || a == Float ) )
		return Double ;
	
	if( ( a == Double && b == Float ) || ( b == Double && a == Float ) )
		return Double ;
	
	return None ;
}


TYPE typeEvaluater(char* expression)
{
	if( expression[0] == '"' )
		return String ;
	
	char* endStr ;
	char* operand = strtok_r( expression, "+-*/%", &endStr ) ;
	
	int first = 1 ;
	TYPE resultType ;
	while( operand )
	{
		Type operandType = typeFinder( operand ) ;
		if( operandType.dimLen > 0 )
			semanticError( "array arithmetic" ) ;	
		
		if( first )
		{	
			resultType = operandType.basicType ;
			first = 0 ;
		}	
		else
			resultType = typeGeneralizer( resultType, operandType.basicType ) ;
		
		if( resultType == None )
			semanticError("invalid expression") ;
		
		operand = strtok_r( NULL, "+-*/%", &endStr ) ;
	}	
	
	return resultType ;
}


void statementChecker(char* lhs, char* rhs)
{
	/** checking existence for lhs **/
	int index ;
	if( (index = varExistenChecker(strdup(lhs), 0)) < 0)
		return ;
	
	/** get the complete type of lhs **/
	Type lhsType = typeFinder( lhs ) ;
	if( lhsType.dimLen > 0 )
		semanticError( "array assignment" ) ;	
	
	Type rhsType ;
	rhsType.dimLen = 0 ;
	if( isBoolean(rhs) && booleanChecker(strdup(rhs)) ) 
		 rhsType.basicType = Bool ;

	/** decomposing rhs and return its final type **/
	else	
		rhsType.basicType = typeEvaluater( rhs ) ;
		
	if( !typeCoercison( &rhsType, &lhsType ) )
		semanticError( "invalid statement: unmatched type" ) ;			
}


TYPE printChecker(char* expr)
{
	if( isBoolean(expr) && booleanChecker(strdup(expr)) ) 
		return Bool ;

	TYPE exprType ;	
	if( (exprType = typeEvaluater(expr)) == Void )
		semanticError( "variable reference in print must be scalar type" ) ;		
	return exprType ;
}

void readChecker(char* var)
{
	Type varType = typeFinder( var ) ;
	
	if( varType.dimLen > 0 )
		semanticError( "variable reference in read must be scalar type" ) ;	
}


void intialChecker(char* Rhs, TYPE lhsType)
{
	if( strstr(Rhs, "=") == NULL )
		return ;

	char* endStr ; 
	char* ID = strtok_r( Rhs, "=", &endStr ) ;
	char* initializers = strtok_r( NULL, "$", &endStr ) ;

	TYPE rhsType ; 
	if( isBoolean(initializers) && booleanChecker(strdup(initializers)) ) 
		 rhsType = Bool ;
	else	
		rhsType = typeEvaluater( initializers ) ;
	
	Type rhs ;
	rhs.basicType = rhsType ;
	rhs.dimLen = 0 ;
	Type lhs ;
	lhs.basicType = lhsType ;
	lhs.dimLen = 0 ;	
	if( !typeCoercison(&rhs, &lhs) )
	{
		char error[BUFLEN] ;
		sprintf( error, "The type of initializers of %s is invalid", ID ) ;
		semanticError( error ) ;
	}

}



void jumpChecker(int inLoop, char* jmp)
{
	if( !inLoop )
	{
		char error[BUFLEN] ;
		sprintf( error, "%s can only appear in loop statements.", jmp ) ;
		semanticError( error ) ;
	}
}	


int isBoolean(char* expr)
{
	char* p = strstr(expr,"!") ; 
	if( p && isalpha(*(p+1)) )
		return 1 ;
	
	return strstr(expr,"&&") || strstr(expr,"||") || strstr(expr,"<=") || strstr(expr,"<") || strstr(expr,"==") || strstr(expr,"!=") || strstr(expr,">") || strstr(expr,">=")  ;
}


int booleanChecker(char* expr)
{
	if( expr == NULL )
		return 1 ;

	char* endStr ;
	char* andSegment = strtok_r( expr, "||", &endStr ) ;
	while( andSegment )
	{
		char* ending ;
		char* copySeg = strdup( andSegment ) ;
		char* seg = strtok_r( copySeg, "&&", &ending ) ;
		while( seg )
		{
			if( boolOperandAnalyser( seg ) == None )
			{
				semanticError( "invalid boolean expression" ) ;
				return 0;
			}	
			seg = strtok_r( NULL, "&&", &ending ) ;
		}	

		andSegment = strtok_r( NULL, "||", &endStr ) ;
	}	

	return 1 ;
}


TYPE boolOperandAnalyser(char* operands)
{
	char* boolOperator[BOOLNUM] = {"<=", "<", "==", ">=", ">", "!="} ;
	char* endStr ;

	for(int i=0; i<BOOLNUM; i++)
	{
		if( strstr( operands, boolOperator[i] ) )
		{
			char* lhsOperand = strtok_r( operands, boolOperator[i], &endStr ) ;
			char* rhsOperand = strtok_r( NULL, boolOperator[i], &endStr ) ;

			if( lhsOperand[0] == '!' )
				lhsOperand ++ ;
			if( rhsOperand[0] == '!' )
				rhsOperand ++ ;

			TYPE leftType  = typeEvaluater( lhsOperand ) ;
			TYPE rightType = typeEvaluater( rhsOperand ) ;

			if( (leftType == Int || leftType == Float || leftType == Double) 
				&& 	(rightType == Int || rightType == Float || rightType == Double) )
				return Bool ;

			if( leftType == Bool && rightType == Bool )
			{
				if( strcmp(boolOperator[i], "==") == 0 || strcmp(boolOperator[i], "!=") == 0 )
					return Bool ;
				else
					return None ;
			}	

			return None ;	
		}	
	}	

	if( operands[0] == '!' )
		operands ++ ;

	Type operandType = typeFinder( operands ) ;
	return (operandType.basicType == Bool)? Bool : None ;
}


void forLoopChecker(char* expr)
{
	if( strcmp(expr, "") == 0 )
		return ;
	
	char* endStr ;
	char* lhs = strtok_r( expr, "=", &endStr ) ;
	char* rhs = strtok_r( NULL, "=", &endStr ) ;
	
	statementChecker( lhs, rhs ) ;	
}


void moduleChecker(char* a, char* b)
{
	if( typeEvaluater(a) != Int || typeEvaluater(b) != Int )
		semanticError("both operands must be integers in modulo operation") ;	
}


void opChecker(char* a, char* b)
{
	if( (isBoolean(a) && booleanChecker(a)) || (isBoolean(b) && booleanChecker(b)) )
		semanticError("boolean arithmetic") ;
}



