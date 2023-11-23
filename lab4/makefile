CC = g++-12
CFLAGS = -Wall -Wextra -O0 -g

LIB_DIR = /usr/local/lib

main: prime area
	$(CC) $(CFLAGS) -o main main.cpp -L. -lprime -larea

regular_main: prime area
	$(CC) $(CFLAGS) -o regular_main main.cpp libprime.so libarea.so

prime:
	$(CC) $(CFLAGS) -shared -o libprime.so prime.cpp -fPIC
	sudo cp libprime.so $(LIB_DIR)

area:
	$(CC) $(CFLAGS) -shared -o libarea.so area.cpp -fPIC
	sudo cp libarea.so $(LIB_DIR)

clear:
	rm -f *.out *.dll *.so
	sudo rm -f $(LIB_DIR)/libprime.so
	sudo rm -f $(LIB_DIR)/libarea.so

