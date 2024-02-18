CC := gcc

all: sup.out cus.out res.out


cus.out: cus.o loger.o recipe.o socket_funcs.o std_msg.o system_environment.o
	$(CC) cus.o loger.o recipe.o socket_funcs.o std_msg.o system_environment.o -o cus.out

res.out: res.o loger.o recipe.o socket_funcs.o std_msg.o system_environment.o
	$(CC) res.o loger.o recipe.o socket_funcs.o std_msg.o system_environment.o -o res.out

sup.out: sup.o loger.o recipe.o socket_funcs.o std_msg.o system_environment.o
	$(CC) sup.o loger.o recipe.o socket_funcs.o std_msg.o system_environment.o -o sup.out


cus.o: customer.c *.h
	$(CC) -c customer.c -o cus.o

res.o: restaurant.c *.h
	$(CC) -c restaurant.c -o res.o

sup.o: supplier.c *.h
	$(CC) -c supplier.c -o sup.o

loger.o: loger.c *.h
	$(CC) -c loger.c -o loger.o

recipe.o: recipe.c *.h
	$(CC) -c recipe.c -o recipe.o

socket_funcs.o: socket_funcs.c *.h
	$(CC) -c socket_funcs.c -o socket_funcs.o

std_msg.o: std_msg.c *.h
	$(CC) -c std_msg.c -o std_msg.o

system_environment.o: system_environment.c *.h
	$(CC) -c system_environment.c -o system_environment.o


clean:
	rm *.o *.out *.log