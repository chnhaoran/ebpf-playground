xdp_drop:	
	clang -O2 -g -Wall -target bpf -c xdp_drop.c -o xdp_drop.o

myloader:
	gcc xdp_loader.c -o myloader -lbpf -lxdp