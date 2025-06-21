grammar SimpleLang;

program
    : (declarationFunction)* EOF
    ;

declarationFunction
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
    | ID
    | literal
    ;

expressionCallFunction
    : ID '(' expressionList? ')'
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
    | 'ivec2'
    | 'vec2'
    ;

FLOAT  : [0-9]+ '.' [0-9]*;
INT    : [0-9]+;
BOOL   : 'true'|'false';
STRING : '"' (~["\r\n])* '"' ;
ID   : [a-z_][a-z0-9_]*;

WHITESPACE : [ \t\r\n]+ -> skip;
COMMENT:   '//' ~[\r\n]* -> skip;