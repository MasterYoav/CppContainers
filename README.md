# CppContainers — Generic Container & Custom Iterators

A header-only Container & Iterators library that provides:

* A **dynamic container** (`MyContainer`) for any comparable type
* **Six custom iterators** for alternative traversal orders
* **Operator <<** for easy printing
* **Unit tests** with [doctest](https://github.com/doctest/doctest)
* **Memory-leak checks** via Valgrind
* **Demo program** showing every iterator in action

---

## Table of Contents

1. [Container Features](#container-features)
2. [Project Structure](#project-structure)
3. [Building](#building)
4. [Running Unit Tests](#running-unit-tests)
5. [Memory Leak Checking](#memory-leak-checking)
6. [Example Usage](#example-usage)
7. [Cleaning Up](#cleaning-up)

---

## Container Features

`MyContainer` stores any type **T** that implements both `operator<` and `operator==`.
Besides normal insertion order it supports six traversal modes:

| Iterator pair                                                        | Order produced for `[7, 15, 6, 1, 2]` |
| -------------------------------------------------------------------- | ------------------------------------- |
| **Order** `begin_order()` / `end_order()`                            | `7 15 6 1 2`                          |
| **Reverse** `begin_reverse_order()` / `end_reverse_order()`          | `2 1 6 15 7`                          |
| **Ascending** `begin_ascending_order()` / `end_ascending_order()`    | `1 2 6 7 15`                          |
| **Descending** `begin_descending_order()` / `end_descending_order()` | `15 7 6 2 1`                          |
| **Side-Cross** `begin_side_cross_order()` / `end_side_cross_order()` | `1 15 2 7 6`                          |
| **Middle-Out** `begin_middle_out_order()` / `end_middle_out_order()` | `6 15 1 7 2`                          |


## Project Structure

```
CppContainers/
│
├── include/
│   ├── MyContainer.hpp     # Container & iterators implementation
│   └── doctest.h           # Test framework header
│
├── Demo.cpp                # Demonstration file
├── tests.cpp               # doctest unit tests (~30 assertions)
├── Makefile                # Main / test / valgrind / clean
└── README.md               # This file
```

---

## Building

Ensure a C++17 compiler is available.

```bash

make Main
```

The demo prints the size of the container followed by the six iterator orders.

---

## Running Unit Tests

```bash
make test
```

* Compiles `tests.cpp` with `include/MyContainer.hpp`

---

## Memory Leak Checking

```bash
make valgrind
```

---

## Cleaning Up

```bash
make clean
```

Removes generated binaries (`Main`, `tests`).

---
