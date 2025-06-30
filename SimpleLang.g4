grammar SimpleLang;

program
    : stateBlock? functionDeclaration* EOF
    ;

stateBlock
    : 'state' '{' statementVariableDeclaration* '}'
    ;

typeDeclaration
    : 'struct' TYPE_ID '{' typeVariableDeclaration* '}'
    ;

typeVariableDeclaration
    : type ID ';'
    ;

functionDeclaration
    : type ID '(' argumentList? ')' block
    ;

argumentList
    : argument (',' argument)*
    ;

argument
    : type ID
    ;

block
    : '{' statement* '}'
    ;

statement
    : block
    | statementVariableDeclaration
    | statementVariableAssignment
    | statementTypeVariableAssignment
    | statementReturn
    | statementIf
    | statementWhile
    | statementExpression
    ;

statementVariableDeclaration
    : type ID '=' expression ';'
    ;

statementVariableAssignment
    : ID '=' expression ';'
    ;

statementTypeVariableAssignment
    : ID '.' ID '=' expression ';'
    ;

statementReturn
    : 'return' expression? ';'
    ;

statementIf
    : 'if' '(' expression ')' block
    ;

statementWhile
    : 'while' '(' expression ')' block
    ;

statementExpression
    : expressionCallFunction ';'
    ;

expressionList
    : expression (',' expression)*
    ;

expression
    : expression op=('==' | '!=' | '<' | '>' | '<=' | '>=') expression
    | expression op=('+' | '-') expression
    | expression op=('*' | '/') expression
    | op=('!' | '-') expression
    | '(' expression ')'
    | expressionCallFunction
    | expressionTypeVariableAccess
    | expressionTypeInitializerList
    | ID
    | literal
    ;

expressionCallFunction
    : ID '(' expressionList? ')'
    ;

expressionTypeVariableAccess
    : ID '.' ID
    ;

expressionTypeInitializerList
    : '{' (statementVariableAssignment)* '}'
    ;

literal
    : BOOL
    | INT
    | FLOAT
    | STRING
    ;

type
    : 'void'
    | 'string'
    | 'bool' 
    | 'int'   
    | 'float'
    | TYPE_ID
    ;

FLOAT   : [0-9]+ '.' [0-9]*;
INT     : [0-9]+;
BOOL    : 'true'|'false';
STRING  : '"' (~["\r\n])* '"' ;
ID      : [a-z_][a-z0-9_]*;
TYPE_ID : [A-Z_][a-zA-Z0-9]*;

WHITESPACE : [ \t\r\n]+ -> skip;
COMMENT:   '//' ~[\r\n]* -> skip;