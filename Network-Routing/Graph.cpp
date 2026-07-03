#include "Graph.h"
#include <cstring>
#include <cstdlib>
#include <ctime>

// Initialize static message ID counter
int Message::nextId = 1;

// String class implementation
String::String() {
    data[0] = '\0';
}

String::String(const char* str) {
    strncpy(data, str, MAX_STRING_LENGTH - 1);
    data[MAX_STRING_LENGTH - 1] = '\0';
}

String::String(const String& other) {
    strncpy(data, other.data, MAX_STRING_LENGTH - 1);
    data[MAX_STRING_LENGTH - 1] = '\0';
}

String& String::operator=(const String& other) {
    if (this != &other) {
        strncpy(data, other.data, MAX_STRING_LENGTH - 1);
        data[MAX_STRING_LENGTH - 1] = '\0';
    }
    return *this;
}

bool String::operator==(const String& other) const {
    return strcmp(data, other.data) == 0;
}

bool String::operator!=(const String& other) const {
    return strcmp(data, other.data) != 0;
}

const char* String::getString() const {
    return data;
}

int String::length() const {
    return strlen(data);
}

void String::copy(char* dest, int size) const {
    strncpy(dest, data, size - 1);
    dest[size - 1] = '\0';
}

bool String::empty() const {
    return data[0] == '\0';
}

// RoutingTableEntry implementation
RoutingTableEntry::RoutingTableEntry() : cost(0) {}

RoutingTableEntry::RoutingTableEntry(const String& dest, const String& next, int c) 
    : destination(dest), nextHop(next), cost(c) {}

// Message implementation
Message::Message() : id(0), priority(0), pathLength(0) {}

Message::Message(int prio, const String& src, const String& dst, const String& pl) 
    : id(nextId++), priority(prio), source(src), destination(dst), payload(pl), pathLength(0) {}

Message::Message(const Message& other) 
    : id(other.id), priority(other.priority), source(other.source), 
      destination(other.destination), payload(other.payload), pathLength(other.pathLength) {
    for (int i = 0; i < pathLength; i++) {
        path[i] = other.path[i];
    }
}

Message& Message::operator=(const Message& other) {
    if (this != &other) {
        id = other.id;
        priority = other.priority;
        source = other.source;
        destination = other.destination;
        payload = other.payload;
        pathLength = other.pathLength;
        for (int i = 0; i < pathLength; i++) {
            path[i] = other.path[i];
        }
    }
    return *this;
}

void Message::addToPath(const String& routerId) {
    if (pathLength < MAX_ROUTERS) {
        path[pathLength++] = routerId;
    }
}

// PriorityQueue implementation
PriorityQueue::PriorityQueue() : size(0) {}

void PriorityQueue::swap(int i, int j) {
    Message temp = messages[i];
    messages[i] = messages[j];
    messages[j] = temp;
}

void PriorityQueue::heapify(int i) {
    int largest = i;
    int left = leftChild(i);
    int right = rightChild(i);

    if (left < size && messages[left].priority > messages[largest].priority)
        largest = left;

    if (right < size && messages[right].priority > messages[largest].priority)
        largest = right;

    if (largest != i) {
        swap(i, largest);
        heapify(largest);
    }
}

void PriorityQueue::push(const Message& message) {
    if (size >= MAX_MESSAGES)
        return;

    int i = size;
    messages[i] = message;
    size++;

    while (i != 0 && messages[parent(i)].priority < messages[i].priority) {
        swap(i, parent(i));
        i = parent(i);
    }
}

Message PriorityQueue::pop() {
    if (size <= 0)
        return Message();

    if (size == 1) {
        size--;
        return messages[0];
    }

    Message root = messages[0];
    messages[0] = messages[size - 1];
    size--;
    heapify(0);

    return root;
}

bool PriorityQueue::empty() const {
    return size == 0;
}

int PriorityQueue::getSize() const {
    return size;
}

Message PriorityQueue::peek() const {
    if (size <= 0)
        return Message();
    return messages[0];
}

