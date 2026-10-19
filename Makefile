all: main.c Source/busca.c Source/globo.c Source/mapa.c
	gcc main.c -o PAA Source/busca.c Source/globo.c Source/mapa.c
run:
	PAA