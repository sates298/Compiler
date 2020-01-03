%code requires{
  #include "headers/bison_declarations.hpp"
}

%union {
  long long   int64;
  std::string *str;

  CodeBlock   *block;
  Variable    *var;
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

// %type<block>  command;
%%
input:
    program
  | error {error("Unrecognized character ", yylineno); return -1;}
;

program:  
    PDECLARE declarations PBEGIN commands PEND {/*tree.setRoot(std::move($4)); delete $4;*/}
  | PBEGIN commands PEND {/*tree.setRoot(std::move($2)); delete $2;*/}
;

declarations:   
    declarations ',' name {    }
  | declarations ',' name '(' number ':' number ')' { }
  | name {  }
  | name '(' number ':' number ')' {}
;

commands:   
    commands command
  | command
;

command:  
    identifier ASSIGN expression ';'
  | IF condition THEN commands ELSE commands ENDIF
  | IF condition THEN commands ENDIF
  | WHILE condition DO commands ENDWHILE
  | DO commands WHILE condition ENDDO
  | FOR name FROM value TO value DO commands ENDFOR
  | FOR name FROM value DOWNTO value DO commands ENDFOR
  | READ identifier ';'
  | WRITE value ';'
;

expression:   
    value
  | value PLUS value
  | value MINUS value
  | value TIMES value
  | value DIV value
  | value MOD value
;

condition:  
    value EQ value
  | value NEQ value
  | value LE value
  | value GE value
  | value LEQ value
  | value GEQ value
;

value:  
    number 
  | identifier
;

identifier:
    name
  | name '(' name ')'  
  | name '(' number ')' 
;

name:
  PIDENTIFIER {$$ = yylval.str;}
;

number:
  NUM   {$$ = yylval.int64;}
;
%%

void yyerror(const char *){}