// Queue implementation
Queue::Queue() : front(0), rear(0), size(0) {}

void Queue::push(const Message& message) {
    if (size >= MAX_MESSAGES)
        return;

    messages[rear] = message;
    rear = (rear + 1) % MAX_MESSAGES;
    size++;
}

Message Queue::pop() {
    if (empty())
        return Message();

    Message message = messages[front];
    front = (front + 1) % MAX_MESSAGES;
    size--;
    return message;
}

bool Queue::empty() const {
    return size == 0;
}

int Queue::getSize() const {
    return size;
}

Message Queue::peek() const {
    if (empty())
        return Message();
    return messages[front];
}

// RoutingTable implementation
RoutingTable::RoutingTable() : size(0) {}

bool RoutingTable::add(const String& destination, const String& nextHop, int cost) {
    if (size >= MAX_ROUTERS)
        return false;

    // Check if entry already exists
    for (int i = 0; i < size; i++) {
        if (entries[i].destination == destination) {
            entries[i].nextHop = nextHop;
            entries[i].cost = cost;
            return true;
        }
    }

    // Add new entry
    entries[size++] = RoutingTableEntry(destination, nextHop, cost);
    return true;
}

bool RoutingTable::remove(const String& destination) {
    for (int i = 0; i < size; i++) {
        if (entries[i].destination == destination) {
            // Shift elements to remove the entry
            for (int j = i; j < size - 1; j++) {
                entries[j] = entries[j + 1];
            }
            size--;
            return true;
        }
    }
    return false;
}

bool RoutingTable::update(const String& destination, const String& nextHop, int cost) {
    for (int i = 0; i < size; i++) {
        if (entries[i].destination == destination) {
            entries[i].nextHop = nextHop;
            entries[i].cost = cost;
            return true;
        }
    }
    return false;
}

RoutingTableEntry* RoutingTable::find(const String& destination) {
    for (int i = 0; i < size; i++) {
        if (entries[i].destination == destination) {
            return &entries[i];
        }
    }
    return nullptr;
}

void RoutingTable::display() const {
    cout << "+------------------+------------------+------+" << endl;
    cout << "| Destination      | Next Hop         | Cost |" << endl;
    cout << "+------------------+------------------+------+" << endl;
    
    for (int i = 0; i < size; i++) {
        cout << "| " << entries[i].destination.getString();
        // Padding
        for (int j = entries[i].destination.length(); j < 16; j++) cout << " ";
        cout << " | " << entries[i].nextHop.getString();
        // Padding
        for (int j = entries[i].nextHop.length(); j < 16; j++) cout << " ";
        cout << " | " << entries[i].cost << "    |" << endl;
    }
    
    cout << "+------------------+------------------+------+" << endl;
}

int RoutingTable::getSize() const {
    return size;
}

RoutingTableEntry RoutingTable::getEntry(int index) const {
    if (index >= 0 && index < size) {
        return entries[index];
    }
    return RoutingTableEntry();
}

// RouterMap implementation
RouterMap::RouterMap() : size(0) {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        entries[i].used = false;
        entries[i].value = nullptr;
    }
}

RouterMap::~RouterMap() {
    // Note: This doesn't delete the Router pointers, as they're managed elsewhere
}

bool RouterMap::put(const String& key, Router* value) {
    // Check if key already exists
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used && entries[i].key == key) {
            entries[i].value = value;
            return true;
        }
    }
    
    // Find an empty slot
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (!entries[i].used) {
            entries[i].key = key;
            entries[i].value = value;
            entries[i].used = true;
            size++;
            return true;
        }
    }
    
    return false; // Map is full
}

Router* RouterMap::get(const String& key) const {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used && entries[i].key == key) {
            return entries[i].value;
        }
    }
    return nullptr;
}

bool RouterMap::contains(const String& key) const {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used && entries[i].key == key) {
            return true;
        }
    }
    return false;
}

bool RouterMap::remove(const String& key) {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used && entries[i].key == key) {
            entries[i].used = false;
            entries[i].value = nullptr;
            size--;
            return true;
        }
    }
    return false;
}

