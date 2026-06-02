grammar ifcc;

axiom
    : prog+ EOF
    ;

prog
    : TYPE VARIABLE BRACKET_OPEN function_variable_creation? BRACKET_CLOSE bloc
    ;

bloc
    : CURLY_BRACE_OPEN statement+ CURLY_BRACE_CLOSE
    ;

statement
    : instruction SEMI_COLON
    | return_statement SEMI_COLON
    | variable_creation SEMI_COLON
    ;

variable_creation
    : TYPE variable_creation_assignement (COMMA variable_creation_assignement)*
    ;

variable_creation_assignement
    : VARIABLE EQUAL instruction #variable_creation_with_initialization
    | VARIABLE                   #variable_creation_without_initialization
    ;

function_variable_creation
    : TYPE VARIABLE (COMMA TYPE VARIABLE )*
    ;

instruction
    : (VARIABLE EQUAL)? expression
    ;

expression : op=(MINUS | NOT) expression                         # unary_operation
           | expression op=(TIMES | DIVIDE | MODULO) expression  # multiplicative_expression
           | expression op=(PLUS | MINUS) expression             # additive_expression
           | expression op=(LE | GE | LT | GT) expression        # comp_expression
           | expression op=(EQ | NE) expression                  # diff_expression
           | BRACKET_OPEN instruction BRACKET_CLOSE              # bracketed_expression
           | CONSTANT                                            # constant_expression
           | SIMPLE_CHAR                                         # simple_char
           | VARIABLE                                            # variable_expression
           ;

return_statement
    : RETURN expression
    ;

RETURN : 'return' ;
TYPE : 'int' | 'double' | 'void' ;

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
NOT               : '!' ;
LE                : '<=' ;
GE                : '>=' ;
LT                : '<' ;
GT                : '>' ;
EQ                : '==' ;
NE                : '!=' ;

VARIABLE  : [a-zA-Z_][a-zA-Z0-9_]* ;
SIMPLE_CHAR : '\'' . '\'' ;
CONSTANT : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
