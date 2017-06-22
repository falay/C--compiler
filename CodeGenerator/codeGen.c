#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "codeGen.h"
#include "semantic.h"

int curLabel = 1 ;


labelStack Stack ;
tempList bufList ;
forBuffer forBuf ;


void genProgram()
{
	outFile[ strlen(outFile) - 2 ] = '\0' ;	
	fprintf( outFp, ".class public %s\n.super java/lang/Object\n", outFile ) ;
	fprintf( outFp, ".field public static _sc Ljava/util/Scanner;\n" ) ;
	bufList.size = 0 ;
	Stack.size = 0 ;
	forBuf.top = 0 ;
}


void genMethodBegin(char* methodName, char* params, char type) 
{
	char paraBuf[ BUFSIZE ] = {'\0'} ;

	if( strcmp( params,"" ) == 0 )
		sprintf( paraBuf, "[Ljava/lang/String;" ) ;
	else
	{	
		char* pch = strtok( params, "," ) ;
		while( pch )
		{	
			sprintf( paraBuf+strlen(paraBuf), "%c", typeSelector(converter(pch)) ) ;
			pch = strtok( NULL, "," ) ;
		}	
	}
	
	sprintf( recursiveBuf, "(%s)%c", paraBuf, type ) ;
	fprintf( outFp, ".method public static %s(%s)%c\n.limit stack 100\n.limit locals 100\n", methodName, paraBuf, type ) ;
}


void genMethodEnd(int isVoid) 
{
	if( isVoid )
		fprintf( outFp, "return\n" ) ;	
	
	fprintf( outFp, ".end method\n" ) ;
}


char typeSelector(TYPE t) 
{
	switch( t )
	{
		case Bool:
			return 'Z' ;
			
		case Int:	
			return 'I' ;
		
		case Float:
		case Double:	
			return 'F' ;
			
		case Void:	
			return 'V' ;
	} ;
}


void genPrintBegin() 
{
	fprintf( outFp, "getstatic java/lang/System/out Ljava/io/PrintStream;\n" ) ;
}

void genPrintEnd(TYPE t) 
{
	switch( t )
	{
		case String:
			fprintf( outFp, "invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n" ) ;
			break ;
					
		case Bool:
			fprintf( outFp, "invokevirtual java/io/PrintStream/print(Z)V\n" ) ;
			break ;
			
		case Int:	
			fprintf( outFp, "invokevirtual java/io/PrintStream/print(I)V\n" ) ;
			break ;		
		
		case Float:
		case Double:
			fprintf( outFp, "invokevirtual java/io/PrintStream/print(F)V\n" ) ;
			break ;	
	} ;		
}

void genVariable(TYPE t, char* rhs)
{
	if( curScope == 0 ) /** Global variable **/ 
	{
		fprintf( outFp, ".field public static %s ", rhs ) ;
		genType( t ) ;
	}
	else /** Local variable **/
	{
		symTable* curTable = &symStack.tableStack[ symStack.size-1 ] ;
		for(int i=0; i<curTable->size; i++)
			if( strcmp(curTable->entrys[i].name, rhs) == 0 && curTable->entrys[i].kind == variable )
			{	
				curTable->entrys[i].Register = symStack.nextRegister ++ ;
				break ;
			}	
	}	
}

void genConstant(char* constant)
{
	Type t = typeFinder( constant ) ;
	switch( t.basicType )
	{
		case Int: 
			snprintf( buffer, sizeof(buffer), "sipush %s", constant ) ;
			break ;	
		
		case Double:
		case Float:
		case String:
			snprintf( buffer, sizeof(buffer), "ldc %s", constant ) ;
			break ;	

		case Bool:
		{
			if( strcmp( constant, "true" ) == 0 )
				snprintf( buffer, sizeof(buffer), "iconst_1" ) ;
			else
				snprintf( buffer, sizeof(buffer), "iconst_0" ) ;
			
			break ; 
		}				
	} ;
	
	storeInstruction( buffer ) ;
}


