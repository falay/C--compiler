%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "statistic.h"
#include "symTable.h"
#include "semantic.h"
#include "codeGen.h"	

#define DEFAULTOUT "output.j"

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */

extern int Opt_Source ;
extern int Opt_Token ;
extern int Opt_Statistic ;
extern int Opt_Symbol ;	

int yylex() ;
int yyerror(char*) ;
void completeMesg() ;
void CminusCompiler(FILE*) ;
int noFunctionDefinition = 1 ;
int inLoop = 0 ;
int readInstance = 0 ;


extern int curLabel ;
extern labelStack Stack ;

factorType type ;
TYPE codeGenType ;
TYPE funcCodeGenType ;

char* outFile ;
FILE* outFp ;


%}

/****** identifier ******/
%token<text> ID         	

/****** reserved words ******/
%token<basicType> INT DOUBLE FLOAT STRING BOOL VOID CONST BOOLEAN
%token PRINT READ FOR WHILE BREAK RETURN CONTINUE DO IF ELSE

/****** operator ******/
%token<text> PLUS MINUS MUL DIV MOD ASSIGN LESS LESSEQ NOTEQ LARGEEQ LARGE EQUAL AND OR NOT 

/****** literal constant ******/
%token<text> INT_CONST 
%token<text> FLOAT_CONST 
%token<text> STRING_CONST SCIENTICFIC_CONST 
%token<text> TRUE FALSE 
		

%left PLUS MINUS
%left MUL DIV

%nonassoc "then"
%nonassoc ELSE

	
%union
{
	TYPE basicType ;
	char* text ;
	void* nonTerminal ;
	Expr exprs ;
}

%type<basicType> type statement compound function_body
%type<nonTerminal> initial_ID normal_declaration initial_ID_list dimension_declaration  
%type<nonTerminal> prefixOperator constant variable_reference
%type<nonTerminal> const_declaratin const_ID_list const_ID prefix
%type<nonTerminal> parameter_list nonEmptyParamList parameter function_declaration
%type<nonTerminal> expression_list nonEmptyExprList assign_expr_list nonEmpty_assign_expr_list assign_expr
%type<nonTerminal> relation_op condition_expression while_condition increment_expr func_expression_list nonEmptyFuncExprList
%type<exprs> originalFactor factor term expr logic_factor logic_term expression


%start program

%%

/*** C- Grammar ***/
program: global_declaration_list 
		| /*epsilon*/
		;
						
global_declaration_list: global_declaration_list global_declaration
						| global_declaration 
						;

global_declaration: variable_declaration 
					| function_declaration 
					;

					
variable_declaration: const_declaratin
					| normal_declaration
					;

const_declaratin: CONST type const_ID_list ';' 
{
	variableInserter( $3, constant, $2, 1 ) ;
} ;

const_ID_list: const_ID_list ',' const_ID {	$$ = concateString( 3, $1, ",", $3 ) ;}
				| const_ID { $$ = $1 ; }
				;

constant:  INT_CONST { $$ = $1 ; }
| FLOAT_CONST { $$ = $1 ; } 
| SCIENTICFIC_CONST { $$ = $1 ; }
| STRING_CONST { $$ = concateString(3, "\"", $1, "\"") ; } 
| TRUE { $$ = $1 ; } | FALSE { $$ = $1 ; } ;
	
	
const_ID: ID ASSIGN prefix constant 
{
	$$ = concateString( 4, $1, "=", $3, $4 ) ;
}
;

prefix: MINUS {$$ = $1 ;} | {$$ = "" ;};
	
normal_declaration: type initial_ID_list ';' 
{  	
	if( strstr( $2, "[" ) )
	{
		int initialNum = stripInitializer($1, $2) ;
		variableInserter( $2, variable, $1, initialNum ) ;	
	}	

	else
	{
		char* endStr ;
		char* pch = strtok_r( $2, ",", &endStr ) ;
	
		while( pch )
		{
			intialChecker( pch, $1 ) ;
			pch = strtok_r( NULL, ",", &endStr ) ;
		}	
	}	
} ;
			
			
type: INT { $$ = Int ; codeGenType = Int ;} | DOUBLE { $$ = Double ; codeGenType = Double ;}  | FLOAT { $$ = Float ; codeGenType = Float ;}  | STRING { $$ = String ; codeGenType = String ;}  | BOOL { $$ = Bool ; codeGenType = Bool; } ; 
	
			
			
