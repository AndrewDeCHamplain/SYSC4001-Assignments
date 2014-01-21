all:
	gcc producer1.c -o pro1 -lrt -lpthread -w
	gcc consumer1.c -o con1 -lrt -lpthread -w
	gcc producer2.c -o pro2 -lrt -lpthread -w
	gcc consumer2.c -o con2 -lrt -lpthread -w
	./kill_ipcs.sh
