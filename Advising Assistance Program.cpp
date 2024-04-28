//============================================================================
// Name        : Advising Assistance Program.cpp
// Author      : Jarrod Helmers 20240418
// Version     : 1.0
// Description : Final Project for CS300
//============================================================================


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// Structure to hold course information
struct Course {
    string courseId;
    string courseName;
    vector<string> preReqs;
};

// Binary search tree node structure
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node() : left(nullptr), right(nullptr) {}
    Node(Course t_course) : course(t_course), left(nullptr), right(nullptr) {}
};

// Binary search tree class for managing courses
class BinarySearchTree {
private:
    Node* root;

    // Helper function to add nodes to the BST
    void addNode(Node*& node, Course course) {
        if (node == nullptr) {
            node = new Node(course);
        }
        else if (course.courseId <= node->course.courseId) {
            addNode(node->left, course);
        }
        else {
            addNode(node->right, course);
        }
    }

    // Helper function for in-order traversal to print courses
    void inOrder(Node* node) {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseId << ", " << node->course.courseName;
            for (const string& preReq : node->course.preReqs) {
                cout << ", " << preReq;
            }
            cout << endl;
            inOrder(node->right);
        }
    }

    // Helper function to clear memory recursively
    void clear(Node* node) {
        if (node != nullptr) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() {
        clear(root);
    }

    void inOrder() {
        inOrder(root);
    }

    Course search(string t_key) {
        Node* currentNode = root;
        while (currentNode != nullptr) {
            if (currentNode->course.courseId == t_key) {
                return currentNode->course;
            }
            else if (currentNode->course.courseId > t_key) {
                currentNode = currentNode->left;
            }
            else {
                currentNode = currentNode->right;
            }
        }
        return Course(); // Return an empty course if not found
    }

    void insert(Course course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            addNode(root, course);
        }
    }
};

// Function to parse a CSV line into tokens
vector<string> parseCsv(const string& line) {
    vector<string> tokens;
    istringstream stream(line);
    string token;
    while (getline(stream, token, ',')) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to load courses from a CSV file into the BST
void loadCourse(const string& csvPath, BinarySearchTree& bst) {
    ifstream file(csvPath);
    if (!file) {
        cerr << "Failed to open file: " << csvPath << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        vector<string> tokens = parseCsv(line);
        if (tokens.size() < 2) continue;
        Course course;
        course.courseId = tokens[0];
        course.courseName = tokens[1];
        for (size_t i = 2; i < tokens.size(); ++i) {
            course.preReqs.push_back(tokens[i]);
        }
        bst.insert(course);
    }
    file.close();
}

int main(int argc, char* argv[]) {
    BinarySearchTree bst;

    int userChoice = 0;
    do {
        cout << "\n-- Menu --" << endl;
        cout << "1: Load course data" << endl;
        cout << "2: Print ordered courses" << endl;
        cout << "3: Course Search" << endl;
        cout << "9: Exit" << endl;
        cout << "Enter choice: ";
        cin >> userChoice;

        switch (userChoice) {
        case 1: {
            cout << "Enter the name of the csv file (or press enter for 'courseInfo.csv'): ";
            string csvFile;
            getline(cin >> ws, csvFile); // Properly handle whitespace
            if (csvFile.empty()) {
                csvFile = "courseInfo.csv";
            }
            loadCourse(csvFile, bst);
            break;
        }
        case 2:
            bst.inOrder();
            break;
        case 3: {
            cout << "Enter course ID: ";
            string key;
            cin >> key;
            Course course = bst.search(key);
            if (!course.courseId.empty()) {
                cout << course.courseId << ", " << course.courseName;
                for (const string& preReq : course.preReqs) {
                    cout << ", " << preReq;
                }
                cout << endl;
            }
            else {
                cout << "Course not found." << endl;
            }
            break;
        }
        case 9:
            cout << "Exiting program." << endl;
            break;
        default:
            cout << "Invalid option. Please try again." << endl;
            break;
        }
    } while (userChoice != 9);

    return 0;
}
