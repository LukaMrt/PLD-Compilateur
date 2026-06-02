grammar ifcc;

axiom
    : prog EOF
    ;

prog
    : INT MAIN BRACKET_OPEN BRACKET_CLOSE CURLY_BRACE_OPEN statement+ CURLY_BRACE_CLOSE
    ;

statement
    : variable_assignment SEMI_COLON
    | return_statement SEMI_COLON
    ;

variable_assignment
    : variable_creation
    | VARIABLE EQUAL expression
    ;

variable_creation
    : INT variable_creation_assignement (COMMA variable_creation_assignement)*
    ;

variable_creation_assignement
    : VARIABLE EQUAL expression #variable_creation_with_initialization
    | VARIABLE #variable_creation_without_initialization
    ;

expression
    : MINUS expression                                   # unary_minus_operation
    | expression op=(TIMES | DIVIDE | MODULO) expression # multiplicative_expression
    | expression op=(PLUS | MINUS) expression            # additive_expression
    | BRACKET_OPEN expression BRACKET_CLOSE              # bracketed_expression
    | CONSTANT                                           # constant_expression
    | VARIABLE                                           # variable_expression
    ;

return_statement
    : RETURN expression
    ;



INT : 'int' ;

MAIN   : 'main' ;
RETURN : 'return' ;

PLUS              : '+' ;
TIMES             : '*' ;
MINUS             : '-' ;
EQUAL             : '=' ;
COMMA             : ',' ;
DIVIDE            : '/' ;
MODULO            : '%' ;
SEMI_COLON        : ';' ;
BRACKET_OPEN      : '(' ;
BRACKET_CLOSE     : ')' ;
CURLY_BRACE_OPEN  : '{' ;
CURLY_BRACE_CLOSE : '}' ;

WS        : [ \t\r\n] -> channel(HIDDEN);
COMMENT   : '/*' .*? '*/' -> skip ;
CONSTANT  : [0-9]+ ;
VARIABLE  : [a-zA-Z_][a-zA-Z0-9_]* ;
DIRECTIVE : '#' .*? '\n' -> skip ;