void genStatement(char* ID, char* expr)
{
	int curSymTableIndex = symStack.size-1 ;
	while( curSymTableIndex >= 0 )
	{
		symTable* curTable = &symStack.tableStack[ curSymTableIndex ] ;
		for(int i=0; i<curTable->size; i++)
			if( strcmp(curTable->entrys[i].name, ID) == 0 )
			{
				if( curTable->level == 0 ) // global variable
				{
					TYPE lhsType = curTable->entrys[i].symbolType.basicType ;		
					fprintf( outFp, "putstatic %s/%s ", outFile, ID ) ;				
					genType( lhsType ) ;	
				}
				else
				{
					TYPE lhsType = curTable->entrys[i].symbolType.basicType ;
					genStore( lhsType ) ;
					fprintf( outFp, "%d\n", curTable->entrys[i].Register ) ;
				}
			
				return ;
			}	
		
		curSymTableIndex -- ;
	}
}

void genType(TYPE t)
{
	switch( t )
	{
		case Int:
			fprintf( outFp, "I\n" ) ;
			break ;		
		
		case Double:
		case Float:
			fprintf( outFp, "F\n" ) ;
			break ;
				
		case Bool:
			fprintf( outFp, "Z\n" ) ;
			break ;
	};	
}

void genStore(TYPE t)
{
	switch( t )
	{		
		case Int:
		case Bool:
			fprintf( outFp, "istore " ) ;
			break ;
						
		case Double:
		case Float:
			fprintf( outFp, "fstore " ) ;
			break ;
	};
	
	storeInstruction( buffer ) ;
}


void genLoad(TYPE t, int reg)
{
	switch( t )
	{		
		case Int:
		case Bool:
			snprintf( buffer, sizeof(buffer), "iload %d", reg ) ;
			break ;
				
		case Double:
		case Float:
			snprintf( buffer, sizeof(buffer), "fload %d", reg ) ;			
			break ;
	} ;
	
	storeInstruction( buffer ) ;	
}


void genOperator(TYPE lhsType, char op, TYPE rhsType)
{
	if( lhsType == None )
	{
		switch( op )
		{
			case '%':
				snprintf( buffer, sizeof(buffer), "irem" ) ;
				break ;	

			case '&':
				snprintf( buffer, sizeof(buffer), "iand" ) ;
				break ;				
			
			case '|':
				snprintf( buffer, sizeof(buffer), "ior" ) ;
				break ;		
		} ;
	}
	else
	{	
		TYPE genType ;
		
		if( lhsType == Int && rhsType == Int )
			genType = Int ;
		else 
		{	
			if( rhsType == Int )
				genToList( "i2f" ) ;
			else if( lhsType == Int )
				addConversion2Buf() ;	
			
			genType = Float ;
		}
		
		switch( op )
		{
			case '+':
				if( genType == Int )
					snprintf( buffer, sizeof(buffer), "iadd" ) ;
				else
					snprintf( buffer, sizeof(buffer), "fadd" ) ;
				break ;
				
			case '-':
				if( genType == Int )
					snprintf( buffer, sizeof(buffer), "isub" ) ;
				else
					snprintf( buffer, sizeof(buffer), "fsub" ) ;
				break ;			
				
			case '*':
				if( genType == Int )
					snprintf( buffer, sizeof(buffer), "imul" ) ;
				else
					snprintf( buffer, sizeof(buffer), "fmul" ) ;
				break ;
				
			case '/':
				if( genType == Int )
					snprintf( buffer, sizeof(buffer), "idiv" ) ;
				else
					snprintf( buffer, sizeof(buffer), "fdiv" ) ;
				break ;			
		} ;
	}
	storeInstruction( buffer ) ;
}

void addConversion2Buf() 
{
	for(int i=bufList.size-1; i>=0; i--)
	{	
		if( strcmp( bufList.list[i], "") == 0 ) /* meaningless */
			continue ;

		if( strncmp( bufList.list[i], "ldc", 3 ) == 0 || strncmp( bufList.list[i], "fload", 5 ) == 0 
			|| strncmp( bufList.list[i], "invokestatic", 12 ) == 0 )
		{	
			strcat( bufList.list[i-1], "\ni2f" ) ;
			break ;
		}
		
	}
}


