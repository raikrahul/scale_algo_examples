#include <iostream>
#include <vector>

// Sliding Window Parameters
const int windowSize = 5;      // Length of the window
const int slidingInterval = 2; // Interval for sliding the window

// Process a window of data
void ProcessWindow(const std::vector<int>& window) {
    // Perform computations or aggregations on the windowed data
    // For example: calculate the sum, average, max, min, etc.
    int sum = 0;
    for (int value : window) {
        sum += value;
    }
    double average = static_cast<double>(sum) / window.size();

    // Output the result for the window
    std::cout << "Window: ";
    for (int value : window) {
        std::cout << value << " ";
    }
    std::cout << "| Average: " << average << std::endl;
}

int main() {
    std::vector<int> dataStream = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Process the data stream using sliding windows
    for (int i = 0; i <= dataStream.size() - windowSize; i += slidingInterval) {
        // Extract the current window of data
        std::vector<int> window(dataStream.begin() + i, dataStream.begin() + i + windowSize);

        // Process the windowed data
        ProcessWindow(window);
    }

    return 0;
}
