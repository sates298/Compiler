%code requires{
  #include "headers/bison_declarations.hpp"
}

%union {
  long long    int64;
  std::string  *str;

  Command      *cmd;
  Multicommand *mcmd;
  Expression   *expr;
  Condition   *cond;
  Call         *cal;
  Value        *val;
}

// VARIABLES-CONSTANTS
%token NUM PIDENTIFIER

// PROGRAM
%token PDECLARE PBEGIN PEND

// COMMANDS
%token ASSIGN IF THEN ELSE ENDIF WHILE DO ENDWHILE ENDDO FOR FROM TO DOWNTO ENDFOR READ WRITE

// EXPRESSION
%token PLUS MINUS TIMES DIV MOD

// CONDITIONS
%token EQ NEQ LE GE LEQ GEQ

%type<int64>  number;
%type<str>    name;

%type<cal>    identifier;
%type<val>    value;
%type<mcmd>   commands;
%type<cmd>    command;
%type<expr>   expression;
%type<cond>   condition; 
%%
input:
    program 
  | error {error("Unrecognized character ", yylineno, true);}
;

program:  
    PDECLARE declarations PBEGIN commands PEND {handleProgram($4); delete $4;}
  | PBEGIN commands PEND {handleProgram($2); delete $2;}
;

declarations:   
    declarations ',' name                           { handleVariableDeclaration(*$3); delete $3;}
  | declarations ',' name '(' number ':' number ')' { handleArrayDeclaration(*$3, $5, $7); delete $3;}
  | name                                            { handleVariableDeclaration(*$1); delete $1;}
  | name '(' number ':' number ')'                  { handleArrayDeclaration(*$1, $3, $5); delete $1;}
;

commands:   
    commands command  {$$ = handleRecursiveCommands($1, $2); delete $2;}
  | command           {$$ = handleCommand($1); delete $1;}
;

command:  
    identifier ASSIGN expression ';'                    {$$ = handleAssign(*$1, *$3); delete $1; delete $3;}
  | IF condition THEN commands ELSE commands ENDIF      {$$ = handleIfElse(*$2, $4, $6); delete $2; delete $4; delete $6;}
  | IF condition THEN commands ENDIF                    {$$ = handleIf(*$2, $4); delete $2; delete $4;}
  | WHILE condition DO commands ENDWHILE                {$$ = handleWhile(*$2, $4); delete $2; delete $4;}
  | DO commands WHILE condition ENDDO                   {$$ = handleDoWhile(*$4, $2); delete $2; delete $4;}
  | FOR name FROM value TO value DO commands ENDFOR     {$$ = handleFor(*$2, *$4, *$6, $8); delete $2; delete $4; delete $6; delete $8;}
  | FOR name FROM value DOWNTO value DO commands ENDFOR {$$ = handleForDown(*$2, *$4, *$6, $8); delete $2; delete $4; delete $6; delete $8;}
  | READ identifier ';'                                 {$$ = handleRead(*$2); delete $2;}
  | WRITE value ';'                                     {$$ = handleWrite(*$2); delete $2;}
;

expression:   
    value             {$$ = handleExpression(*$1, Value(), ENULL); delete $1;}
  | value PLUS value  {$$ = handleExpression(*$1,*$3,EPLUS); delete $1;delete $3;}
  | value MINUS value {$$ = handleExpression(*$1,*$3,EMINUS); delete $1;delete $3;}
  | value TIMES value {$$ = handleExpression(*$1,*$3,ETIMES); delete $1;delete $3;}
  | value DIV value   {$$ = handleExpression(*$1,*$3,EDIV); delete $1;delete $3;}
  | value MOD value   {$$ = handleExpression(*$1,*$3,EMOD); delete $1;delete $3;}
;

condition:  
    value EQ value  {$$ = handleCondition(*$1, *$3, CEQ);delete $1; delete $3;}
  | value NEQ value {$$ = handleCondition(*$1, *$3, CNEQ);delete $1; delete $3;}
  | value LE value  {$$ = handleCondition(*$1, *$3, CLE);delete $1; delete $3;}
  | value GE value  {$$ = handleCondition(*$1, *$3, CGE);delete $1; delete $3;}
  | value LEQ value {$$ = handleCondition(*$1, *$3, CLEQ);delete $1; delete $3;}
  | value GEQ value {$$ = handleCondition(*$1, *$3, CGEQ);delete $1; delete $3;}
;

value:  
    number      {$$ = handleValueNumber($1);}
  | identifier  {$$ = handleValueIdentifier(*$1); delete $1;}
;

identifier:
    name                {$$ = handleName(*$1); delete $1;}
  | name '(' name ')'   {$$ = handleArrayByName(*$1, *$3); delete $1; delete $3;}
  | name '(' number ')' {$$ = handleArrayByNumber(*$1, $3); delete $1;}
;

name:
  PIDENTIFIER {$$ = yylval.str;}
;

number:
  NUM   {$$ = yylval.int64;}
;
%%

void yyerror(const char * c){}