int RouterMap::getSize() const {
    return size;
}

String RouterMap::getKey(int index) const {
    int count = 0;
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used) {
            if (count == index) {
                return entries[i].key;
            }
            count++;
        }
    }
    return String();
}

// QueueMap implementation
QueueMap::QueueMap() : size(0) {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        entries[i].used = false;
    }
}

Queue* QueueMap::get(const String& key) {
    // Check if key exists
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used && entries[i].key == key) {
            return &entries[i].value;
        }
    }
    
    // If key doesn't exist, create a new entry
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (!entries[i].used) {
            entries[i].key = key;
            entries[i].used = true;
            size++;
            return &entries[i].value;
        }
    }
    
    return nullptr; // Map is full
}

bool QueueMap::contains(const String& key) const {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used && entries[i].key == key) {
            return true;
        }
    }
    return false;
}

void QueueMap::addEntry(const String& key) {
    // Check if key already exists
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used && entries[i].key == key) {
            return;
        }
    }
    
    // Find an empty slot
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (!entries[i].used) {
            entries[i].key = key;
            entries[i].used = true;
            size++;
            return;
        }
    }
}

int QueueMap::getSize() const {
    return size;
}

String QueueMap::getKey(int index) const {
    int count = 0;
    for (int i = 0; i < MAX_ROUTERS; i++) {
        if (entries[i].used) {
            if (count == index) {
                return entries[i].key;
            }
            count++;
        }
    }
    return String();
}

// Graph implementation
Graph::Graph() {
    numRouters = 0;
    routerIds = new String[MAX_ROUTERS];
    
    // Initialize adjacency matrix with all edges set to INFINITY_COST
    adjacencyMatrix = new int*[MAX_ROUTERS];
    for (int i = 0; i < MAX_ROUTERS; i++) {
        adjacencyMatrix[i] = new int[MAX_ROUTERS];
        for (int j = 0; j < MAX_ROUTERS; j++) {
            adjacencyMatrix[i][j] = INFINITY_COST;
        }
        // Set diagonal (self) to 0
        adjacencyMatrix[i][i] = 0;
    }
}

Graph::~Graph() {
    // Delete all routers
    for (int i = 0; i < MAX_ROUTERS; i++) {
        String routerId = routers.getKey(i);
        if (routerId.empty()) {
            break;
        }
        Router* router = routers.get(routerId);
        if (router) {
            delete router;
        }
    }
    
    // Delete adjacency matrix
    if (adjacencyMatrix) {
        for (int i = 0; i < numRouters; i++) {
            delete[] adjacencyMatrix[i];
        }
        delete[] adjacencyMatrix;
    }
    
    // Delete router IDs array
    if (routerIds) {
        delete[] routerIds;
    }
}

int Graph::getRouterIndex(const String& routerId) const {
    for (int i = 0; i < numRouters; i++) {
        if (routerIds[i] == routerId) {
            return i;
        }
    }
    return -1;
}

bool Graph::addRouter(const String& routerId) {
    if (routerExists(routerId)) {
        return false;
    }
    
    // Create new router
    Router* router = new Router(routerId, this);
    
    // Add to router map
    if (!routers.put(routerId, router)) {
        delete router;
        return false;
    }
    
    // Update router IDs array and adjacency matrix
    String* newRouterIds = new String[numRouters + 1];
    int** newMatrix = new int*[numRouters + 1];
    
    // Copy existing router IDs
    for (int i = 0; i < numRouters; i++) {
        newRouterIds[i] = routerIds ? routerIds[i] : String();
        newMatrix[i] = new int[numRouters + 1];
        
        // Copy existing row and add new column with INFINITY
        for (int j = 0; j < numRouters; j++) {
            newMatrix[i][j] = adjacencyMatrix ? adjacencyMatrix[i][j] : INFINITY_COST;
        }
        newMatrix[i][numRouters] = INFINITY_COST;
    }
    
    // Add new router ID and row
    newRouterIds[numRouters] = routerId;
    newMatrix[numRouters] = new int[numRouters + 1];
    for (int j = 0; j <= numRouters; j++) {
        newMatrix[numRouters][j] = INFINITY_COST;
    }
    
    // Self-loop costs are 0
    for (int i = 0; i <= numRouters; i++) {
        newMatrix[i][i] = 0;
    }
    
    // Delete old arrays
    if (routerIds) {
        delete[] routerIds;
    }
    
    if (adjacencyMatrix) {
        for (int i = 0; i < numRouters; i++) {
            delete[] adjacencyMatrix[i];
        }
        delete[] adjacencyMatrix;
    }
    
    // Update with new arrays
    routerIds = newRouterIds;
    adjacencyMatrix = newMatrix;
    numRouters++;
    
    return true;
}

