grammar ifcc;

axiom
    : prog EOF
    ;

prog
    : INT MAIN BRACKET_OPEN BRACKET_CLOSE CURLY_BRASE_OPEN statement+ CURLY_BRASE_CLOSE
    ;

statement
    : variable_assignment SEMI_COLON
    | return_statement SEMI_COLON
    ;

variable_assignment
    : variable_creation
    | VAR EQUAL expression
    ;

variable_creation
    : INT VAR EQUAL expression
    | INT VAR
    ;

expression
    : MINUS expression                           # unary_minus_operation
    | expression TIMES expression                # multiply_expression
    | expression DIVIDE expression               # divide_expression
    | expression MODULO expression               # modulo_expression
    | expression PLUS expression                 # add_expression
    | expression MINUS expression                # subtract_expression
    | BRACKET_OPEN expression BRACKET_CLOSE      # bracketed_expression
    | CONST                                      # const_expression
    | VAR                                        # var_expression
    ;

return_statement
    : RETURN expression ;



INT : 'int' ;

MAIN : 'main' ;
RETURN : 'return' ;

EQUAL : '=' ;
TIMES : '*' ;
DIVIDE : '/' ;
MODULO :  '%' ;
PLUS :  '+' ;
MINUS : '-' ;
BRACKET_OPEN : '(' ;
BRACKET_CLOSE : ')' ;
CURLY_BRASE_OPEN : '{' ;
CURLY_BRASE_CLOSE : '}' ;
SEMI_COLON : ';' ;

CONST : [0-9]+ ;
VAR : [a-zA-Z_][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