initial_ID_list: initial_ID_list ',' initial_ID { $$ = concateString(3, $1, ",", $3); }
				| initial_ID { $$ = $1 ; }
				;

initial_ID: ID 
			{
				$$ = $1; 
				variableInserter( $1, variable, codeGenType, 0 ) ;
				genVariable( codeGenType, $1 ) ; 
			}
			
			| ID ASSIGN expression 
			{ 
				$$ = concateString(3, $1, "=", $3.lexVal); 
				variableInserter( $1, variable, codeGenType, 0 ) ;
				genVariable( codeGenType, $1 ) ;
				genExpression(); 
				genStatement( $1, $3.lexVal ) ;
			}
			
			| ID dimension_declaration {  $$ = concateString(2, $1, $2) ;}
			| ID dimension_declaration ASSIGN '{' expression_list '}' {  $$ = concateString(4, $1, $2, "=", $5 ) ; }	
			;
	
dimension_declaration: '[' expression ']' { $$ = concateString(3, "[", $2.lexVal, "]") ;   }	
					| dimension_declaration '[' expression ']' { $$ = concateString(4, $1, "[", $3.lexVal, "]") ; }	
					;

function_declaration: 
					type ID '(' parameter_list  ')' '{' 
					{ 
						enterScope() ;
						paramInserter( strdup($4) ) ; 

						genMethodBegin( $2, strdup($4), typeSelector($1) ) ;	
						
						funcCodeGenType = $1 ;	
					} 
					function_body '}'  
					{ 
						checkRetType($2, $1, $8) ; 
						exitScope(Opt_Symbol); 
						functionInserter( $1, $2, $4, DEFINE ) ;  
						noFunctionDefinition = 0; 
						genMethodEnd( 0 ) ;  
					}
					
					| VOID ID '(' parameter_list ')' '{'  
					{ 
						enterScope() ;
						paramInserter(strdup($4)) ; 

						genMethodBegin( $2, strdup($4), 'V' ) ;
  
					} 
					function_body '}'  
					{ 
						checkRetType($2, Void, $8) ; 
						noFunctionDefinition = 0; 
						exitScope(Opt_Symbol);  
						functionInserter( Void, $2, $4, DEFINE ) ; 
						genMethodEnd( 1 ) ;  
					}	
					

					| type ID '(' parameter_list ')' ';' { functionInserter( $1, $2, $4, DECL ) ; }
					| VOID ID '(' parameter_list ')' ';' { functionInserter( Void, $2, $4, DECL ) ; }
					;
					
parameter_list: nonEmptyParamList { $$ = $1 ; reStartParamReg() ;  }
				| /* the function parameters can be empty */ { $$ = "" ; }
				;

nonEmptyParamList:  nonEmptyParamList ',' parameter	{ $$ = concateString(3, $1, ",", $3) ; }				
				| parameter { $$ = $1 ; }
				;	

parameter: type ID  
		{ 
			$$ = concateString(3, anti_converter($1), " ", $2) ; 
		}
		| type ID dimension_declaration { $$ = concateString(4, anti_converter($1), " ", $2, $3) ;  }
		;

function_body: function_body compound { $$ = $2 ; }
			| /* empty body */ { $$ = Void ; }
			;

