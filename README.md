# Grimoire-DB

A lightweight, high-performance, in-memory Key-Value database written in bare-metal C. Designed with a custom REPL shell and POSIX-compliant binary serialization, this engine is optimized for managing 1-to-N relational data models.

## 🧠 Architecture
Grimoire-DB is built on a custom Hash Table implementation. It utilizes linked-list collision resolution and O(1) head-insertion mechanics to ensure maximum speed when writing data. 

Memory management is strictly controlled, utilizing an "inside-out" cascading destruction protocol to ensure zero memory leaks during prolonged operation.

## ⚡ Current Features
* **In-Memory Engine:** Blazing fast CRUD operations using direct pointer manipulation.
* **1-to-N Relationships:** Natively supports linking multiple child nodes (Cards) to a single parent node (Deck).
* **Decoupled Presentation:** Utilizes callback function pointers to separate search logic from CLI presentation.

## 🛠️ Compilation
Ensure you are using a standard C compiler (like GCC) and compile with POSIX 2008 standards for `strdup` support:
```bash
gcc -std=c99 -D_POSIX_C_SOURCE=200809L main.c engine.c -o grimoire
./grimoire