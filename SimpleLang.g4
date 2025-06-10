grammar SimpleLang;

program
    : declarationFunction* EOF
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
    : expression ';'
    | block
    | statementVariableDeclaration
    | statementVariableAssignment
    | statementReturn
    | statementIf
    | statementWhile
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

expressionList
    : expression (',' expression)*
    ;

expression
    : expression op=('==' | '!=' | '<' | '>' | '<=' | '>=') expression  # ExpressionComparison
    | expression op=('+' | '-') expression                              # ExpressionAddSubtract
    | expression op=('*' | '/') expression                              # ExpressionMultiplyDivide
    | op=('!' | '-') expression                                         # ExpressionUnary
    | ID '(' expressionList? ')'                                        # ExpressionCallFunction
    | '(' expression ')'                                                # ExpressionParentheses
    | ID                                                                # ExpressionVariableReference
    | literal                                                           # ExpressionLiteral
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