compound: variable_declaration { $$ = Void ; } | statement { $$ = $1 ; } ;




			
statement: 	'{' { enterScope() ; } function_body '}' { exitScope(Opt_Symbol) ;  $$ = $3 ; } /* compund */		
			| variable_reference ASSIGN expression ';'	
			{ 
				$$ = Void ; 
				statementChecker($1, $3.lexVal) ;  
				genExpression(); 
				genStatement( $1, $3.lexVal ) ; 				
			}	/* assignment */ 
			| PRINT {  genExpression() ;genPrintBegin() ;} expression ';'	
			{ 
				TYPE exprType = printChecker($3.lexVal); 
				$$ = Void ; 
				
				genExpression();  
				genPrintEnd( exprType ) ;
			}					/* print */
			| READ variable_reference ';' 	
			{ 
				readChecker($2); 
				$$ = Void ; 
				genRead( &readInstance, $2 ) ;  
			}			/* read */	
			| ID '(' func_expression_list ')' ';' 
			{ 
				funcChecker( $1, $3 ) ; 
				$$ = Void ; 
				genExpression() ;
				genFunction( $1 ) ;
			}	/* function invocation */
			
			| IF '(' condition_expression ')' 
			statement 
			{ 
				booleanChecker($3); $$ = $5 ;  
				fprintf( outFp, "Lelse_%d:\n", Stack.stack[ Stack.size-1 ] ) ; 	
				Stack.size -- ;	
			}	%prec "then"  /* conditional */
			
			| IF '(' condition_expression ')' 
			statement 
			ELSE 
			{ 
				int curL = Stack.stack[ Stack.size-1 ] ;
				fprintf( outFp, "goto Lexit_%d\nLelse_%d:\n", curL, curL ) ; 
			} 
			statement 
			{ 		
				booleanChecker($3); 
				$$ = ifElseReturnTypeChecker($5, $8) ; 
				fprintf( outFp, "Lexit_%d:\nnop\n", Stack.stack[ Stack.size-1 ] ) ; 	
				Stack.size -- ;				
			}
					
			| WHILE '(' while_condition ')' 
			{ 
				booleanChecker($3); 
				inLoop = 1 ; 
				genExpression() ;
			} 
			statement 
			{ 
				$$ = $6 ; 
				inLoop = 0 ; 
				int curL = Stack.stack[ Stack.size-1 ] ;
				fprintf( outFp, "goto Lbegin_%d\nLexit_%d:\n", curL, curL ) ;
				Stack.size -- ;	
			}	 /* while */
			
			| DO  
			{ 
				inLoop = 1 ; 
				Stack.stack[ Stack.size ++ ] = curLabel++ ;
				fprintf( outFp, "Lbegin_%d:\n", Stack.stack[ Stack.size-1 ] ) ;	
			} 
			statement WHILE '(' expression ')' ';' 
			{ 
				booleanChecker($6.lexVal); 
				$$ = $3 ; 
				inLoop = 0 ; 
				genExpression() ;
				int curL = Stack.stack[ Stack.size-1 ] ;
				fprintf( outFp, "ifeq Lexit_%d\ngoto Lbegin_%d\nLexit_%d:\n", curL, curL, curL ) ;
				Stack.size -- ;
			}	
			
			
			| FOR '(' assign_expr_list ';' expression_list { genExpression(); } ';' increment_expr ')' 
			{ 
				booleanChecker($5); 
				inLoop = 1 ; 
			} 
			statement 
			{ 
				forLoopChecker($3); 
				forLoopChecker($8); 
				$$ = $11 ; 
				inLoop = 0 ; 

				genTmpExpr() ;
		
				int curL = Stack.stack[ Stack.size-1 ] ;
				fprintf( outFp, "goto Lbegin_%d\nLexit_%d:\n", curL, curL ) ;
				Stack.size -- ;					
			}	 /* for */
			
			
			| RETURN expression ';' 
			{  
				if( isBoolean($2.lexVal) && booleanChecker(strdup($2.lexVal)) ) 
					$$ = Bool; 
				else 	
					$$ = typeEvaluater(strdup($2.lexVal)) ;  
				
				genExpression() ;
				genReturn( $2.type ) ;		
			} /* return */
			| BREAK ';' { jumpChecker(inLoop, "break") ;  $$ = Void ; }	 /* break */
			| CONTINUE ';' { jumpChecker(inLoop, "continue") ; $$ = Void ; }	 /* continue */
			;


			
assign_expr_list: nonEmpty_assign_expr_list { $$ = $1; }
				| /* epsilon */ { $$ = "" ; }
				;

nonEmpty_assign_expr_list: nonEmpty_assign_expr_list ',' assign_expr
						| assign_expr { $$ = $1; }
						;

increment_expr: ID ASSIGN expression
			{
				$$ = concateString(3, $1, "=", $3.lexVal) ; 
				genExpr2Tmp() ;
				genStmt2Tmp( $1 ) ;
			}
			;
						
assign_expr: ID ASSIGN expression 
			{ 
				$$ = concateString(3, $1, "=", $3.lexVal) ; 
				genExpression() ; 
				genStatement( $1, $3.lexVal ) ;	
			} 
			;
			
			
expression_list: nonEmptyExprList { $$ = $1 ; }
				| { $$ = "" ; }/*empty expression list*/	
				;
				
