default:
	lex tarnish.lex
	yacc -d tarnish.y
	gcc tarnish.int.c symbols.c lex.yy.c y.tab.c -o tarnish -ll -ly

clean:
	rm lex.yy.c y.tab.c y.tab.h tarnish
