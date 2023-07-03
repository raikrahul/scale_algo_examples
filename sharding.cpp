#include <iostream>
#include <unordered_map>

// Define the number of shards
const int NUM_SHARDS = 4;

// Shard data structure
using Shard = std::unordered_map<std::string, std::string>;

// Array of shards
Shard shards[NUM_SHARDS];

int getShardIndex(const std::string& shardKey) {
    // Use the value of the shard key to determine the shard assignment
    if (shardKey == "China") {
        return 0; // Assign to the first shard
    } else if (shardKey == "Finland") {
        return 1; // Assign to the second shard
    } else if (shardKey == "USA") {
        return 2; // Assign to the third shard
    } else {
        return 3; // Assign to the fourth shard
    }
}

int getShardIndexRange(const std::string& shardKey) {
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
    int shardIndex = getShardIndexRange(shardKey);
    if (shardIndex != -1) {
        shards[shardIndex][key] = value;
    } else {
        std::cout << "Invalid shard key: " << shardKey << std::endl;
    }
}

// Retrieve value for a given key
std::string retrieveData(const std::string& key, const std::string& shardKey) {
    int shardIndex = getShardIndexRange(shardKey);
    if (shardIndex != -1) {
        const Shard& shard = shards[shardIndex];
        auto it = shard.find(key);
        if (it != shard.end()) {
            return it->second;
        }
    } else {
        std::cout << "Invalid shard key: " << shardKey << std::endl;
    }
    return ""; // Key not found
}




























void storeData(const std::string& key, const std::string& value, const std::string& shardKey) {
    int shardIndex = getShardIndex(shardKey);
    shards[shardIndex][key] = value;
}

// Retrieve value for a given key
std::string retrieveData(const std::string& key, const std::string& shardKey) {
    int shardIndex = getShardIndex(shardKey);
    const Shard& shard = shards[shardIndex];
    auto it = shard.find(key);
    if (it != shard.end()) {
        return it->second;
    }
    return ""; // Key not found
}
// Hash function to determine shard assignment
int getShardIndex(const std::string& key) {
    std::hash<std::string> hash_fn;
    return hash_fn(key) % NUM_SHARDS;
}

// Store a key-value pair
void storeData(const std::string& key, const std::string& value) {
    int shardIndex = getShardIndex(key);
    shards[shardIndex][key] = value;
}

// Retrieve value for a given key
std::string retrieveData(const std::string& key) {
    int shardIndex = getShardIndex(key);
    const Shard& shard = shards[shardIndex];
    auto it = shard.find(key);
    if (it != shard.end()) {
        return it->second;
    }
    return ""; // Key not found
}

int main() {
    // Store data objects
    storeData("key1", "value1");
    storeData("key2", "value2");
    storeData("key3", "value3");

    // Retrieve data objects
    std::cout << retrieveData("key1") << std::endl; // Output: value1
    std::cout << retrieveData("key2") << std::endl; // Output: value2
    std::cout << retrieveData("key3") << std::endl; // Output: value3

    return 0;
}
