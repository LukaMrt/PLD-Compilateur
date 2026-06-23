grammar ifcc;

axiom
    : function+ EOF
    ;

function
    : TYPE TIMES* IDENTIFIER PARENTHESES_OPEN function_parameters_declaration? PARENTHESES_CLOSE block
    ;

function_parameters_declaration
    : function_parameter_declaration (COMMA function_parameter_declaration)*
    ;

function_parameter_declaration
    : TYPE TIMES* IDENTIFIER (EQUAL expression)?
    ;

block
    : CURLY_BRACE_OPEN statement* CURLY_BRACE_CLOSE
    ;

following_condition
    : statement
    | block
    | SEMI_COLON
    ;

statement
    : assignment SEMI_COLON
    | expression SEMI_COLON
    | variable_declaration SEMI_COLON
    | return_statement SEMI_COLON
    | IF PARENTHESES_OPEN expression PARENTHESES_CLOSE following_condition (ELSE following_condition)?
    | WHILE PARENTHESES_OPEN expression PARENTHESES_CLOSE following_condition
    | SEMI_COLON
    ;

variable_declaration
    : TYPE variable_definition (COMMA variable_definition)*
    ;

variable_definition
    : IDENTIFIER BRACKET_OPEN CONSTANT? BRACKET_CLOSE (EQUAL table_init)? #table_definition
    | left_value EQUAL expression #variable_definition_with_instruction
    | left_value                   #variable_definition_without_instruction
    ;
    
table_init
    : CURLY_BRACE_OPEN expression (COMMA expression)* CURLY_BRACE_CLOSE    
    ;

left_value
    : TIMES* IDENTIFIER
    | IDENTIFIER BRACKET_OPEN expression BRACKET_CLOSE
    ;

assignment
    : left_value EQUAL expression
    ;

expression
    : op=(MINUS | NOT | BITWISE_AND | TIMES) expression                                # unary_operation
    | expression op=(TIMES | DIVIDE | MODULO) expression                               # multiplicative_expression
    | expression op=(PLUS | MINUS) expression                                          # additive_expression
    | expression op=(LESSER | LESSER_OR_EQUAL | GREATER_OR_EQUAL | GREATER) expression # comparison_expression
    | expression op=(COMPARE_EQUAL | NOT_EQUAL) expression                             # equal_expression
    | expression BITWISE_AND expression                                                # bitwise_and_expression
    | expression BITWISE_XOR expression                                                # bitwise_xor_expression
    | expression BITWISE_OR expression                                                 # bitwise_or_expression
    | PARENTHESES_OPEN expression PARENTHESES_CLOSE                                    # bracketed_expression
    | CONSTANT                                                                         # constant_expression
    | CHARACTER                                                                        # character_expression
    | ESCAPED_CHARACTER                                                                # escaped_character_expression
    | IDENTIFIER                                                                       # variable_expression
    | IDENTIFIER BRACKET_OPEN expression BRACKET_CLOSE                                 # table_expression_read_value
    | IDENTIFIER PARENTHESES_OPEN (expression (COMMA expression)*)? PARENTHESES_CLOSE  # function_call
    ;

return_statement
    : RETURN expression
    ;

TYPE
    : 'int'
    | 'double'
    | 'void'
    | 'char'
    ;

// Keywords
IF : 'if' ;
ELSE : 'else' ;
WHILE : 'while' ;
RETURN : 'return' ;

// Arithmetic and logical operators
NOT         : '!' ;
PLUS        : '+' ;
TIMES       : '*' ;
MINUS       : '-' ;
EQUAL       : '=' ;
COMMA       : ',' ;
DIVIDE      : '/' ;
MODULO      : '%' ;
SEMI_COLON  : ';' ;
BITWISE_OR  : '|' ;
BITWISE_AND : '&' ;
BITWISE_XOR : '^' ;

// Comparison operators
LESSER           : '<' ;
GREATER          : '>' ;
NOT_EQUAL        : '!=' ;
COMPARE_EQUAL    : '==' ;
LESSER_OR_EQUAL  : '<=' ;
GREATER_OR_EQUAL : '>=' ;

// Delimiters
PARENTHESES_OPEN      : '(' ;
PARENTHESES_CLOSE     : ')' ;
CURLY_BRACE_OPEN  : '{' ;
CURLY_BRACE_CLOSE : '}' ;
BRACKET_OPEN      : '[' ;
BRACKET_CLOSE     : ']' ;


WS    : [ \t\r\n] -> channel(HIDDEN);
COMMENT : '/*' .*? '*/' -> skip ;
LINE_COMMENT : '//' ~[\r\n]* -> skip ;
CONSTANT : [0-9]+ ;
DIRECTIVE : '#' .*? '\n' -> skip ;
ESCAPED_CHARACTER : '\'\\' . '\'' ;
CHARACTER : '\'' . '\'' ;
IDENTIFIER  : [a-zA-Z_][a-zA-Z0-9_]* ;