# Book Database System

## Description
This is a `C`-based program that functions as a database system to manage book information. The system allows users to view, add, remove, update, and list books. It is implemented as a Unix command line utility using getopt for command-line argument parsing.


## Features
**Unix Command Line Utility with Getopt**
- The program utilizes the getopt library to parse command-line arguments.
- Users can specify various options to perform operations on the database.

**Database Header Management**
- The program creates and validates a header for the database file.
- The header contains metadata such as magic number, version, book count, and file size.

**Book Operations**
- Add Book: Add a new book to the database.
- Remove Book: Remove an existing book from the database.
- Update Book: Update the publication year of a book.
- List Books: List all books in the database.


## Usage

**Create a New Database File**
```bash
./BookDBController -n -f <database file>
```
- -n: Create a new database file.
- -f <database file>: Path to the database file.

**Add a New Book**
  ```bash
   ./BookDBController -f <database file> -a <new book info>
   ```
- -a <new book info>: Information of the new book (title, author, genre, ISBN, year).

**List All Books**
  ```bash
   ./BookDBController -f <database file> -l
   ```
- -l: List all books in the database.

**Remove a Book:**
  ```bash
   ./BookDBController -f <database file> -r <book title>
   ```
- -r <book title>: Title of the book to be removed.

**Update Book’s Published Year**:
  ```bash
   ./BookDBController -f <database file> -u <book title, year>
   ```
- -u <book title, year>: Title of the book and the new publication year.



## Prerequisites

- A modern C compiler (e.g., GCC, Clang)
- CMake for building the project (optional)


### Installation

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yourusername/BookDBSystem.git
   cd BookDBSystem
   ```
2. **Compile the Program**
   ```bash
   make
   ```
3. **Run the Program(Example)**
   ```bash
   ./bin/BookDBController -n -f test.db
   ```
   
## File Structure
- **Makefile**: Instructions to build and run the program.
- **main**: The main entry point of the program.
- **parse**: Contains operations for controlling the data (adding, removing, updating, listing books).
- **file**: Handles file operations (creating and opening database files).


## Contributing
Contributions to the system are welcome! Please fork the repository and submit a pull request with your enhancements.

## License
This project is licensed under the MIT License - see the [MIT License Documentation](https://opensource.org/licenses/MIT) for details.

## Contact
For support, please feel free to contact me.
