# nstd (Non-Standard Library)

**nstd** is a personal learning project dedicated to implementing core components of the C++ Standard Library (STL) and other system-level utilities. 

As the name implies, this is **N**on-**ST**andar**D**—a playground where learning, experimentation, and understanding the "how" take precedence over strict standard compliance.

## 🎯 Project Goals

The primary goal is not to replace the STL, but to demystify it. By rebuilding these fundamental classes, I aim to cover a wide range of advanced C++ language features, including memory management, concurrency, and template metaprogramming.

### Design Philosophy
* **Educational Depth:** Classes are chosen specifically to challenge my understanding of C++ internals (e.g., `function` for type erasure, `shared_ptr` for atomics).
* **Standard-Adjacent:** I aim to mirror the standard STL interfaces and behaviors where possible to maintain familiarity. However, strict adherence is not enforced; the code evolves and improves iteratively as I learn new techniques and best practices.
* **Modern C++:** utilizing C++20/23 features where applicable (Concepts, Constraints, Implicit Lifetime types).

## 🧩 Implemented Classes

### 🧠 Smart Pointers & Memory Management
* **`nstd::memory_pool`**: Fixed-size block allocator using embedded free-lists and $O(1)$ expansion.
* **`nstd::unique_ptr`**: RAII ownership wrapper focusing on move semantics and custom deleters.
* **`nstd::shared_ptr`**: Reference-counted ownership using and control block management.

### 📦 Containers
* **`nstd::vector`**: Dynamic array focusing on raw buffer management and exception safety.
* **`nstd::string`**: String implementation with generic CharT tamplate type.
* **`nstd::list`**: Doubly linked list implementation.
* **`nstd::array`**: Zero-overhead compile-time array wrapper.
* **`nstd::stack`**: Container adapter over the nstd::vector.

### ⚙️ Utilities & Metaprogramming
* **`nstd::function`**: General-purpose polymorphic function wrapper.
    * *Key Concept:* **Type Erasure** (hiding concrete types like lambdas/functors behind a uniform interface).
* **`nstd::expected`** (C++23): Error handling wrapper that holds either a value or an error.
    * *Key Concept:* **Tagged Unions**.

### ⚡ Concurrency
* **`nstd::thread_pool`**: Asynchronous task scheduler using `std::mutex`, `std::condition_variable`, and generic task queue.

## 🧪 Testing

* **Current Status:** Tests are currently generated via AI assistants to verify core logic, edge cases, and memory safety (e.g., expansion limits, resource leaks).
* **Future Roadmap:** I plan to integrate a formal C++ testing framework (like Google Test or Catch2) to gain hands-on experience with automated testing pipelines and unit test authoring.

---

*Note: This library is for educational purposes and is under active development.*
