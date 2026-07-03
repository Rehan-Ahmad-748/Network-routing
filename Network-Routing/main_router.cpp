#include "Graph.h"
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

void displayMenu() {
    cout << "\n==============================================" << endl;
    cout << "         NETWORK ROUTER SIMULATOR             " << endl;
    cout << "==============================================" << endl;
    cout << "1. Display all routers" << endl;
    cout << "2. Display routing table for a router" << endl;
    cout << "3. Send a message" << endl;
    cout << "4. Process incoming messages" << endl;
    cout << "5. Send all outgoing messages" << endl;
    cout << "6. Display path between routers" << endl;
    cout << "7. Reload network from CSV file" << endl;
    cout << "8. Run Dijkstra algorithm to update routing tables" << endl;
    cout << "9. Display network adjacency matrix" << endl;
    cout << "10. Update edge weight" << endl;
    cout << "11. Add/remove edge" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter your choice: ";
}

// Function to parse the CSV file and create routers and routing tables
bool loadNetworkFromCSV(Graph& network, const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return false;
    }
    
    cout << "Loading network from " << filename << "..." << endl;
    
    // Read the header to get router/node names
    string line;
    if (!getline(file, line)) {
        cout << "Error: File is empty or cannot read header" << endl;
        file.close();
        return false;
    }
    
    // Parse header to get node names
    char* headerLine = new char[line.length() + 1];
    strcpy(headerLine, line.c_str());
    
    const int MAX_NODES = 50;
    String nodeNames[MAX_NODES];
    int nodeCount = 0;
    
    // Get column headers (node names)
    char* token = strtok(headerLine, ",");
    while (token != NULL && nodeCount < MAX_NODES) {
        if (strlen(token) > 0) { // Skip the first empty cell
            nodeNames[nodeCount++] = String(token);
        }
        token = strtok(NULL, ",");
    }
    
    delete[] headerLine;
    
    if (nodeCount == 0) {
        cout << "Error: No node names found in header" << endl;
        file.close();
        return false;
    }
    
    cout << "Found " << nodeCount << " nodes in the network" << endl;
    
    // Create routers for all nodes starting with "R"
    for (int i = 0; i < nodeCount; i++) {
        const char* nodeName = nodeNames[i].getString();
        if (nodeName[0] == 'R') {
            if (!network.routerExists(nodeNames[i])) {
                network.addRouter(nodeNames[i]);
                cout << "Created router " << nodeName << endl;
            }
        }
    }
    
    // Count how many routers we created
    int routerCount = 0;
    for (int i = 0; i < nodeCount; i++) {
        if (nodeNames[i].getString()[0] == 'R') {
            routerCount++;
        }
    }
    
    if (routerCount == 0) {
        cout << "Error: No routers (starting with 'R') found in the network" << endl;
        file.close();
        return false;
    }
    
    // Parse each line to create routing tables and build adjacency matrix
    int lineNum = 0;
    while (getline(file, line) && lineNum < MAX_NODES) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        char* dataLine = new char[line.length() + 1];
        strcpy(dataLine, line.c_str());
        
        // First token is the source node
        char* token = strtok(dataLine, ",");
        if (token == NULL) {
            delete[] dataLine;
            continue;
        }
        
        String sourceNode(token);
        cout << "Processing routes for node: " << sourceNode.getString() << endl;
        
        // Process each cell in the row
        int col = 0;
        while ((token = strtok(NULL, ",")) != NULL && col < nodeCount) {
            // If the token is a number (not "?"), add a route or edge
            if (token[0] != '?') {
                int cost = atoi(token);
                if (cost > 0) {
                    // If source and destination are both routers, add an edge
                    if (sourceNode.getString()[0] == 'R' && nodeNames[col].getString()[0] == 'R') {
                        network.addEdge(sourceNode, nodeNames[col], cost);
                        cout << "Added edge: " << sourceNode.getString() << " -> " 
                             << nodeNames[col].getString() << " (weight: " << cost << ")" << endl;
                    }
                    
                    // If source is a router, add to routing table
                    if (sourceNode.getString()[0] == 'R') {
                        Router* router = network.getRouter(sourceNode);
                        if (router) {
                            router->addRoute(nodeNames[col], nodeNames[col], cost);
                            cout << "Added route: " << sourceNode.getString() << " -> " 
                                 << nodeNames[col].getString() << " (cost: " << cost << ")" << endl;
                        }
                    }
                }
            }
            col++;
        }
        
        delete[] dataLine;
        lineNum++;
    }
    
    file.close();
    
    // Run Dijkstra's algorithm to build optimal routing tables
    network.buildRoutingTables();
    
    cout << "Network loaded successfully!" << endl;
    return true;
}

