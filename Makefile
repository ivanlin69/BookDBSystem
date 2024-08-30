TARGET_SERVER = bin/BookDBServer
TARGET_CLIENT = bin/BookDBClient
SRC_SERVER = $(wildcard src/server/*.c)
OBJ_SERVER = $(patsubst src/server/%.c, obj/server/%.o, $(SRC_SERVER))

SRC_CLIENT = $(wildcard src/client/*.c)
OBJ_CLIENT = $(patsubst src/client/%.c, obj/client/%.o, $(SRC_CLIENT))


run: clean default
<<<<<<< Updated upstream
	./$(TARGET_SERVER) -f sample.db -n
=======
<<<<<<< HEAD
#	./$(TARGET_SERVER) -f sample.db -n -l -a "The Fountainhead,Ayn Rand,Philosophical fiction,978-0026009102,1943"
	./$(TARGET_SERVER) -f sample.db -p 8080 -n
=======
	./$(TARGET_SERVER) -f sample.db -n
>>>>>>> b313068358aae2fa797e6a8d95a82042e7581fec
>>>>>>> Stashed changes
#	./$(TARGET_CLIENT) -p 8080 -h 127.0.0.1
#	./$(TARGET_CLIENT) -p 8080 -h 127.0.0.1 -a "The Fountainhead,Ayn Rand,Philosophical fiction,978-0026009102,1943"
#	./$(TARGET_CLIENT) -p 8080 -h 127.0.0.1 -a "Meditations,Marcus Aurelius,Philosophy,978-1503280465,2018"

default: $(TARGET_SERVER) $(TARGET_CLIENT)

clean:
	rm -f obj/server/*.o
	rm -f obj/client/*.o
	rm -f bin/*

$(TARGET_SERVER): $(OBJ_SERVER)
	gcc -o $@ $^

$(TARGET_CLIENT): $(OBJ_CLIENT)
	gcc -o $@ $^

obj/server/%.o : src/server/%.c
	gcc -c $< -o $@ -Iinclude

obj/client/%.o : src/client/%.c
	gcc -c $< -o $@ -Iinclude
