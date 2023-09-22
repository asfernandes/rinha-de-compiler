grammar Rinha;

// Parser rules

root
	: term EOF		#rootRule
	;

term
	: logical									#termLogicalRule
	| '(' term ',' term ')'						#termTupleRule
	| LET reference '=' value=term ';'
		next=term								#termLetRule
	| IF '(' condition=term ')'
		'{' then=term '}'
		ELSE
		'{' otherwise=term '}'					#termIfRule
	| FN '(' reference? (',' reference)* ')'
		'=>' term								#termFnRule
	| '{' term '}'								#termTermRule
	;

logical
	: arithmetic					#logicalArithmeticRule
	| arithmetic
		op=(
			'&&' |
			'||' |
			'==' |
			'!=' |
			'<=' |
			'>=' |
			'<' |
			'>'
		)
		logical						#logicalOpRule
	;

arithmetic
	: factor								#arithmeticFactorRule
	| factor op=('+' | '-') arithmetic		#arithmeticOpRule
	;

factor
	: apply									#factorApplyRule
	| apply op=('*' | '/' | '%') factor		#factorOpRule
	;

apply
	: primary							#applyPrimaryRule
	| call								#applyCallRule
	| apply '(' term? (',' term)* ')'	#callApplyRule
	;

primary
	: '(' term ')'	#primaryTermRule
	| TRUE			#primaryTrueRule
	| FALSE			#primaryFalseRule
	| INT			#primaryIntRule
	| STRING		#primaryStringRule
	| reference		#primaryVarRule
	;

call
	: PRINT '(' term ')'	#callPrintRule
	| FIRST '(' term ')'	#callFirstRule
	| SECOND '(' term ')'	#callSecondRule
	;

reference
	: TEXT
	;

// Lexer rules

ELSE : 'else' ;
FALSE : 'false' ;
FIRST : 'first' ;
FN : 'fn' ;
IF : 'if' ;
LET : 'let' ;
PRINT : 'print' ;
SECOND : 'second' ;
TRUE : 'true' ;

TEXT : [a-zA-Z{}][a-zA-Z{}0-9_]* | '_' ;
INT : ('0'..'9')+;
STRING : '"' (STRING_ESC | ~["\\])* '"' ;

BLOCK_COMMENT : '/*' .*? '*/' -> channel(HIDDEN) ;
LINE_COMMENT : '//' ~[\r\n]* -> channel(HIDDEN) ;

WS: [ \t\n\r]+ -> skip ;

fragment STRING_ESC : '\\' ["\\] ;
