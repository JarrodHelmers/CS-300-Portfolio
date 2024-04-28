//============================================================================
// Name        : HashTable.cpp
// Author      : Jarrod Helmers
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold bids
    struct Node {
        Bid bid;
        unsigned int key;
        Node *next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a bid
        Node(Bid aBid) : Node() {
            bid = aBid;
        }

        // initialize with a bid and a key
        Node(Bid aBid, unsigned int aKey) : Node(aBid) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initialize the structures used to hold bids
    nodes.resize(tableSize); // Resizing the vector to tableSize
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    tableSize = size;
    nodes.resize(tableSize); // Resizing the vector to the specified size
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // Iterate through each node in the vector and delete only dynamically allocated nodes
    for (Node& node : nodes) {
        Node* current = node.next;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
}


/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // Calculate the hash using modulo operator to fit within the table size
    return key % tableSize;
}


/**
 * Insert a bid
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Bid bid) {
    // Calculate the hash key for the bid
    unsigned int key = hash(atoi(bid.bidId.c_str()));

    // Retrieve the node using the key
    Node* node = &nodes[key];

    // If no entry found for the key, assign this node to the key position
    if (node->key == UINT_MAX) {
        node->bid = bid;
        node->key = key;
    }
    else {
        // If node is not used, assign old node key to UNIT_MAX, set to key,
        // set old node to bid, and old node next to null pointer
        while (node->next != nullptr) {
            node = node->next;
        }
        Node* newNode = new Node(bid, key);
        node->next = newNode;
    }
}

/**
 * Print all bids
 */
void HashTable::PrintAll() {
    // Iterate through each node in the vector
    for (Node node : nodes) {
        // Check if the node is used
        if (node.key != UINT_MAX) {
            // Output key, bidID, title, amount, and fund
            cout << "Key " << node.key << ": ";
            cout << node.bid.bidId << " | " << node.bid.title << " | ";
            cout << node.bid.amount << " | " << node.bid.fund << endl;

            // Iterate through the linked list if there are collisions
            Node* current = node.next;
            while (current != nullptr) {
                cout << "Key " << node.key << ": ";
                cout << current->bid.bidId << " | " << current->bid.title << " | ";
                cout << current->bid.amount << " | " << current->bid.fund << endl;
                current = current->next;
            }
        }
    }
}


/**
 * Remove a bid
 *
 * @param bidId The bid id to search for
 */
void HashTable::Remove(string bidId) {
    // Calculate the hash key for the bidId
    unsigned int key = hash(atoi(bidId.c_str()));

    // Retrieve the node using the key
    Node* node = &nodes[key];

    // If the first node matches the bidId, remove it
    if (node->bid.bidId == bidId) {
        if (node->next == nullptr) {
            // If no collisions, clear the node
            node->key = UINT_MAX;
            node->bid = Bid();
        }
        else {
            // If there are collisions, move the next node to the current position
            Node* nextNode = node->next;
            *node = *nextNode;
            delete nextNode;
        }
        return;
    }

    // If the bidId is not found at the first node, search in the linked list
    Node* prevNode = node;
    node = node->next;
    while (node != nullptr) {
        if (node->bid.bidId == bidId) {
            prevNode->next = node->next;
            delete node;
            return;
        }
        prevNode = node;
        node = node->next;
    }

    // Output a message if the bidId is not found
    cout << "Bid Id " << bidId << " not found." << endl;
}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid HashTable::Search(string bidId) {
    // Calculate the hash key for the bidId
    unsigned int key = hash(atoi(bidId.c_str()));

    // Retrieve the node using the key
    Node* node = &nodes[key];

    // If the first node matches the bidId, return the bid
    if (node->bid.bidId == bidId) {
        return node->bid;
    }

    // If the bidId is not found at the first node, search in the linked list
    node = node->next;
    while (node != nullptr) {
        if (node->bid.bidId == bidId) {
            return node->bid;
        }
        node = node->next;
    }

    // Return an empty bid if the bidId is not found
    return Bid();
}


//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            hashTable->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98109";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
        bidKey = "98109";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a hash table to hold all the bids
    HashTable* bidTable;

    Bid bid;
    bidTable = new HashTable();
    
    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bidTable);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bidTable->PrintAll();
            break;

        case 3:
            ticks = clock();

            bid = bidTable->Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
                cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 4:
            bidTable->Remove(bidKey);
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