bool Graph::removeRouter(const String& routerId) {
    int index = getRouterIndex(routerId);
    if (index == -1) {
        return false;
    }
    
    // Delete the router
    Router* router = routers.get(routerId);
    if (router) {
        delete router;
        routers.remove(routerId);
    }
    
    // Update router IDs array and adjacency matrix
    String* newRouterIds = new String[numRouters - 1];
    int** newMatrix = new int*[numRouters - 1];
    
    // Copy router IDs and matrix, skipping the removed router
    int newIndex = 0;
    for (int i = 0; i < numRouters; i++) {
        if (i != index) {
            newRouterIds[newIndex] = routerIds[i];
            newMatrix[newIndex] = new int[numRouters - 1];
            
            int newCol = 0;
            for (int j = 0; j < numRouters; j++) {
                if (j != index) {
                    newMatrix[newIndex][newCol] = adjacencyMatrix[i][j];
                    newCol++;
                }
            }
            
            newIndex++;
        }
    }
    
    // Delete old arrays
    if (routerIds) {
        delete[] routerIds;
    }
    
    if (adjacencyMatrix) {
        for (int i = 0; i < numRouters; i++) {
            delete[] adjacencyMatrix[i];
        }
        delete[] adjacencyMatrix;
    }
    
    // Update with new arrays
    routerIds = newRouterIds;
    adjacencyMatrix = newMatrix;
    numRouters--;
    
    return true;
}

bool Graph::addEdge(const String& fromRouter, const String& toRouter, int weight) {
    int fromIndex = getRouterIndex(fromRouter);
    int toIndex = getRouterIndex(toRouter);
    
    if (fromIndex == -1 || toIndex == -1) {
        return false;
    }
    
    // Update adjacency matrix
    adjacencyMatrix[fromIndex][toIndex] = weight;
    
    return true;
}

bool Graph::removeEdge(const String& fromRouter, const String& toRouter) {
    int fromIndex = getRouterIndex(fromRouter);
    int toIndex = getRouterIndex(toRouter);
    
    if (fromIndex == -1 || toIndex == -1) {
        return false;
    }
    
    // Set weight to INFINITY in adjacency matrix
    adjacencyMatrix[fromIndex][toIndex] = INFINITY_COST;
    
    return true;
}

bool Graph::updateEdgeWeight(const String& fromRouter, const String& toRouter, int newWeight) {
    int fromIndex = getRouterIndex(fromRouter);
    int toIndex = getRouterIndex(toRouter);
    
    if (fromIndex == -1 || toIndex == -1) {
        return false;
    }
    
    // Update weight in adjacency matrix
    adjacencyMatrix[fromIndex][toIndex] = newWeight;
    
    return true;
}

int Graph::getEdgeWeight(const String& fromRouter, const String& toRouter) const {
    int fromIndex = getRouterIndex(fromRouter);
    int toIndex = getRouterIndex(toRouter);
    
    if (fromIndex == -1 || toIndex == -1) {
        return INFINITY_COST;
    }
    
    return adjacencyMatrix[fromIndex][toIndex];
}

