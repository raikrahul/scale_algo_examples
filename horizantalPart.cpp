#include <iostream>
#include <vector>
#include <unordered_map>


// Splitting up, or partitioning data in a relational database, is a technique for distributing the database 
// over multiple independent disk partitions and database engines. 
// Precisely how partitioning is supported is highly product-specific. 
// In general, there are two strategies: horizontal partitioning and vertical partitioning.

// Horizontal partitioning splits a logical table into multiple physical partitions. 
// Individual rows are allocated to a partition based on some partitioning strategy. 
// Common partitioning strategies are to allocate rows to partitions based on some value in the row, or to use a hash function on the primary key. As shown in Figure 10-3, you can allocate a row to a partition based on the value of the region field in each row.

// Horizontal database partitioning

// Structure representing a row in the table
struct Row {
    int id;
    std::string name;
    std::string region;
};

// Function to determine the partition for a given row based on the partitioning strategy
int getPartition(const Row& row, const std::unordered_map<std::string, int>& partitionMap) {
    // Extract the partition key from the row (e.g., 'region' field)
    std::string partitionKey = row.region;

    // Find the corresponding partition in the partition map
    auto it = partitionMap.find(partitionKey);
    if (it != partitionMap.end()) {
        return it->second; // Return the partition number
    }

    // Handle the case when partition key is not found
    // You can choose to assign it to a default partition or throw an exception
    // Here, we assign it to partition 0 as a default
    return 0;
}

int main() {
    // Define the number of partitions and their mapping
    const int numPartitions = 4;
    std::unordered_map<std::string, int> partitionMap{
        {"North", 0},
        {"South", 1},
        {"East", 2},
        {"West", 3}
    };

    // Create a vector of partitions to hold the rows
    std::vector<std::vector<Row>> partitions(numPartitions);

    // Sample rows in the table
    std::vector<Row> rows{
        {1, "John", "North"},
        {2, "Emma", "South"},
        {3, "Michael", "West"},
        {4, "Sophia", "North"},
        // Add more rows...
    };

    // Distribute the rows into their respective partitions
    for (const auto& row : rows) {
        int partition = getPartition(row, partitionMap);
        partitions[partition].push_back(row);
    }

    // Print the contents of each partition
    for (int i = 0; i < numPartitions; ++i) {
        std::cout << "Partition " << i << ":\n";
        for (const auto& row : partitions[i]) {
            std::cout << "ID: " << row.id << ", Name: " << row.name << ", Region: " << row.region << "\n";
        }
        std::cout << std::endl;
    }

    return 0;
}