void genPreOp(char* preOp)
{
	if( strcmp(preOp, "") == 0 )
		return ;

	switch( preOp[0] )
	{
		case '-':
			snprintf( buffer, sizeof(buffer), "fneg" ) ;
			break ;
			
		case '!':
			snprintf( buffer, sizeof(buffer), "iconst_1\nixor" ) ;
			break ;
	} ;

	storeInstruction( buffer ) ;
}


char* jmpDecider(char* relOp)
{
	if( strcmp(relOp, "<") == 0 )
		return strdup("iflt") ;
	
	else if( strcmp(relOp, "<=") == 0 )
		return strdup("ifle") ;
	
	else if( strcmp(relOp, "!=") == 0 )
		return strdup("ifne") ;

	else if( strcmp(relOp, ">=") == 0 )
		return strdup("ifge") ;	
	
	else if( strcmp(relOp, ">") == 0 )
		return strdup("ifgt") ;

	else if( strcmp(relOp, "==") == 0 )
		return strdup("ifeq") ;
}


void genRelOp(TYPE lhsType, char* relOp, TYPE rhsType)
{
	TYPE genType ;
		
	if( lhsType == Int && rhsType == Int )
		genType = Int ;
	else 
	{	
		if( rhsType == Int )
			genToList( "i2f" ) ;
		genType = Float ;
	}	
	
	int L1 = curLabel ++ ;
	int L2 = curLabel ++ ; 
	
	char* conditionJmp = jmpDecider( relOp ) ;
	
	if( genType == Int )
		snprintf( buffer, sizeof(buffer), "isub\n%s L%d\niconst_0\ngoto L%d\nL%d:\niconst_1\nL%d:",conditionJmp, L1, L2, L1, L2 ) ;
	else
		snprintf( buffer, sizeof(buffer), "fcmpl\n%s L%d\niconst_0\ngoto L%d\nL%d:\niconst_1\nL%d:",conditionJmp, L1, L2, L1, L2 ) ;
	
	storeInstruction( buffer ) ;
	free( conditionJmp ) ;
}

void genVarRef(char* ID) 
{
	int curSymTableIndex = symStack.size-1 ;
	while( curSymTableIndex >= 0 )
	{
		symTable* curTable = &symStack.tableStack[ curSymTableIndex ] ;
		for(int i=0; i<curTable->size; i++)
		{	
			if( strcmp(curTable->entrys[i].name, ID) == 0 )
			{	
				if( curTable->entrys[i].kind == variable || curTable->entrys[i].kind == parameter )
				{
					if( curTable->level == 0 ) // global variable
					{
						fprintf( outFp, "getstatic %s/%s ", outFile, ID ) ;
						genType( curTable->entrys[i].symbolType.basicType ) ;	
					}
					else
					{
						genLoad( curTable->entrys[i].symbolType.basicType, curTable->entrys[i].Register ) ;
						//fprintf( outFp, "%d\n", curTable->entrys[i].Register ) ;
					}
				}	
				else if( curTable->entrys[i].kind == constant )
				{
					switch( curTable->entrys[i].symbolType.basicType )
					{
						case Int:
							snprintf( buffer, sizeof(buffer), "sipush %d", curTable->entrys[i].attr.attr.intValue ) ;
							break ;
						
						case Double:
						case Float:
							snprintf( buffer, sizeof(buffer), "ldc %f", curTable->entrys[i].attr.attr.floatValue ) ;
							break ;							
						
						case String:
							snprintf( buffer, sizeof(buffer), "ldc %s", curTable->entrys[i].attr.attr.stringValue ) ;						
							break ;	
							
						case Bool:
						{
							if( curTable->entrys[i].attr.attr.boolValue )
								snprintf( buffer, sizeof(buffer), "iconst_1\n" ) ;
							else
								snprintf( buffer, sizeof(buffer), "iconst_0\n" ) ;
							
							break ;	
						}						
					} ;	 
				}	
				
				storeInstruction( buffer ) ;	
				return ;
			}	
		}
			
		curSymTableIndex -- ;
	}
}

