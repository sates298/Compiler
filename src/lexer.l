%{
    #include "headers/scanner_declarations.hpp"
%}

%x COMMENT
%%
\n      yylineno++;
\[      BEGIN(COMMENT);
<COMMENT>{
    \n  yylineno++;
    \]  BEGIN(INITIAL);
    .   /*ignore*/;
}

[_a-z]+         {yylval.str = new std::string(yytext); return PIDENTIFIER;}
[[:digit:]]+    {yylval.int64 = std::stoll(yytext); return NUM;}
[ \t]+    /*ignore*/;


DECLARE {return PDECLARE;} 
BEGIN   {return PBEGIN;} 
END     {return PEND;} 

ASSIGN      {return ASSIGN;} 
IF          {return IF;} 
THEN        {return THEN;} 
ELSE        {return ELSE;} 
ENDIF       {return ENDIF;} 
WHILE       {return WHILE;} 
DO          {return DO;} 
ENDWHILE    {return ENDWHILE;} 
ENDDO       {return ENDDO;} 
FOR         {return FOR;} 
FROM        {return FROM;} 
TO          {return TO;} 
DOWNTO      {return DOWNTO;} 
ENDFOR      {return ENDFOR;} 
READ        {return READ;} 
WRITE       {return WRITE;} 

PLUS    {return PLUS;} 
MINUS   {return MINUS;} 
TIMES   {return TIMES;} 
DIV     {return DIV;} 
MOD     {return MOD;} 

EQ  {return EQ;}  
NEQ {return NEQ;} 
LE  {return LE;} 
GE  {return GE;} 
LEQ {return LEQ;} 
GEQ {return GEQ;} 

.   {return yytext[0];}
%%

int yywrap(){
    return 1;
}