
# CMSC 341 - Spring 2024 - Project 4: Vaccination Database

## Overview
This project implements a vaccination database system (`VacDB`) using hash tables. It demonstrates efficient data storage, retrieval, collision handling, and rehashing. The database manages records of patients, supporting insertion, removal, searching, and updating operations. It also incorporates various collision handling mechanisms and testing functionalities.

## Features
### Vaccination Database (`VacDB`)
- **Hash Table Implementation**:
  - Dynamically resizes based on load factors and deletion ratios.
  - Supports multiple probing policies: Linear, Quadratic, and Double Hashing.

- **Operations**:
  - **Insert**: Adds a patient to the database, ensuring no duplicate records.
  - **Remove**: Marks patient records as deleted using a lazy deletion scheme.
  - **Search**: Retrieves patient records by name and serial number.
  - **Update**: Modifies serial numbers for existing patients.
  - **Rehashing**: Dynamically adjusts hash table size for optimal performance.

- **Collision Handling Policies**:
  - Linear Probing
  - Quadratic Probing
  - Double Hashing

### Patient Class
- Attributes:
  - `Name`: Unique identifier for each patient.
  - `Serial`: Unique serial number for each patient.
  - `Used`: Status indicator (active or deleted).

- Overloaded Operators:
  - Equality operator (`==`): Compares patients based on `name` and `serial`.
  - Insertion operator (`<<`): Outputs patient details.

### Random Data Generation
- Supports various data distributions for testing:
  - Uniform Integer
  - Uniform Real
  - Normal Distribution
  - Shuffled Data

### Comprehensive Testing Framework
- **Tester Class**: Validates the following:
  - Insertion, removal, and search operations.
  - Handling of colliding and non-colliding keys.
  - Correctness of rehashing and collision policy changes.

## Files
### Source Files
- **`vacdb.cpp` & `vacdb.h`**:
  - Implements the `VacDB` class and its operations.
  - Handles hash table initialization, collision resolution, rehashing, and dynamic resizing.
- **`mytest.cpp`**:
  - Contains the `Tester` class for evaluating the functionality of the `VacDB` class.
  - Includes randomized test cases and performance checks.

### Header Files
- **`vacdb.h`**:
  - Defines the `VacDB` and `Patient` classes.
  - Specifies constants such as `MINID`, `MAXID`, `MINPRIME`, and `MAXPRIME`.
  - Declares the hash function and collision policies.

## Installation and Execution
1. Compile the program:
   ```bash
   g++ -o vacdb mytest.cpp vacdb.cpp
   ```
2. Run the program:
   ```bash
   ./vacdb
   ```

## Usage Examples
### Inserting a Patient
```cpp
VacDB vacdb(101, hashCode, DOUBLEHASH);
Patient newPatient("John", 1234, true);
vacdb.insert(newPatient);
```

### Searching for a Patient
```cpp
Patient retrieved = vacdb.getPatient("John", 1234);
if (!retrieved.getKey().empty()) {
    cout << "Found: " << retrieved << endl;
}
```

### Changing Collision Policy
```cpp
vacdb.changeProbPolicy(LINEAR);
```

### Rehashing
Rehashing is triggered automatically when:
- Load factor exceeds 0.5.
- Deleted ratio exceeds 0.8.

## Testing Framework
The `Tester` class includes the following tests:
- **Insertion**:
  - Verifies correct placement and handling of collisions.
- **Search**:
  - Ensures correct retrieval of both colliding and non-colliding keys.
- **Removal**:
  - Confirms proper marking of records as deleted and their absence from active lookups.
- **Rehashing**:
  - Tests rehashing under different conditions, such as load factor and deletion ratio.
- **Policy Changes**:
  - Validates behavior when switching between collision handling policies.

## Dependencies
- **Standard Libraries**:
  - `<iostream>`
  - `<string>`
  - `<vector>`
  - `<cmath>`
  - `<algorithm>`
  - `<random>`

## License
This project is for educational purposes and does not include warranties or guarantees.
