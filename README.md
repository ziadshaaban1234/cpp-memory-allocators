# Custom Memory Allocators and Performance Benchmarking in C++

## Overview
This project implements and evaluates various custom memory allocation strategies in C++. Modern high-performance applications, such as game engines and real-time systems, often bypass the standard operating system allocators to avoid fragmentation and reduce overhead. This repository contains functional implementations of several custom allocator designs and a rigorous benchmarking framework that compares their efficiency against standard dynamic memory allocation (malloc/free).

## Core Allocator Designs

### 1. Linear Allocator
Pre-allocates a continuous block of memory and satisfies allocation requests by sequentially moving an internal offset pointer. This approach delivers strictly O(1) allocation time by reducing the operation to simple pointer arithmetic. However, it does not support individual deallocations; the entire block must be cleared at once.

### 2. Stack Allocator
Operates similarly to the Linear Allocator but introduces a mechanism to free memory blocks in a strict Last-In, First-Out (LIFO) sequence. This design provides highly efficient memory reuse for scoped allocations while maintaining zero fragmentation during its lifecycle.

### 3. Pool Allocator
Divides a single large block of pre-allocated memory into an array of fixed-size chunks. It tracks available slots using a free list structure. This approach achieves deterministic O(1) time for both allocation and deallocation, making it exceptionally well-suited for systems handling large quantities of uniform objects.

### 4. Free List Allocator
A dynamic, variable-sized memory manager designed to allocate arbitrary memory sizes. It tracks free fragments across the memory block and implements structural policies to locate suitable blocks for allocation and safely merge adjacent free fragments during deallocation.

## Benchmarking Methodology
The embedded benchmark class evaluates the custom allocators under high stress by executing thousands of contiguous operations. The primary evaluation criteria include:
* **Contiguous Multi-Allocation:** Allocating consecutive blocks of varying sizes to measure peak sequential throughput.
* **Sequential Release:** Deallocating arrays of pointers to assess structural deallocation performance.
* **Random Pattern Simulation:** Shuffling pointer sequences and executing unpredictable allocation and deallocation steps to mimic real-world runtime memory behavior.
* **Microsecond-Latency Evaluation:** Calculating absolute time intervals for individual, high-priority allocation requests.

## Performance Analysis under Compilation Optimizations
When compiled with full optimization flags (Release Mode, x64), the benchmark metrics demonstrate significant performance advantages for custom memory management:
* **Pointer Arithmetic Efficiency:** Both Linear and Stack allocators routinely outperform the standard C allocator by several orders of magnitude, as they eliminate kernel-level searches and synchronization locks.
* **Deterministic Behavior:** The Pool allocator achieves optimal performance for fixed-size blocks, ensuring predictable execution speeds crucial for real-time processing constraints.
* **Standard Allocator Overhead:** The standard implementation introduces measurable latency due to general-purpose searching algorithms, thread safety validations, and continuous interaction with the operating system kernel.

## Build and Execution Instructions
1. Open the source file `allocator_benchmark.cpp` in Visual Studio or any compatible C++ compiler environment.
2. Ensure the build configuration is explicitly set to Release mode instead of Debug mode to enable compiler optimizations.
3. Execute the binary directly or use the standard shortcut (Ctrl + F5) to run the performance testing suite and inspect the terminal output.
