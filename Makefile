TARGET = bin/BookDBController
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

default: $(TARGET)

run: clean default
	./$(TARGET) -n -f test.db
	./$(TARGET) -a "The Fountainhead, Ayn Rand, 	Philosophical fiction,  978-0026009102, 1943" -f test.db

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $^

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude
