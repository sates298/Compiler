%{
 #include "headers/main.hpp"
%}
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

%%
input:
    program
  | error {error("Unrecognized character ", yylineno); return -1;}
;

program:  
    PDECLARE declarations PBEGIN commands PEND
  | PBEGIN commands PEND
;

declarations:   
    declarations ',' PIDENTIFIER
  | declarations ',' PIDENTIFIER '(' NUM ':' NUM ')'
  | PIDENTIFIER
  | PIDENTIFIER '(' NUM ':' NUM ')'
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
  | FOR PIDENTIFIER FROM value TO value DO commands ENDFOR
  | FOR PIDENTIFIER FROM value DOWNTO value DO commands ENDFOR
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
    NUM
  | identifier
;

identifier:
    PIDENTIFIER
  | PIDENTIFIER '(' PIDENTIFIER ')'
  | PIDENTIFIER '(' NUM ')'
;
%%