build:
	gcc main.c -o meu_cliente -O2
	# make test

test:
	./meu_cliente unb.br 8.8.8.8
	./meu_cliente imagdaskdasdasj.br 1.1.1.1
	./meu_cliente fga.unb.br 8.8.8.8
	./meu_cliente unb.br 1.2.3.4
