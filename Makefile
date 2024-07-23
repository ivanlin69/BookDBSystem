TARGET_SERVER = bin/BookDBServer
TARGET_CLIENT = bin/BookDBClient
SRC_SERVER = $(wildcard src/server/*.c)
OBJ_SERVER = $(patsubst src/server/%.c, obj/server/%.o, $(SRC_SERVER))

SRC_CLIENT = $(wildcard src/client/*.c)
OBJ_CLIENT = $(patsubst src/client/%.c, obj/client/%.o, $(SRC_CLIENT))


run: clean default
	./$(TARGET_SERVER) -f ./test.db -p 8080 -n

default: $(TARGET_SERVER) $(TARGET_CLIENT)

clean:
	rm -f obj/server/*.o
	rm -f obj/client/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET_SERVER): $(OBJ_SERVER)
	gcc -o $@ $^

$(TARGET_CLIENT): $(OBJ_CLIENT)
	gcc -o $@ $^

obj/server/%.o : src/server/%.c
	gcc -c $< -o $@ -Iinclude

obj/client/%.o : src/client/%.c
	gcc -c $< -o $@ -Iinclude
