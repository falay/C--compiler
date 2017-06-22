%{
#include "symTable.h"
#include "semantic.h"	
#include "y.tab.h"
#include "statistic.h"


#define LIST                { strncat(buf, yytext, sizeof(buf) - strlen(buf) - 1); }
#define token(t)            { LIST; if (Opt_Token) printf("<\"%s\">\n", t); }
#define tokenString(t, s)   { LIST; if (Opt_Token) printf("<%s:%s>\n", t, s); }
#define APPEND(s)			{ strcat(buf, s) ; }

#define optionHandler(s)	{\
	if( strcmp(s, "#pragma source on") == 0 ) \
		Opt_Source = 1 ; \
	else if( strcmp(s, "#pragma source off") == 0 ) \
		Opt_Source = 0 ; \
	else if( strcmp(s, "#pragma token on") == 0 ) \
		Opt_Token = 1 ; \
	else if( strcmp(s, "#pragma token off") == 0 ) \
		Opt_Token = 0 ; \
	else if( strcmp(s, "#pragma statistic on") == 0 ) \
		Opt_Statistic = 1 ; \
	else if( strcmp(s, "#pragma statistic off") == 0 ) \
		Opt_Statistic = 0 ; \
	else if( strcmp(s, "#pragma symbol on") == 0 ) \
		Opt_Symbol = 1 ; \
	else if( strcmp(s, "#pragma symbol off") == 0 ) \
		Opt_Symbol = 0 ; \
}		




#define MAX_LINE_LENGTH 257
#define MAX_ID_LENGTH 257
#define MAX_STRING_LENGTH 257

int Opt_Source = 0;
int Opt_Token = 0;
int Opt_Statistic = 0;
int Opt_Symbol = 0 ;

int linenum = 1;
char buf[MAX_LINE_LENGTH];
char strContent[MAX_STRING_LENGTH];
%}

letter [a-zA-Z]
digit  [0-9]
positiveDigit [1-9]
space  [ \t\r]+

%START COMMENT COMMENT2 STR STR_ESCAPE

%%


 
"#pragma source on"|"#pragma source off"|"#pragma token on"|"#pragma token off"|"#pragma statistic on"|"#pragma statistic off"|"#pragma symbol on"|"#pragma symbol off" {
	APPEND(yytext) ;
	optionHandler(yytext) ;
}


<INITIAL>"/*" {
	APPEND("/*") ;	
	BEGIN COMMENT ;
}

<COMMENT>.  LIST ;

<COMMENT>"*/" {
	APPEND("*/")
	BEGIN INITIAL ;
}

<INITIAL>"//" {
	APPEND("//") ;
	BEGIN COMMENT2 ;	
}

<COMMENT2>. LIST ;

<COMMENT2>"\n" {
	BEGIN INITIAL ;
	REJECT ;	
}


<INITIAL>"," {
    tokenString("delim", yytext); 
	return ',' ;
}

<INITIAL>";" {
	tokenString("delim", yytext); 
	return ';' ;
}

<INITIAL>"(" {
	tokenString("delim", yytext); 
	return '(' ;
}

<INITIAL>")" {
	tokenString("delim", yytext); 
	return ')' ;
}

<INITIAL>"[" {
	tokenString("delim", yytext); 
	return '[' ;
}

<INITIAL>"]" {
	tokenString("delim", yytext); 
	return ']' ;
}

<INITIAL>"{" {
	tokenString("delim", yytext); 
	return '{' ;
}

<INITIAL>"}" {
	tokenString("delim", yytext); 
	return '}' ;
}



<INITIAL>"+" {
	token(yytext) ;
	return PLUS ;
}

<INITIAL>"-" {
	token(yytext) ;
	yylval.text = "-" ;
	return MINUS ;
}

<INITIAL>"*" {
	token(yytext) ;
	return MUL ;
}

<INITIAL>"/" {
	token(yytext) ;
	return DIV ;
}

<INITIAL>"%" {
	token(yytext) ;
	return MOD ;
}

<INITIAL>"=" {
	token(yytext) ;
	return ASSIGN ;
}

<INITIAL>"<" {
	token(yytext) ;
	return LESS ;
}

<INITIAL>"<=" {
	token(yytext) ;
	return LESSEQ ;
}

<INITIAL>"!=" {
	token(yytext) ;
	return NOTEQ ;
}

<INITIAL>">=" {
	token(yytext) ;
	return LARGEEQ ;
}

<INITIAL>">" {
	token(yytext) ;
	return LARGE ;
}

<INITIAL>"==" {
	token(yytext) ;
	return EQUAL ;
}

<INITIAL>"&&" {
	token(yytext) ;
	return AND ;
}

<INITIAL>"||" {
	token(yytext) ;
	return OR ;
}