void Graph::displayAdjacencyMatrix() const {
    cout << "Adjacency Matrix:" << endl;
    cout << "   ";
    for (int i = 0; i < numRouters; i++) {
        cout << routerIds[i].getString() << " ";
    }
    cout << endl;
    
    for (int i = 0; i < numRouters; i++) {
        cout << routerIds[i].getString() << " ";
        for (int j = 0; j < numRouters; j++) {
            if (adjacencyMatrix[i][j] == INFINITY_COST) {
                cout << "INF ";
            } else {
                cout << adjacencyMatrix[i][j] << " ";
            }
        }
        cout << endl;
    }
}

// Dijkstra's algorithm
void Graph::runDijkstra(const String& sourceId) {
    int sourceIndex = getRouterIndex(sourceId);
    if (sourceIndex == -1) {
        return;
    }
    
    // Initialize arrays for Dijkstra's algorithm
    int* dist = new int[numRouters];
    int* prev = new int[numRouters];
    bool* visited = new bool[numRouters];
    
    // Initialize distances and visited array
    for (int i = 0; i < numRouters; i++) {
        dist[i] = INFINITY_COST;
        prev[i] = -1;
        visited[i] = false;
    }
    
    // Distance to source is 0
    dist[sourceIndex] = 0;
    
    // Main Dijkstra loop
    for (int count = 0; count < numRouters - 1; count++) {
        // Find minimum distance vertex
        int u = -1;
        int minDist = INFINITY_COST;
        for (int i = 0; i < numRouters; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }
        
        // If no unvisited vertices have finite distance, break
        if (u == -1) {
            break;
        }
        
        // Mark as visited
        visited[u] = true;
        
        // Update distances to neighbors
        for (int v = 0; v < numRouters; v++) {
            if (!visited[v] && adjacencyMatrix[u][v] != INFINITY_COST) {
                int alt = dist[u] + adjacencyMatrix[u][v];
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                }
            }
        }
    }
    
    // Update routing table for source router
    Router* sourceRouter = getRouter(sourceId);
    if (sourceRouter) {
        sourceRouter->clearRoutingTable();
        
        for (int i = 0; i < numRouters; i++) {
            if (i != sourceIndex && dist[i] != INFINITY_COST) {
                // Find the next hop - backtrack from destination
                int nextHopIndex = i;
                while (prev[nextHopIndex] != sourceIndex && prev[nextHopIndex] != -1) {
                    nextHopIndex = prev[nextHopIndex];
                }
                
                // Add to routing table
                if (nextHopIndex != -1) {
                    sourceRouter->addRoute(routerIds[i], routerIds[nextHopIndex], dist[i]);
                }
            }
        }
    }
    
    // Clean up
    delete[] dist;
    delete[] prev;
    delete[] visited;
}

void Graph::buildRoutingTables() {
    // Run Dijkstra's algorithm for each router
    for (int i = 0; i < numRouters; i++) {
        runDijkstra(routerIds[i]);
    }
    
    cout << "Routing tables updated using Dijkstra's algorithm." << endl;
}

bool Graph::routerExists(const String& routerId) const {
    return routers.contains(routerId);
}

void Graph::deliverMessage(const Message& message, const String& fromRouter) {
    Router* destRouter = getRouter(message.destination);
    if (destRouter) {
        // Make a copy of the message so we can modify its path
        Message msgCopy = message;
        destRouter->receiveMessage(msgCopy);
    } else {
        cout << "Error: Cannot deliver message to router " << message.destination.getString() 
             << " as it does not exist." << endl;
    }
}

void Graph::displayAllRouters() const {
    cout << "Network Routers:" << endl;
    for (int i = 0; i < MAX_ROUTERS; i++) {
        String routerId = routers.getKey(i);
        if (routerId.empty()) {
            break;
        }
        Router* router = routers.get(routerId);
        if (router) {
            cout << "Router " << routerId.getString() << ":" << endl;
            router->displayRoutingTable();
            cout << endl;
        }
    }
}

void Graph::sendAll() {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        String routerId = routers.getKey(i);
        if (routerId.empty()) {
            break;
        }
        Router* router = routers.get(routerId);
        if (router) {
            router->sendAll();
        }
    }
}