void genRead(int* readInstance, char* varID)
{
	if( *readInstance == 0 )
	{
		fprintf( outFp, "new java/util/Scanner\n" ) ;
		fprintf( outFp, "dup\n" ) ;
		fprintf( outFp, "getstatic java/lang/System/in Ljava/io/InputStream;\n" ) ;
		fprintf( outFp, "invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n" ) ;
		fprintf( outFp, "putstatic %s/_sc Ljava/util/Scanner;\n", outFile ) ;
		*readInstance = 1 ;
	}	
	
	fprintf( outFp, "getstatic %s/_sc Ljava/util/Scanner;\n", outFile ) ;
	
	int curSymTableIndex = symStack.size-1 ;
	while( curSymTableIndex >= 0 )
	{
		symTable* curTable = &symStack.tableStack[ curSymTableIndex ] ;
		for(int i=0; i<curTable->size; i++)
		{	
			if( strcmp(curTable->entrys[i].name, varID) == 0 && curTable->entrys[i].kind == variable )
			{	
				switch( curTable->entrys[i].symbolType.basicType )
				{
					case Int:	
						fprintf( outFp, "invokevirtual java/util/Scanner/nextInt()I\n" ) ;
						break ;	
					
					case Double:
					case Float:
						fprintf( outFp, "invokevirtual java/util/Scanner/nextFloat()F\n" ) ;
						break ;							
											
					case Bool:
						fprintf( outFp, "invokevirtual java/util/Scanner/nextBoolean()Z\n" ) ;
						break ;	
																
				} ;	 
				
				if( curTable->level == 0 ) // global variable
				{
					fprintf( outFp, "putstatic %s/%s ", outFile, varID ) ;
					genType( curTable->entrys[i].symbolType.basicType ) ;	
				}
				else
				{
					genStore( curTable->entrys[i].symbolType.basicType ) ;
					fprintf( outFp, "%d\n", curTable->entrys[i].Register ) ;
				}
				
				return ;
			}	
		}
			
		curSymTableIndex -- ;
	}		
}

void genIfBegin(int label) 
{
	fprintf( outFp, "ifeq Lelse_%d\n", label ) ;
}

void genElseBegin(int label) 
{
	fprintf( outFp, "goto Lexit_%d\n", label ) ;
	fprintf( outFp, "Lelse_%d:\n", label ) ;
}

void genExit(int label) 
{
	fprintf( outFp, "Lexit_%d:\n", label ) ;
}


void storeInstruction(char* instruction)
{
	if( strcmp( instruction, "" ) == 0 )
		return ;
	
	bufList.list[ bufList.size++ ] = strdup( instruction ) ;
	memset( instruction, 0, sizeof(instruction) ) ;
}


void genExpression() 
{
	for(int i=0; i<bufList.size; i++)
	{	
		if( strcmp( bufList.list[i], "") == 0 ) /* meaningless */
			continue ;
		fprintf( outFp, "%s\n", bufList.list[i] ) ;
		free( bufList.list[i] ) ;
	}
	bufList.size = 0 ;
}


void genToList(char* stmt, ...)
{
	char tmpBuf[ BUFSIZE ] ;
	
	va_list param ;
	va_start( param, stmt ) ;
	vsnprintf( tmpBuf, sizeof(tmpBuf), stmt, param ) ;
	va_end( param ) ;
	
	storeInstruction( tmpBuf ) ;
}


void genExpr2Tmp() 
{
	for(int i=0; i<bufList.size; i++)
	{	
		if( strcmp( bufList.list[i], "") == 0 ) /* meaningless */
			continue ;
		strcat( forBuf.incBuf[forBuf.top], bufList.list[i] ) ;	
		strcat( forBuf.incBuf[forBuf.top], "\n" ) ;
		free( bufList.list[i] ) ;
	}
	bufList.size = 0 ;	
}

