TARGET = bin/BookDBController
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

default: $(TARGET)

run: clean default
	./$(TARGET) -n -f test.db
	./$(TARGET) -a "The Fountainhead,Ayn Rand,Philosophical fiction,978-0026009102,1943" -f test.db
	./$(TARGET) -a "Meditations,Marcus Aurelius,Philosophy,978-1503280465,2018" -f test.db
	./$(TARGET) -a "When Nietzsche Wept,Irvin D. Yalom,Philosophical fiction,978-0062009302,1992" -f test.db
	./$(TARGET) -f test.db -l
	./$(TARGET) -f test.db -r "dflakjs"
	./$(TARGET) -f test.db -l
	./$(TARGET) -f test.db -r "The Fountainhead"
	./$(TARGET) -f test.db -l
	./$(TARGET) -a "The Fountainhead,Ayn Rand,Philosophical fiction,978-0026009102,1943" -f test.db
	./$(TARGET) -f test.db -l
	./$(TARGET) -f test.db -u "The Fountainhead,2222"
	./$(TARGET) -f test.db -l

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $^

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude
