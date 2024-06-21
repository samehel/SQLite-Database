# SQLite Database Implementation

This project is an implementation of a SQLite-like database system with added features, developed primarily in C++ for educational purposes. It is inspired by concepts from the tutorial by [Connor Stack](https://cstack.github.io/db_tutorial/), incorporating multiple database options and test cases.

## Features

- **Memory-based Database**: Stores data in volatile memory, suitable for rapid access and testing.
- **Disk-based Database**: Persists data on disk, leveraging file I/O operations for durability.
- **B-Tree based Database**: Implements a B-Tree data structure to optimize indexing and retrieval operations.
- **C++ Implementation**: Utilizes modern C++ features for clarity and efficiency.

## Usage

### Prerequisites

- C++ compiler with C++11 support.
- Git for cloning the repository.

### Building the Project on Visual Studio

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/samehel/SQLite-Database.git
   cd SQLite-Database
   ```

2. **Open Visual Studio:**

   - Launch Visual Studio.
   - Open the solution file (`SQLite-Database.sln`) located in the cloned repository.

3. **Configure Project Settings:**

   - Ensure the project is configured to compile with C++11 or higher standards.

4. **Build the Solution:**

   - In Visual Studio, right-click on the solution in the Solution Explorer.
   - Select **Build Solution** to compile the entire project.

5. **Verify Build Success:**

   - Check the Output window for build messages.
   - Resolve any compilation errors or warnings as needed.

---

### Example Usage

- After selecting one of three main options you can either:

```plaintext
.exit: Terminates the database program and exits to the command prompt or closes the application.

.select: Displays all data currently stored in the database table.

.insert ID Name Email: Inserts a new record into the database table with the specified ID, Name, and Email values.
```

## Acknowledgements

- This project draws inspiration from [Connor Stack's tutorial](https://cstack.github.io/db_tutorial/) on building a simple database from scratch.
- Additional features and adaptations by Sameh.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
