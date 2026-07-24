# Multi-Threaded Banking & Fraud Detection Engine (C++)

A robust C++ console application simulating core banking operations, multi-currency transactions, and automated security auditing. Built with Object-Oriented Programming (OOP) principles, thread safety, and custom graph-based audit algorithms to prevent financial fraud.

---

## Key Features

* **Advanced OOP Architecture:** Implements polymorphism, abstraction, and inheritance for account types (`CurrentAccount` with overdraft protection, `SavingsAccount` with compounding interest).
* **Concurrent Forex Engine:** Background threads continuously fetch and update dynamic foreign exchange rates, using mutex locks to ensure thread safety across concurrent transactions.
* **Multi-Currency Transactions:** Supports seamless deposits, withdrawals, and cross-currency transfers with real-time rate conversion.
* **Cycle-Based Fraud Audit Module:** An automated security system that continuously inspects the transaction graph to detect and interrupt circular transfer attempts (e.g., $A \rightarrow B \rightarrow C \rightarrow A$).
* **Custom Exception Handling:** Robust error management using custom C++ exception hierarchies for insufficient funds, unauthorized overdrafts, and flagged fraudulent transfers.

---

## Tech Stack & Concepts

* **Language:** C++
* **Concurrency:** `std::thread`, `std::mutex`, `std::lock_guard`
* **Concepts:** Object-Oriented Programming, Graph Processing (Cycle Detection), Custom Exceptions, Memory Management

---

## Architectural Highlights & Design Patterns

1. **Thread Safety:** Financial ledger state and forex rates are guarded by synchronization primitives to prevent race conditions during parallel account operations.
2. **Fraud Detection Algorithm:** Models user transfers as a Directed Graph, leveraging depth-first search / cycle detection to flag suspicious circular transactions in real time.