// Router implementation
Router::Router(const String& id, Graph* graph) : routerId(id), network(graph) {}

Router::~Router() {}

bool Router::addRoute(const String& destination, const String& nextHop, int cost) {
    return routingTable.add(destination, nextHop, cost);
}

bool Router::removeRoute(const String& destination) {
    return routingTable.remove(destination);
}

bool Router::updateRoute(const String& destination, const String& nextHop, int cost) {
    return routingTable.update(destination, nextHop, cost);
}

RoutingTableEntry* Router::findRoute(const String& destination) {
    return routingTable.find(destination);
}

void Router::displayRoutingTable() const {
    cout << "Routing Table for " << routerId.getString() << ":" << endl;
    routingTable.display();
}

bool Router::loadRoutingTableFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        cout << "Error: Could not open file " << filename << " for reading." << endl;
        return false;
    }
    
    char destBuf[MAX_STRING_LENGTH], nextBuf[MAX_STRING_LENGTH];
    int cost;
    int count = 0;
    
    while (fscanf(file, "%s %s %d", destBuf, nextBuf, &cost) == 3) {
        addRoute(String(destBuf), String(nextBuf), cost);
        count++;
    }
    
    fclose(file);
    cout << "Loaded " << count << " routing table entries from " << filename << endl;
    return true;
}

bool Router::saveRoutingTableToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        cout << "Error: Could not open file " << filename << " for writing." << endl;
        return false;
    }
    
    for (int i = 0; i < routingTable.getSize(); i++) {
        RoutingTableEntry entry = routingTable.getEntry(i);
        fprintf(file, "%s %s %d\n", 
                entry.destination.getString(), 
                entry.nextHop.getString(), 
                entry.cost);
    }
    
    fclose(file);
    cout << "Saved " << routingTable.getSize() << " routing table entries to " << filename << endl;
    return true;
}

void Router::sendMessage(int priority, const String& source, const String& destination, const String& payload) {
    Message message(priority, source, destination, payload);
    message.addToPath(routerId);
    
    cout << "Message " << message.id << " created: " << priority << " " 
         << source.getString() << " " << destination.getString() << " " << payload.getString() << endl;
    
    if (destination == routerId) {
        // Message is for this router
        cout << "Message " << message.id << " is for this router, delivered immediately." << endl;
    } else {
        // Find the next hop
        RoutingTableEntry* route = findRoute(destination);
        if (route) {
            Queue* outQueue = outgoingQueues.get(route->nextHop);
            if (outQueue) {
                outQueue->push(message);
                cout << "Message " << message.id << " queued for sending to " 
                     << route->nextHop.getString() << endl;
            } else {
                cout << "Error: Failed to queue message " << message.id << endl;
            }
        } else {
            cout << "Error: No route to destination " << destination.getString() << endl;
        }
    }
}

void Router::receiveMessage(const Message& message) {
    // Copy the message so we can modify it
    Message msgCopy = message;
    msgCopy.addToPath(routerId);
    
    if (message.destination == routerId) {
        // Message is for this router
        cout << "Message " << message.id << " received at destination " << routerId.getString() << endl;
        cout << "Message path: ";
        for (int i = 0; i < msgCopy.pathLength; i++) {
            if (i > 0) cout << " -> ";
            cout << msgCopy.path[i].getString();
        }
        cout << endl;
        cout << "Message content: " << msgCopy.payload.getString() << endl;
        
        // Save the path to path.txt
        FILE* pathFile = fopen("path.txt", "a");
        if (pathFile) {
            fprintf(pathFile, "Message %d path: ", msgCopy.id);
            for (int i = 0; i < msgCopy.pathLength; i++) {
                if (i > 0) fprintf(pathFile, " -> ");
                fprintf(pathFile, "%s", msgCopy.path[i].getString());
            }
            fprintf(pathFile, "\nContent: %s\n", msgCopy.payload.getString());
            
            // Add total cost based on routing tables
            int totalCost = 0;
            for (int i = 1; i < msgCopy.pathLength; i++) {
                String prevRouter = msgCopy.path[i-1];
                String currRouter = msgCopy.path[i];
                
                Router* prevRouterObj = network->getRouter(prevRouter);
                if (prevRouterObj) {
                    RoutingTableEntry* entry = prevRouterObj->findRoute(currRouter);
                    if (entry) {
                        totalCost += entry->cost;
                    }
                }
            }
            
            fprintf(pathFile, "Total cost: %d\n\n", totalCost);
            fclose(pathFile);
        }
    } else {
        // Message is for another router, queue it
        incomingQueue.push(msgCopy);
        cout << "Message " << message.id << " received at router " << routerId.getString() 
             << " and added to incoming queue." << endl;
    }
}