nonEmptyExprList: nonEmptyExprList ',' expression { $$ = concateString(3, $1, ",", $3.lexVal) ; }
				| 
				{
					Stack.stack[ Stack.size ++ ] = curLabel++ ;		
					genToList( "Lbegin_%d:", Stack.stack[ Stack.size-1 ] ) ; 
				} 
				expression 
				{ 
					$$ = $2.lexVal ; 
					genToList( "ifeq Lexit_%d", Stack.stack[ Stack.size-1 ] ) ;
				}		
				;

func_expression_list: nonEmptyFuncExprList { $$ = $1 ; }
					| { $$ = "" ; }
					;
					
nonEmptyFuncExprList: nonEmptyFuncExprList ',' expression { $$ = concateString(3, $1, ",", $3.lexVal) ; }
					| expression { $$ = $1.lexVal; } ;
					
					
variable_reference: ID { $$ = $1 ; }
				| ID dimension_declaration { $$ = concateString(2, $1, $2) ; } 
				;
				
/* Expression */	
condition_expression: expression 
			{ 
				$$ = $1.lexVal ; 
				Stack.stack[ Stack.size ++ ] = curLabel++ ;
				genToList( "ifeq Lelse_%d", Stack.stack[ Stack.size-1 ] ) ;
			}

			
while_condition: 
			{
				Stack.stack[ Stack.size ++ ] = curLabel++ ;		
				genToList( "Lbegin_%d:", Stack.stack[ Stack.size-1 ] ) ; 
			} 
			expression			
			{	
				$$ = $2.lexVal ;
				genToList( "ifeq Lexit_%d", Stack.stack[ Stack.size-1 ] ) ;
			}

					
			
			
expression: logic_term { $$.lexVal = $1.lexVal ;  $$.type = $1.type ; }
			| expression OR logic_term 
			{ 
				$$.lexVal = concateString(3, $1.lexVal, "||", $3.lexVal); 
				genOperator(None, '|', None) ;  
			}
			;
			
logic_term: logic_factor { $$.lexVal = $1.lexVal ; $$.type = $1.type ;	 }
	| logic_term AND logic_factor 
	{ 
		$$.lexVal = concateString(3, $1.lexVal, "&&", $3.lexVal); 
		genOperator(None, '&', None) ;  
	}
	;
	
logic_factor: expr 
		{ 		
			$$.lexVal = $1.lexVal ; 
			$$.type = $1.type ;	 
		}
		| expr relation_op expr 
		{ 
			$$.lexVal = concateString(3, $1.lexVal, $2, $3.lexVal) ; 
			genRelOp($1.type, $2, $3.type); 
		}
		;		

relation_op: LESS { $$ = strdup("<");  } 
			| LESSEQ  { $$ = strdup("<="); } 
			| NOTEQ  { $$ = strdup("!="); } | LARGEEQ  { $$ = strdup(">="); }
			| LARGE  { $$ = strdup(">"); }  | EQUAL  { $$ = strdup("=="); } ; 		
		
		
expr: expr PLUS term 
	{ 
		$$.lexVal = concateString(3, $1.lexVal, "+", $3.lexVal) ;  
		opChecker($1.lexVal, $3.lexVal); 
		genOperator($1.type, '+', $3.type) ;  
		$$.type = typeGeneralizer($1.type, $3.type) ;
	}
	| expr MINUS term 
	{ 
		$$.lexVal = concateString(3, $1.lexVal, "-", $3.lexVal) ; 
		opChecker($1.lexVal, $3.lexVal); 
		genOperator($1.type, '-', $3.type) ; 
		$$.type = typeGeneralizer($1.type, $3.type) ;
	}
	| term 
	{ 
		$$.lexVal = $1.lexVal ; 
		$$.type = $1.type ;	
	}	
	;

term: term MUL factor 
	{ 
		$$.lexVal = concateString(3, $1.lexVal, "*", $3.lexVal) ;  
		opChecker($1.lexVal, $3.lexVal); 
		genOperator($1.type, '*', $3.type) ; 		
		$$.type = typeGeneralizer($1.type, $3.type) ;
	}
	| term DIV factor 
	{ 
		$$.lexVal = concateString(3, $1.lexVal, "/", $3.lexVal) ; 
		opChecker($1.lexVal, $3.lexVal); 
		genOperator($1.type, '/', $3.type) ;
		$$.type = typeGeneralizer($1.type, $3.type) ;
	}
	| term MOD factor { $$.lexVal = concateString(3, $1.lexVal, "%", $3.lexVal) ; moduleChecker($1.lexVal, $3.lexVal) ; genOperator(None, '%', None) ;  }
	| factor 
	{ 
		$$.lexVal = $1.lexVal ; 
		$$.type = $1.type ;	
	}
	;

