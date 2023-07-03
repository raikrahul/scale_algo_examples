#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <sstream>
#include <iterator>
#include <thread>
#include <mutex>

// Mutex for synchronization
std::mutex mutex;

// Map function: takes an input and emits key-value pairs
void Map(const std::string& input, std::map<std::string, int>& intermediate) {
    std::stringstream ss(input);
    std::string word;
    while (ss >> word) {
        // Perform some computation or transformation on the input

        // Emit the key-value pair
        std::lock_guard<std::mutex> lock(mutex);
        intermediate[word]++;
    }
}

// Reduce function: takes a key and a vector of values and produces a result
void Reduce(const std::map<std::string, int>& intermediate, std::map<std::string, int>& output) {
    for (const auto& pair : intermediate) {
        const std::string& key = pair.first;
        int count = pair.second;

        // Perform some aggregation or computation on the values
        int sum = count;

        // Store the result
        std::lock_guard<std::mutex> lock(mutex);
        output[key] = sum;
    }
}

int main() {
    // Input file
    std::ifstream inputFile("wc.txt");
    if (!inputFile) {
        std::cerr << "Failed to open input file." << std::endl;
        return 1;
    }

    // Intermediate data
    std::map<std::string, int> intermediate;

    // Output data
    std::map<std::string, int> output;

    // Read input file line by line
    std::string line;
    while (std::getline(inputFile, line)) {
        // Map phase for each line of input
        std::thread mapThread(Map, line, std::ref(intermediate));
        mapThread.join();
    }

    // Reduce phase
    std::vector<std::thread> reduceThreads;
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        reduceThreads.emplace_back(Reduce, std::cref(intermediate), std::ref(output));
    }

    // Wait for all reduce threads to finish
    for (auto& thread : reduceThreads) {
        thread.join();
    }

    // Print the output
    for (const auto& pair : output) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }

    return 0;
}