int main() {
    Graph network;
    int choice;
    char idBuffer[MAX_STRING_LENGTH], destBuffer[MAX_STRING_LENGTH], payloadBuffer[MAX_STRING_LENGTH];
    int priority, weight;
    Router* router;
    
    cout << "Welcome to Network Router Simulator" << endl;
    
    // Automatically load the network from network.csv at startup
    if (!loadNetworkFromCSV(network, "network.csv")) {
        cout << "Failed to load default network. Starting with empty network." << endl;
    }
    
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: // Display all routers
                network.displayAllRouters();
                break;
                
            case 2: // Display routing table for a specific router
                cout << "Enter router ID (e.g., R1, R2): ";
                cin >> idBuffer;
                router = network.getRouter(String(idBuffer));
                
                if (router) {
                    router->displayRoutingTable();
                } else {
                    cout << "Router not found." << endl;
                }
                break;
                
            case 3: // Send a message
                cout << "Enter source router ID (e.g., R1): ";
                cin >> idBuffer;
                router = network.getRouter(String(idBuffer));
                
                if (router) {
                    cout << "Enter destination router ID (e.g., R2): ";
                    cin >> destBuffer;
                    cout << "Enter message priority (1-10): ";
                    cin >> priority;
                    cout << "Enter message payload: ";
                    cin.ignore(); // Clear the newline
                    cin.getline(payloadBuffer, MAX_STRING_LENGTH);
                    
                    router->sendMessage(priority, String(idBuffer), String(destBuffer), String(payloadBuffer));
                } else {
                    cout << "Source router not found." << endl;
                }
                break;
                
            case 4: // Process incoming messages
                cout << "Enter router ID: ";
                cin >> idBuffer;
                router = network.getRouter(String(idBuffer));
                
                if (router) {
                    if (!router->processMessage()) {
                        cout << "No messages to process or processing failed." << endl;
                    }
                } else {
                    cout << "Router not found." << endl;
                }
                break;
                
            case 5: // Send all outgoing messages
                network.sendAll();
                break;
                
            case 6: // Display path between routers
                cout << "Enter source router ID: ";
                cin >> idBuffer;
                router = network.getRouter(String(idBuffer));
                
                if (router) {
                    cout << "Enter destination router ID: ";
                    cin >> destBuffer;
                    router->displayPathTo(String(destBuffer));
                } else {
                    cout << "Source router not found." << endl;
                }
                break;
                
            case 7: // Reload network from CSV file
                {
                    char filename[MAX_STRING_LENGTH];
                    cout << "Enter CSV filename (e.g., network.csv): ";
                    cin >> filename;
                    if (loadNetworkFromCSV(network, filename)) {
                        cout << "Network reloaded successfully from " << filename << endl;
                    } else {
                        cout << "Failed to reload network from " << filename << endl;
                    }
                }
                break;
                
            case 8: // Run Dijkstra algorithm to update routing tables
                network.buildRoutingTables();
                cout << "Routing tables updated using Dijkstra's algorithm." << endl;
                break;
                
            case 9: // Display network adjacency matrix
                network.displayAdjacencyMatrix();
                break;
                
            case 10: // Update edge weight
                cout << "Enter source router ID: ";
                cin >> idBuffer;
                if (!network.routerExists(String(idBuffer))) {
                    cout << "Source router not found." << endl;
                    break;
                }
                
                cout << "Enter destination router ID: ";
                cin >> destBuffer;
                if (!network.routerExists(String(destBuffer))) {
                    cout << "Destination router not found." << endl;
                    break;
                }
                
                cout << "Enter new weight: ";
                cin >> weight;
                
                if (network.updateEdgeWeight(String(idBuffer), String(destBuffer), weight)) {
                    cout << "Edge weight updated successfully." << endl;
                    // After updating weight, rebuild routing tables
                    network.buildRoutingTables();
                } else {
                    cout << "Failed to update edge weight." << endl;
                }
                break;
                
            case 11: // Add/remove edge
                cout << "1. Add edge" << endl;
                cout << "2. Remove edge" << endl;
                cout << "Enter choice: ";
                int edgeChoice;
                cin >> edgeChoice;
                
                cout << "Enter source router ID: ";
                cin >> idBuffer;
                if (!network.routerExists(String(idBuffer))) {
                    cout << "Source router not found." << endl;
                    break;
                }
                
                cout << "Enter destination router ID: ";
                cin >> destBuffer;
                if (!network.routerExists(String(destBuffer))) {
                    cout << "Destination router not found." << endl;
                    break;
                }
                
                if (edgeChoice == 1) { // Add edge
                    cout << "Enter weight: ";
                    cin >> weight;
                    
                    if (network.addEdge(String(idBuffer), String(destBuffer), weight)) {
                        cout << "Edge added successfully." << endl;
                        // After adding edge, rebuild routing tables
                        network.buildRoutingTables();
                    } else {
                        cout << "Failed to add edge." << endl;
                    }
                } else if (edgeChoice == 2) { // Remove edge
                    if (network.removeEdge(String(idBuffer), String(destBuffer))) {
                        cout << "Edge removed successfully." << endl;
                        // After removing edge, rebuild routing tables
                        network.buildRoutingTables();
                    } else {
                        cout << "Failed to remove edge." << endl;
                    }
                } else {
                    cout << "Invalid choice." << endl;
                }
                break;
                
            case 0: // Exit
                cout << "Exiting. Goodbye!" << endl;
                break;
                
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 0);
    
    return 0;
} 