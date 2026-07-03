# Network Routing Simulator

A graph-based network routing simulator built in C++. Models a network of routers as nodes and connections as weighted edges, then computes the most efficient path between any two points using shortest-path and graph traversal algorithms.

The project reads a real network topology from a CSV file, builds the graph in memory, and outputs the optimal route — simulating how actual routing protocols find paths through a network.

---

## What It Does

- Loads network topology from a CSV file (nodes and connections)
- Builds a graph using adjacency lists
- Computes shortest paths between routers using Dijkstra's Algorithm
- Supports graph traversal with BFS and DFS
- Outputs the optimal route and path cost to the console

---

## Algorithms & Concepts

`Dijkstra's Algorithm` `BFS` `DFS` `Adjacency Lists` `Priority Queues` `Graph Theory` `OOP`

---

## Tech Stack

`C++` `STL` `File Handling` `GCC`

---

## Project Structure

```
project/
├── Graph.h            → graph class definition
├── Graph.cpp          → graph implementation
├── main_router.cpp    → main routing logic and entry point
└── network.csv        → network topology input data
```

---

## How to Build

```bash
g++ -std=c++11 Graph.cpp main_router.cpp -o router
```

## How to Run

```bash
./router network.csv
```

> If the program expects a different input format, check `main_router.cpp` for the expected command-line parameters.

---

*Built as part of Data Structures & Algorithms coursework at FAST-NUCES Islamabad.*
