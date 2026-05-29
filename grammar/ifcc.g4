grammar ifcc;

axiom : prog EOF ;

prog : INT 'main' '(' ')' '{' statement+ '}' ;

statement: variable_assignment
    | return_statement
    ;

variable_assignment: variable_creation
    | VAR '=' expression ';'
    ;

variable_creation: INT VAR '=' expression ';'
    | INT VAR ';'
    ;

expression : CONST
    | VAR
    ;

return_statement: RETURN expression ';' ;

INT : 'int' ;
RETURN : 'return' ;
CONST : [0-9]+ ;
VAR : [a-zA-Z_][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
