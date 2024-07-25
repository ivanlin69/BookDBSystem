# Book Database System

## Description
This is a `C`-based program functions as a database system to manage book information. The system allows clients to view, add, remove, update, and list books remotely through network operations and can also work locally. It is implemented as a Unix command-line utility using getopt for command-line argument parsing.


## Features

**Network Capabilities**
- Multiplexing with poll: The server uses the poll system call to handle multiple client connections simultaneously.
- Finite State Machines (FSM): The server manages client connections and their states using finite state machines, ensuring robust and efficient state management.
- Custom Protocol: The communication between the client and server is managed using a custom protocol, ensuring structured and reliable data exchange.
 
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

### Local
  ```bash
  ./bin/BookDBServer -n -f <database file>
  ./bin/BookDBServer -f <database file> -a <new book info>
  ./bin/BookDBServer -f <database file> -l
  ./bin/BookDBServer -f <database file> -r <book title>
  ./bin/BookDBServer -f <database file> -u <book title, year>
   ```
- -n: Create new database file
- -f: Path to database file (required)
- -a: Information of the new book (title, author, genre, ISBN, year).
- -r: Title of the book to be removed.
- -u: Title of the book and the new publication year.
- -l: List all books in the database.


### Server
```bash
    ./bin/BookDBServer -f <database file> -p <port number>
   ```
-  -p: Port number to listen on (required)

### Client
```bash
  ./bin/BookDBClient -p 8080 -h 127.0.0.1 -a 'Title,Author,Genre,ISBN'
  ./bin/BookDBClient -p 8080 -h 127.0.0.1 -l
  ./bin/BookDBClient -p 8080 -h 127.0.0.1 -r 'Title'
  ./bin/BookDBClient -p 8080 -h 127.0.0.1 -u 'Title,Year'
   ```
- -p: Specify the port number to connect to (required)
- -h: Specify the host IP address to connect to (required)
- -a: Add a new book with the given info (title, author, genre, isbn)
- -l: List all books in the database
- -r: Remove the book with the given title
- -u: Update the published year of the book with the given title

  
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
   ./bin/BookDBServer -n -f test.db
   ```
   
## File Structure
- **Makefile**: Instructions to build and run the program.
- **main**: The main entry point of the program.
- **parse**: Contains operations for controlling the data (adding, removing, updating, listing books).
- **file**: Handles file operations (creating and opening database files).
- **server**: Manages server operations, handling multiple client connections and managing states.
- **client**: Manages client operations, sending requests to the server and processing responses.


## Contributing
Contributions to the system are welcome! Please fork the repository and submit a pull request with your enhancements.

## License
This project is licensed under the MIT License - see the [MIT License Documentation](https://opensource.org/licenses/MIT) for details.

## Contact
For support, please feel free to contact me.
