#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
using namespace std;

// Maximum number of routers in the network
const int MAX_ROUTERS = 100;
// Maximum number of messages in a queue
const int MAX_MESSAGES = 100;
// Maximum length for strings
const int MAX_STRING_LENGTH = 256;
// Maximum value for cost/weight
const int INFINITY_COST = 99999;

// Utility class for string operations
class String {
private:
    char data[MAX_STRING_LENGTH];
    
public:
    String();
    String(const char* str);
    
    // Copy constructor and assignment operator
    String(const String& other);
    String& operator=(const String& other);
    
    // Comparison operators
    bool operator==(const String& other) const;
    bool operator!=(const String& other) const;
    
    // Conversion to C-style string
    const char* getString() const;
    
    // String operations
    int length() const;
    void copy(char* dest, int size) const;
    bool empty() const;
};

// Structure to represent a routing table entry
struct RoutingTableEntry {
    String destination;
    String nextHop;
    int cost;
    
    // Constructor for convenient creation
    RoutingTableEntry();
    RoutingTableEntry(const String& dest, const String& next, int c);
};

// Structure to represent a message
struct Message {
    static int nextId;  // Static counter for message IDs
    int id;             // Unique message ID
    int priority;       // Message priority (higher values = higher priority)
    String source;      // Source router ID
    String destination; // Destination router ID
    String payload;     // Message content
    String path[MAX_ROUTERS]; // Path taken by the message
    int pathLength;     // Current length of the path
    
    // Constructor
    Message();
    Message(int prio, const String& src, const String& dst, const String& payload);
    
    // Copy constructor and assignment operator
    Message(const Message& other);
    Message& operator=(const Message& other);
    
    // Add a router to the path
    void addToPath(const String& routerId);
};

// Forward declaration of Router class for Graph to reference
class Router;

// Priority Queue for messages (max heap)
class PriorityQueue {
private:
    Message messages[MAX_MESSAGES];
    int size;
    
    // Helper functions for heap operations
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }
    void heapify(int i);
    void swap(int i, int j);
    
public:
    PriorityQueue();
    
    void push(const Message& message);
    Message pop();
    bool empty() const;
    int getSize() const;
    Message peek() const;
};

// FIFO Queue for messages
class Queue {
private:
    Message messages[MAX_MESSAGES];
    int front;
    int rear;
    int size;
    
public:
    Queue();
    
    void push(const Message& message);
    Message pop();
    bool empty() const;
    int getSize() const;
    Message peek() const;
};

// Routing Table implemented as a linear list
class RoutingTable {
private:
    RoutingTableEntry entries[MAX_ROUTERS];
    int size;
    
public:
    RoutingTable();
    
    bool add(const String& destination, const String& nextHop, int cost);
    bool remove(const String& destination);
    bool update(const String& destination, const String& nextHop, int cost);
    RoutingTableEntry* find(const String& destination);
    void display() const;
    int getSize() const;
    RoutingTableEntry getEntry(int index) const;
    void clear();
};

// Router Map to store routers by ID
class RouterMap {
private:
    struct Entry {
        String key;
        Router* value;
        bool used;
    };
    
    Entry entries[MAX_ROUTERS];
    int size;
    
public:
    RouterMap();
    ~RouterMap();
    
    bool put(const String& key, Router* value);
    Router* get(const String& key) const;
    bool contains(const String& key) const;
    bool remove(const String& key);
    int getSize() const;
    String getKey(int index) const;
};

// Queue Map to store outgoing queues by destination
class QueueMap {
private:
    struct Entry {
        String key;
        Queue value;
        bool used;
    };
    
    Entry entries[MAX_ROUTERS];
    int size;
    
public:
    QueueMap();
    
    Queue* get(const String& key);
    bool contains(const String& key) const;
    void addEntry(const String& key);
    int getSize() const;
    String getKey(int index) const;
};

// Graph class to manage the network of routers
class Graph {
private:
    RouterMap routers;  // Map of router ID to Router object
    int** adjacencyMatrix; // Adjacency matrix for edge weights
    int numRouters;     // Number of routers in the network
    String* routerIds;  // Array of router IDs
    
    // Helper method to get index of a router by ID
    int getRouterIndex(const String& routerId) const;
    
    // Internal methods for Dijkstra algorithm
    void runDijkstra(const String& sourceId);
    void updateRoutingTables();
    
public:
    Graph();
    ~Graph();
    
    // Router management
    bool addRouter(const String& routerId);
    bool removeRouter(const String& routerId);
    Router* getRouter(const String& routerId) const;
    bool routerExists(const String& routerId) const;
    
    // Edge/connection management
    bool addEdge(const String& fromRouter, const String& toRouter, int weight);
    bool removeEdge(const String& fromRouter, const String& toRouter);
    bool updateEdgeWeight(const String& fromRouter, const String& toRouter, int newWeight);
    int getEdgeWeight(const String& fromRouter, const String& toRouter) const;
    
    // Network-wide operations
    void deliverMessage(const Message& message, const String& fromRouter);
    void displayAllRouters() const;
    void sendAll();
    
    // Routing algorithms
    void buildRoutingTables();
    void displayAdjacencyMatrix() const;
    
    // Path operations
    int findShortestPath(const String& source, const String& destination, String path[], int& pathLength);
};

// Router class to represent a network router
class Router {
private:
    String routerId;                   // Router identifier
    RoutingTable routingTable;         // Routing table as a linear list
    PriorityQueue incomingQueue;       // Priority queue for incoming messages
    QueueMap outgoingQueues;           // Outgoing queues per destination
    Graph* network;                    // Reference to the network graph
    
public:
    Router(const String& id, Graph* graph);
    ~Router();
    
    // Routing table operations
    bool addRoute(const String& destination, const String& nextHop, int cost);
    bool removeRoute(const String& destination);
    bool updateRoute(const String& destination, const String& nextHop, int cost);
    RoutingTableEntry* findRoute(const String& destination);
    void displayRoutingTable() const;
    bool loadRoutingTableFromFile(const char* filename);
    bool saveRoutingTableToFile(const char* filename);
    void clearRoutingTable();
    
    // Message handling
    void sendMessage(int priority, const String& source, const String& destination, const String& payload);
    void receiveMessage(const Message& message);
    bool processMessage(); // Process one message from the incoming queue
    void sendAll(); // Send all messages from outgoing queues
    
    // Path finding
    void pathTo(const String& destination, String path[], int& pathLength);
    void displayPathTo(const String& destination);
    
    // Getters
    const String& getId() const;
    int getIncomingQueueSize() const;
    int getOutgoingQueueSize(const String& destination) const;
    int getTotalOutgoingQueueSize() const;
    RoutingTable* getRoutingTable() { return &routingTable; }
};

#endif // GRAPH_H 