bool Router::processMessage() {
    if (incomingQueue.empty()) {
        cout << "No messages in incoming queue." << endl;
        return false;
    }
    
    Message message = incomingQueue.pop();
    cout << "Processing message " << message.id << " from incoming queue..." << endl;
    
    // Simulated 1-second delay handled in the send method
    
    // Find the next hop
    RoutingTableEntry* route = findRoute(message.destination);
    if (route) {
        Queue* outQueue = outgoingQueues.get(route->nextHop);
        if (outQueue) {
            outQueue->push(message);
            cout << "Message " << message.id << " forwarded to outgoing queue for " 
                 << route->nextHop.getString() << endl;
            return true;
        } else {
            cout << "Error: Failed to queue message " << message.id << endl;
            return false;
        }
    } else {
        cout << "Error: No route to destination " << message.destination.getString() << endl;
        return false;
    }
}

void Router::sendAll() {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        String nextHop = outgoingQueues.getKey(i);
        if (nextHop.empty()) {
            continue;
        }
        
        Queue* outQueue = outgoingQueues.get(nextHop);
        if (!outQueue || outQueue->empty()) {
            continue;
        }
        
        Router* nextRouter = network->getRouter(nextHop);
        if (!nextRouter) {
            cout << "Error: Cannot find router " << nextHop.getString() << " to forward messages." << endl;
            continue;
        }
        
        while (!outQueue->empty()) {
            Message message = outQueue->pop();
            cout << "Sending message " << message.id << " to " << nextHop.getString() << endl;
            
            // Simulate a 1-second delay per hop
            cout << "Simulating 1-second delay..." << endl;
            
            // Sleep for 1 second (platform-independent method)
            clock_t startTime = clock();
            while (clock() - startTime < CLOCKS_PER_SEC) {
                // Busy wait
            }
            
            nextRouter->receiveMessage(message);
        }
    }
}

void Router::displayPathTo(const String& destination) {
    // First check if the destination router exists
    if (!network->routerExists(destination)) {
        cout << "Error: Destination router " << destination.getString() << " does not exist." << endl;
        return;
    }
    
    String path[MAX_ROUTERS];
    int pathLength = 0;
    
    pathTo(destination, path, pathLength);
    
    if (pathLength == 0) {
        cout << "No path found from " << routerId.getString() << " to " << destination.getString() << endl;
        return;
    }
    
    cout << "Path from " << routerId.getString() << " to " << destination.getString() << ": ";
    for (int i = 0; i < pathLength; i++) {
        if (i > 0) cout << " -> ";
        cout << path[i].getString();
    }
    cout << endl;
    
    // Calculate total cost
    int totalCost = 0;
    String currentRouter = routerId;
    
    // Only calculate costs for direct routes we know about
    for (int i = 1; i < pathLength; i++) {
        RoutingTableEntry* route = findRoute(path[i]);
        if (route) {
            totalCost += route->cost;
        }
    }
    
    cout << "Total cost: " << totalCost << endl;
}

const String& Router::getId() const {
    return routerId;
}

int Router::getIncomingQueueSize() const {
    return incomingQueue.getSize();
}

