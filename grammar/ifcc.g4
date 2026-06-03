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
    | IF BRACKET_OPEN instruction BRACKET_CLOSE (statement | block | SEMI_COLON) (ELSE (statement | block | SEMI_COLON))?
    | WHILE BRACKET_OPEN instruction BRACKET_CLOSE (statement | block | SEMI_COLON)
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
    : op=(MINUS | NOT) expression                                                      # unary_operation
    | expression op=(TIMES | DIVIDE | MODULO) expression                               # multiplicative_expression
    | expression op=(PLUS | MINUS) expression                                          # additive_expression
    | expression op=(LESSER | GREATER_OR_EQUAL | LESSER_OR_EQUAL | GREATER) expression # comparison_expression
    | expression op=(COMPARE_EQUAL | NOT_EQUAL) expression                             # equal_expression
    | expression BITWISE_AND expression.                                               # bitwise_and_expression
    | expression BITWISE_XOR expression.                                               # bitwise_xor_expression
    | expression BITWISE_OR expression.                                                # bitwise_or_expression
    | BRACKET_OPEN instruction BRACKET_CLOSE                                           # bracketed_expression
    | CONSTANT                                                                         # constant_expression
    | CHARACTER                                                                        # character_expression
    | IDENTIFIER                                                                       # variable_expression
    | IDENTIFIER BRACKET_OPEN (expression (COMMA expression)*)? BRACKET_CLOSE          # function_call
    ;

return_statement
    : RETURN expression
    ;

TYPE
    : 'int'
    | 'double'
    | 'void'
    ;

IF : 'if' ;
ELSE : 'else' ;
WHILE : 'while' ;
RETURN : 'return' ;

NOT               : '!' ;
PLUS              : '+' ;
TIMES             : '*' ;
MINUS             : '-' ;
EQUAL             : '=' ;
COMMA             : ',' ;
DIVIDE            : '/' ;
MODULO            : '%' ;
LESSER            : '<' ;
GREATER           : '>' ;
NOT_EQUAL         : '!=' ;
SEMI_COLON        : ';' ;
BITWISE_OR        : '|' ;
BITWISE_AND       : '&' ;
BITWISE_XOR       : '^' ;
BRACKET_OPEN      : '(' ;
COMPARE_EQUAL     : '==' ;
BRACKET_CLOSE     : ')' ;
LESSER_OR_EQUAL   : '<=' ;
GREATER_OR_EQUAL  : '>=' ;
CURLY_BRACE_OPEN  : '{' ;
CURLY_BRACE_CLOSE : '}' ;

WS    : [ \t\r\n] -> channel(HIDDEN);
COMMENT : '/*' .*? '*/' -> skip ;
CONSTANT : [0-9]+ ;
DIRECTIVE : '#' .*? '\n' -> skip ;
CHARACTER : '\'' . '\'' ;
IDENTIFIER  : [a-zA-Z_][a-zA-Z0-9_]* ;
