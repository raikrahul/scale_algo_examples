#include <iostream>
#include <vector>

// Structure representing a row in the table
struct Row {
    int id;
    std::string staticData;
    std::string dynamicData;
};

// Function to split a row into static and dynamic parts
std::pair<std::string, std::string> splitRow(const Row& row) {
    // Split the row based on the desired partitioning strategy
    return std::make_pair(row.staticData, row.dynamicData);
}

int main() {
    // Sample rows in the table
    std::vector<Row> rows{
        {1, "Static 1", "Dynamic 1"},
        {2, "Static 2", "Dynamic 2"},
        {3, "Static 3", "Dynamic 3"},
        // Add more rows...
    };

    // Vectors to hold the vertical partitions
    std::vector<std::string> staticPartition;
    std::vector<std::string> dynamicPartition;

    // Split each row into static and dynamic parts and store them in the respective partitions
    for (const auto& row : rows) {
        auto split = splitRow(row);
        staticPartition.push_back(split.first);
        dynamicPartition.push_back(split.second);
    }

    // Print the contents of each partition
    std::cout << "Static Partition:\n";
    for (const auto& data : staticPartition) {
        std::cout << data << "\n";
    }
    std::cout << std::endl;

    std::cout << "Dynamic Partition:\n";
    for (const auto& data : dynamicPartition) {
        std::cout << data << "\n";
    }
    std::cout << std::endl;

    return 0;
}