<INITIAL>"!" {
	token(yytext) ;
	return NOT ;
}


<INITIAL>"while" {
	tokenString("KW", yytext) ;
	return WHILE ;
}

<INITIAL>"do" {
	tokenString("KW", yytext) ;
	return DO ;
}

<INITIAL>"if" {
	tokenString("KW", yytext) ;
	return IF ;
}

<INITIAL>"else" {
	tokenString("KW", yytext) ;
	return ELSE ;
}

<INITIAL>"true" {
	tokenString("KW", yytext) ;
	
	yylval.text = strdup( yytext ) ;
	
	return TRUE ;
}

<INITIAL>"false" {
	tokenString("KW", yytext) ;
	
	yylval.text = strdup( yytext ) ;
	
	return FALSE ;
}

<INITIAL>"for" {
	tokenString("KW", yytext) ;
	return FOR ;
}

<INITIAL>"int" {
	tokenString("KW", yytext) ;
	return INT ;
}

<INITIAL>"print" {
	tokenString("KW", yytext) ;
	return PRINT ;
}

<INITIAL>"const" {
	tokenString("KW", yytext) ;
	return CONST ;
}

<INITIAL>"read" {
	tokenString("KW", yytext) ;
	return READ ;
}

<INITIAL>"boolean" {
	tokenString("KW", yytext) ;
	return BOOLEAN ;
}

<INITIAL>"bool" {
	tokenString("KW", yytext) ;
	return BOOL ;
}

<INITIAL>"void" {
	tokenString("KW", yytext) ;
	return VOID ;
}

<INITIAL>"float" {
	tokenString("KW", yytext) ;
	return FLOAT ;
}

<INITIAL>"double" {
	tokenString("KW", yytext) ;
	return DOUBLE ;
}

<INITIAL>"string" {
	tokenString("KW", yytext) ;
	return STRING ;
}

<INITIAL>"continue" {
	tokenString("KW", yytext) ;
	return CONTINUE ;
}

<INITIAL>"break" {
	tokenString("KW", yytext) ;
	return BREAK ;
}

<INITIAL>"return" {
	tokenString("KW", yytext) ;
	return RETURN ;
}



<INITIAL>{positiveDigit}{digit}*|"0" {
	tokenString("integer", yytext) ;
	
	yylval.text = strdup( yytext ) ;
	
	return INT_CONST ;
}

<INITIAL>{digit}+"."{digit}+ {
	tokenString("float", yytext) ;	
	
	yylval.text = strdup( yytext ) ;
	
	return FLOAT_CONST ;
}


<INITIAL>{letter}({letter}|{digit})* {
	tokenString("id", yytext) ;

	if( Opt_Statistic )
		tableInserter( yytext ) ;

	yylval.text = strdup( yytext ) ;
	
	return ID ;
}

<INITIAL>"\""	{ 
	LIST;
    strContent[0] = '\0';
	BEGIN STR;
}
	
<STR>"\\"	{
	LIST;
	BEGIN STR_ESCAPE;
}
	
<STR_ESCAPE>\"	{
	LIST;
	strncat(strContent, "\"", MAX_STRING_LENGTH);
	BEGIN STR;
}

<STR_ESCAPE>n	{
	LIST;
	strncat( strContent, "\\n", MAX_STRING_LENGTH);
	BEGIN STR;
}

<STR_ESCAPE>t	{
	LIST;
	strncat( strContent, "\\t", MAX_STRING_LENGTH);
	BEGIN STR;
}

<STR_ESCAPE>\\	{
	LIST;
	strncat( strContent, "\\", MAX_STRING_LENGTH);
	BEGIN STR;
}

<STR_ESCAPE>\n  {
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

<STR_ESCAPE>[^\"nt\\]	{
	LIST;
	strncat( strContent, "\\", MAX_STRING_LENGTH);
	strncat( strContent, yytext, MAX_STRING_LENGTH);
	BEGIN STR;
}

<STR>"\n"	{
    fprintf(stderr,"Error at line %d: %s\n",linenum,yytext);
	exit(1);
}

<STR>\"	{
	tokenString("string", strContent);
	BEGIN INITIAL;
	
	yylval.text = strdup( strContent ) ;
	
	return STRING_CONST ;
}

<STR>[^\\\n\"]	{
	LIST;
	strncat( strContent, yytext, MAX_STRING_LENGTH);
}

<INITIAL>([0-9]+|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)	{
	tokenString("scientific", yytext);
	
	yylval.text = strdup( yytext ) ;
	
	return SCIENTICFIC_CONST ;
}

\n {
    LIST;
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
}

<<EOF>> {
    LIST;
    if (Opt_Source)
        printf("%d:%s\n\n", linenum, buf);
	return 0 ;	
}

{space} {
	LIST;
}

<INITIAL>. { // Unrecognized character
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

%%