void genStmt2Tmp(char* ID) 
{
	int curSymTableIndex = symStack.size-1 ;
	while( curSymTableIndex >= 0 )
	{
		symTable* curTable = &symStack.tableStack[ curSymTableIndex ] ;
		for(int i=0; i<curTable->size; i++)
			if( strcmp(curTable->entrys[i].name, ID) == 0 )
			{
				if( curTable->level == 0 ) // global variable
				{
					sprintf( forBuf.incBuf[forBuf.top]+strlen(forBuf.incBuf[forBuf.top]), "putstatic %s/%s ", outFile, ID ) ;
					genType2Tmp( curTable->entrys[i].symbolType.basicType ) ;	
				}
				else
				{
					genStore2Tmp( curTable->entrys[i].symbolType.basicType ) ;
					sprintf( forBuf.incBuf[forBuf.top]+strlen(forBuf.incBuf[forBuf.top]), "%d\n", curTable->entrys[i].Register ) ;
				}
					
				forBuf.top ++ ;
			
				return ;
			}	
		
		curSymTableIndex -- ;
	}	
}


void genType2Tmp(TYPE t)
{
	switch( t )
	{	
		case Int:
			strcat( forBuf.incBuf[forBuf.top], "I\n" ) ;
			break ;
		
		case Double:
		case Float:
			strcat( forBuf.incBuf[forBuf.top], "F\n" ) ;
			break ;
				
		case Bool:
			strcat( forBuf.incBuf[forBuf.top], "Z\n" ) ;
			break ;
	};	
}

void genStore2Tmp(TYPE t)
{
	switch( t )
	{		
		case Bool:	
		case Int:
			strcat( forBuf.incBuf[forBuf.top], "istore " ) ;
			break ;
						
		case Double:
		case Float:
			strcat( forBuf.incBuf[forBuf.top], "fstore " ) ;
			break ;
	};

}

void genTmpExpr() 
{
	forBuf.top -- ;

	fprintf( outFp, "%s", forBuf.incBuf[forBuf.top] ) ;
	forBuf.incBuf[forBuf.top][0] = '\0' ;
}


funcInfo lookUpFunc(char* funcID)
{
	symTable* curTable = &symStack.tableStack[ 0 ] ;
	funcInfo info ;
	
	for(int i=0; i<curTable->size; i++)
		if( strcmp(curTable->entrys[i].name, funcID) == 0 && curTable->entrys[i].kind == function )
		{		
			
			info.paramLen = curTable->entrys[i].attr.attr.param.len ;
			info.retType = curTable->entrys[i].symbolType.basicType ;
			for(int j=0; j<info.paramLen; j++)
				info.paramType[j] = curTable->entrys[i].attr.attr.param.paramList[j].basicType ;	
			
			return info ;
		}	
	
	info.paramLen = -1 ;
	return info ; 		
}


void genFunction(char* funcID)
{
	funcInfo info ;
	info = lookUpFunc( funcID ) ;
	char paraBuf[ BUFSIZE ] = {'\0'} ;
	
	
	if( info.paramLen == -1 )  // Recursive
	{	
		fprintf( outFp, "invokestatic %s/%s%s\n", outFile, funcID, recursiveBuf, typeSelector(info.retType) ) ;
		//genToList( "invokestatic %s/%s%s\n", outFile, funcID, recursiveBuf, typeSelector(info.retType) ) ;
		return ;
	}	
	
	if( info.paramLen == 0 )
		sprintf( paraBuf, "[Ljava/lang/String;" ) ;
	
	else
	{		
		for(int i=0; i<info.paramLen; i++)
			sprintf( paraBuf+strlen(paraBuf), "%c", typeSelector(info.paramType[i]) ) ;
	}
	
	//genToList( "invokestatic %s/%s%s\n", outFile, funcID, recursiveBuf, typeSelector(info.retType) ) ;
	fprintf( outFp, "invokestatic %s/%s(%s)%c\n", outFile, funcID, paraBuf, typeSelector(info.retType) ) ;
}

char* getFuncID(char* funcCall)
{
	return strtok( funcCall, "(" ) ;
}

void genReturn(TYPE expType)
{	
	if( expType == Int )
		fprintf( outFp, "ireturn\n") ;
	else
		fprintf( outFp, "freturn\n") ;	
}