int Router::getOutgoingQueueSize(const String& destination) const {
    for (int i = 0; i < MAX_ROUTERS; i++) {
        String dest = outgoingQueues.getKey(i);
        if (dest.empty()) {
            break;
        }
        if (dest == destination) {
            Queue* queue = const_cast<QueueMap&>(outgoingQueues).get(destination);
            if (queue) {
                return queue->getSize();
            }
            break;
        }
    }
    return 0;
}

int Router::getTotalOutgoingQueueSize() const {
    int total = 0;
    for (int i = 0; i < MAX_ROUTERS; i++) {
        String dest = outgoingQueues.getKey(i);
        if (dest.empty()) {
            break;
        }
        Queue* queue = const_cast<QueueMap&>(outgoingQueues).get(dest);
        if (queue) {
            total += queue->getSize();
        }
    }
    return total;
}

// Additional Router methods
void Router::clearRoutingTable() {
    routingTable.clear();
}

void RoutingTable::clear() {
    size = 0;
}

// Path finding operations
int Graph::findShortestPath(const String& source, const String& destination, String path[], int& pathLength) {
    int sourceIndex = getRouterIndex(source);
    int destIndex = getRouterIndex(destination);
    
    if (sourceIndex == -1 || destIndex == -1) {
        pathLength = 0;
        return INFINITY_COST;
    }
    
    // Initialize arrays for Dijkstra's algorithm
    int* dist = new int[numRouters];
    int* prev = new int[numRouters];
    bool* visited = new bool[numRouters];
    
    // Initialize distances and visited array
    for (int i = 0; i < numRouters; i++) {
        dist[i] = INFINITY_COST;
        prev[i] = -1;
        visited[i] = false;
    }
    
    // Distance to source is 0
    dist[sourceIndex] = 0;
    
    // Main Dijkstra loop
    for (int count = 0; count < numRouters - 1; count++) {
        // Find minimum distance vertex
        int u = -1;
        int minDist = INFINITY_COST;
        for (int i = 0; i < numRouters; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }
        
        // If no unvisited vertices have finite distance, break
        if (u == -1) {
            break;
        }
        
        // Mark as visited
        visited[u] = true;
        
        // If destination is reached, break
        if (u == destIndex) {
            break;
        }
        
        // Update distances to neighbors
        for (int v = 0; v < numRouters; v++) {
            if (!visited[v] && adjacencyMatrix[u][v] != INFINITY_COST) {
                int alt = dist[u] + adjacencyMatrix[u][v];
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                }
            }
        }
    }
    
    // If destination is unreachable
    if (dist[destIndex] == INFINITY_COST) {
        pathLength = 0;
        delete[] dist;
        delete[] prev;
        delete[] visited;
        return INFINITY_COST;
    }
    
    // Reconstruct path
    int pathIndex = 0;
    for (int at = destIndex; at != -1; at = prev[at]) {
        path[pathIndex++] = routerIds[at];
    }
    
    // Reverse path (it's currently destination to source)
    for (int i = 0; i < pathIndex / 2; i++) {
        String temp = path[i];
        path[i] = path[pathIndex - 1 - i];
        path[pathIndex - 1 - i] = temp;
    }
    
    pathLength = pathIndex;
    
    // Clean up
    delete[] dist;
    delete[] prev;
    delete[] visited;
    
    return dist[destIndex];
}

// Updated Router::pathTo method to use Graph's findShortestPath
void Router::pathTo(const String& destination, String path[], int& pathLength) {
    pathLength = 0;
    
    // If source and destination are the same
    if (destination == routerId) {
        path[pathLength++] = routerId;
        return;
    }
    
    // Use graph's shortest path algorithm
    int cost = network->findShortestPath(routerId, destination, path, pathLength);
    
    if (pathLength == 0) {
        // No path found, try using routing table as backup
        RoutingTableEntry* route = findRoute(destination);
        if (route) {
            path[pathLength++] = routerId;
            path[pathLength++] = route->nextHop;
            
            // If next hop is not the destination, add destination
            if (route->nextHop != destination) {
                path[pathLength++] = destination;
            }
        }
    }
}

// Implementation for getRouter method
Router* Graph::getRouter(const String& routerId) const {
    return routers.get(routerId);
} 