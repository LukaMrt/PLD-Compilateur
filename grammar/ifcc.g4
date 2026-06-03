grammar ifcc;

axiom
    : function+ EOF
    ;

function
    : TYPE IDENTIFIER BRACKET_OPEN function_variable_declaration? BRACKET_CLOSE block
    ;

function_variable_declaration
    : TYPE IDENTIFIER (COMMA TYPE IDENTIFIER )*
    ;

block
    : CURLY_BRACE_OPEN statement* CURLY_BRACE_CLOSE
    ;

statement
    : instruction SEMI_COLON
    | variable_declaration SEMI_COLON
    | return_statement SEMI_COLON
    | IF BRACKET_OPEN expression BRACKET_CLOSE (statement | block | SEMI_COLON) (ELSE (statement | block | SEMI_COLON))?
    | WHILE BRACKET_OPEN expression BRACKET_CLOSE (statement | block | SEMI_COLON)
    ;

variable_declaration
    : TYPE variable_definition (COMMA variable_definition)*
    ;

variable_definition
    : IDENTIFIER EQUAL instruction #variable_definition_with_instruction
    | IDENTIFIER                   #variable_definition_without_instruction
    ;

instruction
    : (IDENTIFIER EQUAL)* expression
    ;

expression
    : op=(MINUS | NOT) expression                                             # unary_operation
    | expression op=(TIMES | DIVIDE | MODULO) expression                      # multiplicative_expression
    | expression op=(PLUS | MINUS) expression                                 # additive_expression
    | expression op=(LE | GE | LT | GT) expression                            # comp_expression
    | expression op=(EQ | NE) expression                                      # diff_expression
    | expression op=(AND | XOR | OR) expression                               # bitwise_expression
    | BRACKET_OPEN instruction BRACKET_CLOSE                                  # bracketed_expression
    | CONSTANT                                                                # constant_expression
    | SIMPLE_CHAR                                                             # simple_char
    | IDENTIFIER                                                              # variable_expression
    | IDENTIFIER BRACKET_OPEN (expression (COMMA expression)*)? BRACKET_CLOSE # function_call
    ;

return_statement
    : RETURN expression
    ;

RETURN : 'return' ;
TYPE
    : 'int'
    | 'double'
    | 'void'
    ;
IF : 'if' ;
ELSE : 'else' ;
WHILE : 'while' ;

LE                : '<=' ;
GE                : '>=' ;
LT                : '<' ;
GT                : '>' ;
EQ                : '==' ;
NE                : '!=' ;
OR                : '|' ;
AND               : '&' ;
XOR               : '^' ;
NOT               : '!' ;
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

WS    : [ \t\r\n] -> channel(HIDDEN);
COMMENT : '/*' .*? '*/' -> skip ;
CONSTANT : [0-9]+ ;
DIRECTIVE : '#' .*? '\n' -> skip ;
IDENTIFIER  : [a-zA-Z_][a-zA-Z0-9_]* ;
SIMPLE_CHAR : '\'' . '\'' ;