factor: prefixOperator originalFactor 
		{ 
			$$.lexVal = concateString(2, $1, $2.lexVal); 					
			$$.type = $2.type ;
			
			switch( type )
			{
				case EXPR:
					genExpression(); 
					break ;
					
				case CONSTANT:
					genConstant( $2.lexVal ); 
					break ;
					
				case FUNC:
					genExpression() ;
					genFunction( getFuncID(strdup($2.lexVal)) ) ;
					break ;
				
				case VAR:
					genVarRef( $2.lexVal ) ; 
					break ;
			} ;
				
			genPreOp( $1 ) ;
		}  ;

prefixOperator: MINUS { $$ = "-" ; }
			| NOT { $$ = "!" ; }
			| /* epsilon */ { $$ = "" ; }	
			;

originalFactor: '(' expression ')' 
				{ 
					$$.lexVal = $2.lexVal ; 
					$$.type = $2.type ;
					type = EXPR; 
				}
				
				| constant 
				{ 
					$$.lexVal = $1 ; 
					$$.type = typeEvaluater( $1 ) ;
					type = CONSTANT ;
				} 
				
				| ID '(' func_expression_list ')' 
				{ 
					$$.lexVal = concateString(4, $1, "(", $3, ")") ; 
					char* func = strdup( $1 ) ;
					strcat( func, "(" ) ;
					$$.type = typeEvaluater( func ) ;
					if( $$.type == None )
						$$.type = funcCodeGenType ;
					
					funcChecker( $1, $3 ) ; 
					type = FUNC ; 
				}
				
				| variable_reference 
				{
					$$.lexVal = $1 ; 
					$$.type = typeEvaluater( $1 ) ;
					type = VAR ;
				}				
				;
				
%%

int yyerror(char* mesg)
{
	fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );	
	fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
	fprintf( stderr, "|\n" );
	fprintf( stderr, "| Unmatched token: %s\n", yytext );
	fprintf( stderr, "|--------------------------------------------------------------------------\n" );
	exit(-1);
}


void completeMesg()
{
	fprintf( stdout, "\n" );
	fprintf( stdout, "|---------------------------------------------|\n" );
	fprintf( stdout, "|  There is no syntactic and semantic error!  |\n" );
	fprintf( stdout, "|---------------------------------------------|\n" );	
}


void CminusCompiler(FILE* fp)
{
	genProgram() ;
		
	/*** for statistical purpose ***/ 
	tableInitializer( 1 ) ;
	
	/*** for symbol tables ***/
	symStackInitializer( 1 ) ; 
	
	/*** for function list ***/
	funcDeclListInitializer( 1 ) ;
	
	/*** for global symbol table ***/
	pushSymTable() ;
	
	yyin = fp;
	yyparse();

	if( noFunctionDefinition )
	{
		fprintf( stderr, "Error: no function definition exists\n" ) ;
		exit(0) ;	
	}	
			
	funcMapChecker() ;
	
	exitScope( Opt_Symbol ) ;
	
	symStackFreer( &symStack ) ;
	
	if( Opt_Statistic )	
		tablePrinter() ;
	tableFreer( &IDtable ) ;
	
	if( !hasSemanticError )
		completeMesg() ;
	else
	{	
		fclose( outFp ) ;
		strcat( outFile, ".j" ) ;
		unlink( outFile ) ;
	}		
}


int main(int argc, char **argv)
{
	if( argc == 2 )
	{	
		outFile = strdup( DEFAULTOUT ) ;
		outFp = fopen( DEFAULTOUT, "w" ) ;
	}
	else if( argc == 3 )
	{
		outFile = strdup( argv[2] )  ;
		outFp = fopen( argv[2], "w" ) ;
	}
	else
	{
		printf("Usage: %s [source file] [target file]\n", argv[0]);
		exit(0);	
	}	

	FILE *fp = fopen(argv[1], "r");
	if( fp == NULL || outFp == NULL )  
	{
		printf("Open file error\n");
		exit(-1);
	}
	
	CminusCompiler( fp ) ;

	return 0 ;
}
