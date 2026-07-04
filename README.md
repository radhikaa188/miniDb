# MiniDB 🚀

### A Relational Database Engine Built From Scratch in Modern C++

MiniDB is a lightweight relational database engine implemented entirely from scratch in C++.

Unlike CRUD applications that rely on MySQL, PostgreSQL, or MongoDB, MiniDB implements core database internals itself:

* Query parsing
* Binary storage
* Table management
* Transactions
* B-Tree indexing
* Buffer management
* Query planning
* Concurrency control

The goal is not to compete with production databases, but to understand how database systems work internally by building the major components from the ground up.

---

## ✨ Highlights

* Persistent storage using custom binary files
* SQL-inspired command-line interface
* Shadow-copy transaction support
* Self-implemented B-Tree index
* LRU page cache / buffer manager
* Rule-based query planning
* Thread-safe write operations
* Modern C++ memory management using RAII and smart pointers
* Modular architecture with separate compilation units

---

## 🛠 Tech Stack

| Component         | Technology          |
| ----------------- | ------------------- |
| Language          | C++17               |
| Storage           | Custom Binary Files |
| Indexing          | Self-Built B-Tree   |
| Buffer Pool       | LRU Cache           |
| Concurrency       | std::mutex          |
| Memory Management | unique_ptr, RAII    |
| Build System      | GCC / G++           |

No external libraries, frameworks, ORMs, or database engines are used.

---

## 🏗 Architecture

```text
User Command

      │

      ▼

Query Parser / Tokenizer

      │

      ▼

TableManager

      │

 ┌────┴────┐

 ▼         ▼

Table   Transaction Manager

 │

 ├──────────────┐
 │              │
 ▼              ▼

B-Tree      Buffer Manager

(Index)      (LRU Cache)

 │              │

 └──────┬───────┘
        ▼

 Storage Layer

(.db / .idx / .schema)
```

---

## 📦 Core Features

### 1. Table Management

```sql
CREATE TABLE students (
    id INT,
    name TEXT,
    gpa DOUBLE
);
```

Supports:

* CREATE TABLE
* INSERT
* SELECT
* UPDATE
* DELETE
* ORDER BY
* WHERE filtering

---

### 2. Persistent Storage

Each table is stored using custom binary serialization.

Files generated:

```text
students.db
students.idx
students.schema
```

Features:

* Variable-length string encoding
* Fast binary read/write
* Automatic schema restoration
* Data survives program restarts

---

### 3. B-Tree Indexing

Every table maintains an index on its primary (first) column.

Operations:

| Operation       | Complexity |
| --------------- | ---------- |
| Insert          | O(log n)   |
| Search          | O(log n)   |
| Equality Lookup | O(log n)   |

Example:

```sql
SELECT * FROM students
WHERE id = 5;
```

Execution:

```text
B-Tree Search
      ↓
Direct Row Lookup
      ↓
Result
```

Current limitation:

* Range queries still fall back to linear scans.

---

### 4. Transactions

Supported commands:

```sql
BEGIN;

INSERT INTO students VALUES (5, Priya, 9.5);

COMMIT;
```

or

```sql
BEGIN;

INSERT INTO students VALUES (5, Priya, 9.5);

ROLLBACK;
```

Implementation:

* Shadow-copy transaction model
* Snapshots captured on first write
* Writes remain in memory during transaction
* Rollback restores original state
* Commit flushes modified tables to disk

---

### 5. Buffer Manager (LRU Cache)

MiniDB implements a simplified database buffer pool.

Configuration:

```cpp
PAGE_SIZE = 5
MAX_CACHE_SIZE = 3
```

Mechanism:

1. Rows are grouped into pages.
2. Page offsets are stored in `.idx`.
3. Frequently accessed pages stay in memory.
4. Least Recently Used pages are evicted when cache is full.

Complexities:

| Operation    | Complexity |
| ------------ | ---------- |
| Cache Hit    | O(1)       |
| Cache Update | O(1)       |
| Eviction     | O(1)       |

---

### 6. Query Planning

MiniDB automatically selects execution paths.

Example:

```sql
WHERE id = 5
```

Uses:

```text
B-Tree Index
```

Example:

```sql
WHERE name = 'Riya'
```

Uses:

```text
Linear Scan
```

This acts as a simple rule-based query planner.

---

### 7. Concurrency Control

Write operations are protected using table-level mutexes.

```cpp
lock_guard<mutex>
```

Protected operations:

* INSERT
* UPDATE
* DELETE

Benefits:

* Prevents race conditions
* Automatic unlock via RAII
* Exception-safe locking

Verified using multi-threaded insert testing.

---

## 💻 Example Session

```sql
CREATE TABLE students
(id INT, name TEXT, gpa DOUBLE)

INSERT INTO students
VALUES (1, Riya, 9.1)

INSERT INTO students
VALUES (2, Aman, 8.4)

SELECT * FROM students

SELECT * FROM students
WHERE id = 2

UPDATE students
SET name = Priya
WHERE id = 2

DELETE FROM students
WHERE id = 1
```

---

## 📂 Project Structure

```text
miniDb/

├── include/
│   ├── Column.h
│   ├── Row.h
│   ├── Table.h
│   ├── TableManager.h
│   ├── Query.h
│   ├── BufferManager.h
│   ├── BTree.h
│   ├── BTreeNode.h
│   ├── Config.h
│   └── Exceptions.h
│
├── src/
│   ├── Column.cpp
│   ├── Row.cpp
│   ├── Table.cpp
│   ├── TableManager.cpp
│   ├── Query.cpp
│   ├── BufferManager.cpp
│   ├── BTree.cpp
│   ├── BTreeNode.cpp
│   ├── Utils.cpp
│   └── main.cpp
│
└── README.md
```

---

## 🎓 Concepts Demonstrated

### C++

* OOP Design
* Inheritance
* Polymorphism
* Virtual Functions
* Templates
* Smart Pointers
* Move Semantics
* RAII
* Exception Handling
* STL Containers

### Database Systems

* Binary Storage
* Serialization
* Transactions
* Buffer Pools
* Page Management
* B-Tree Indexing
* Query Parsing
* Query Planning
* Concurrency Control

---

## 🚧 Current Limitations

MiniDB intentionally focuses on database internals and does not currently implement:

* SQL JOINs
* Secondary indexes
* Networking
* Client-server architecture
* Authentication
* Query cost estimation
* WAL (Write-Ahead Logging)
* Crash recovery
* Replication
* Distributed storage

---

## 🎯 Why Build This?

Most developers learn databases from the outside by writing SQL queries.

MiniDB was built to understand what happens underneath:

* How rows become bytes on disk
* How indexes accelerate lookups
* How transactions isolate changes
* How buffer pools reduce I/O
* How query execution is planned
* How concurrency is controlled safely

The project serves as both a learning exercise and a systems-programming showcase demonstrating core database-engine concepts implemented directly in C++.

---

## 🔨 Build & Run

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o minidb -lpthread

./minidb
```

Requirements:

* GCC/G++ with C++17 support
* pthread
* Linux, WSL, or MSYS2 MinGW64
