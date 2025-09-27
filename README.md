# Hill Climbing for Host-Guest Assignment

This project is a C++ implementation of a local search algorithm, specifically Hill Climbing, to solve a combinatorial optimization problem. The goal is to find an optimal assignment of "guests" to "hosts" over several time periods, minimizing a cost function based on a set of constraints.

This was developed as a university assignment for an Artificial Intelligence course.

## Problem Description

The program attempts to solve a scheduling problem where a number of guest boats must be assigned to host boats over a set of time periods (`T`). The assignments are subject to several constraints, such as:

*   **Capacity (CAPA)**: Each host has a limited capacity.
*   **Once per Period (ONCE)**: A guest cannot be assigned to the same host more than once in the same time period.
*   **Different Hosts (DIFF)**: A guest should be assigned to different hosts across different time periods.

The cost function (`Cost()`) calculates penalties for each violated constraint. The objective of the algorithm is to find an assignment that minimizes this total cost.

## How it Works

The program uses a combination of algorithms to find a near-optimal solution:

1.  **Greedy Algorithm**: An initial solution is constructed using a randomized greedy approach. This provides a starting point for the optimization.
2.  **Hill Climbing (First Improvement)**: The core of the project. It iteratively explores the neighborhood of the current solution by making small changes (mutations) and accepts any change that results in a lower cost. This process continues until no further improvements can be found in the local neighborhood.
3.  **Random Restart**: To avoid getting stuck in local optima, the algorithm can perform a "restart," which significantly perturbs the current best solution and then applies the Hill Climbing algorithm again from the new state.

## How to Compile and Run

The project includes a `Makefile` for easy compilation and execution.

1.  **Compile**: Open a terminal and run the `make` command. This will compile `main.cpp` and create an executable named `HC`.
    ```sh
    make
    ```
2.  **Run**: The `make` command will also automatically execute the program (`./HC`). The output of the run will be saved to a file corresponding to the configuration used (e.g., `config1.out`).

## Configuration

To change the problem instance, you need to modify the `main.cpp` file.

*   **Line 7**: Change the `config` macro to select the desired configuration file.
    ```cpp
    #define config "config1" // <-- Change the number to use config1.txt, config2.txt, etc.
    ```

The program reads input data from two files:
*   `PPP.txt`: Contains the main problem instance, including the number of boats, time periods, and boat characteristics.
*   `configX.txt`: Specifies which of the boats are hosts for a given scenario. The remaining boats are considered guests.