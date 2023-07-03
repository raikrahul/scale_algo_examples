#include <iostream>
#include <unordered_map>
#include <vector>

// Define the number of shards and replicas per shard
const int NUM_SHARDS = 4;
const int NUM_REPLICAS = 3;

// Shard data structure
using Shard = std::unordered_map<std::string, std::string>;

// Array of shards
std::vector<Shard> shards(NUM_SHARDS);

// Hash function to determine shard assignment
int getShardIndex(const std::string& shardKey) {
    // Use the range of the shard key to determine the shard assignment
    int shardIndex = -1;
    if (shardKey >= "1000" && shardKey <= "2999") {
        shardIndex = 0; // Assign to the first shard
    } else if (shardKey >= "3000" && shardKey <= "4999") {
        shardIndex = 1; // Assign to the second shard
    } else if (shardKey >= "5000" && shardKey <= "6999") {
        shardIndex = 2; // Assign to the third shard
    } else if (shardKey >= "7000" && shardKey <= "9999") {
        shardIndex = 3; // Assign to the fourth shard
    }
    return shardIndex;
}

// Store a key-value pair
void storeData(const std::string& key, const std::string& value, const std::string& shardKey) {
    int shardIndex = getShardIndex(shardKey);
    if (shardIndex != -1) {
        for (int replicaIndex = 0; replicaIndex < NUM_REPLICAS; ++replicaIndex) {
            shards[shardIndex][key + "_" + std::to_string(replicaIndex)] = value;
        }
    } else {
        std::cout << "Invalid shard key: " << shardKey << std::endl;
    }
}

// Retrieve value for a given key
std::string retrieveData(const std::string& key, const std::string& shardKey) {
    int shardIndex = getShardIndex(shardKey);
    if (shardIndex != -1) {
        for (int replicaIndex = 0; replicaIndex < NUM_REPLICAS; ++replicaIndex) {
            const Shard& shard = shards[shardIndex];
            auto it = shard.find(key + "_" + std::to_string(replicaIndex));
            if (it != shard.end()) {
                return it->second;
            }
        }
    } else {
        std::cout << "Invalid shard key: " << shardKey << std::endl;
    }
    return ""; // Key not found
}

// Simulate a node going down (removing replicas)
void simulateNodeFailure(int shardIndex, int replicaIndex) {
    shards[shardIndex].erase("key1_" + std::to_string(replicaIndex));
    shards[shardIndex].erase("key2_" + std::to_string(replicaIndex));
    shards[shardIndex].erase("key3_" + std::to_string(replicaIndex));
}

int main() {
    // Store data objects
    storeData("key1", "value1", "1500");
    storeData("key2", "value2", "4000");
    storeData("key3", "value3", "6000");

    // Retrieve data objects
    std::cout << "Data retrieval:" << std::endl;
    std::cout << "key1: " << retrieveData("key1", "1500") << std::endl;
    std::cout << "key2: " << retrieveData("key2", "4000") << std::endl;
    std::cout << "key3: " << retrieveData("key3", "6000") << std::endl;

    // Simulate a node failure
    simulateNodeFailure(2, 1);

    // Retrieve data after node failure
    std::cout << "\nData retrieval after node failure:" << std::endl;
    std::cout << "key1: " << retrieveData("key1", "1500") << std::endl;
    std::cout << "key2: " << retrieveData("key2", "4000") << std::endl;
    std::cout << "key3: " << retrieveData("key3", "6000") << std::endl;

    return 0;
}
