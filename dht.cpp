#include <iostream>
#include <map>
#include <functional>
#include <string>

// Number of virtual nodes per physical node (adjust as needed)
const int numVirtualNodes = 3;

// Type alias for hash function
using HashFunction = std::function<std::size_t(const std::string&)>;

// Consistent Distributed Hash Table (DHT)
class DistributedHashTable {
public:
    DistributedHashTable(HashFunction hashFunction) : hashFunction(hashFunction) {}

    // Add a physical node to the DHT
    void AddNode(const std::string& node) {
        for (int i = 0; i < numVirtualNodes; ++i) {
            std::string virtualNode = node + "-" + std::to_string(i);
            std::size_t hash = hashFunction(virtualNode);
            dht[hash] = node;
        }
    }

    // Remove a physical node from the DHT
    void RemoveNode(const std::string& node) {
        for (int i = 0; i < numVirtualNodes; ++i) {
            std::string virtualNode = node + "-" + std::to_string(i);
            std::size_t hash = hashFunction(virtualNode);
            dht.erase(hash);
        }
    }

    // Get the physical node responsible for a key
    std::string GetNodeForKey(const std::string& key) {
        std::size_t hash = hashFunction(key);
        auto it = dht.upper_bound(hash);
        if (it == dht.end()) {
            // Wrap around to the first node if the key's hash is greater than the largest hash in the DHT
            return dht.begin()->second;
        } else {
            return it->second;
        }
    }

private:
    std::map<std::size_t, std::string> dht;
    HashFunction hashFunction;
};

// Hash function using std::hash
std::size_t stdHash(const std::string& key) {
    std::hash<std::string> hasher;
    return hasher(key);
}

int main() {
    // Create a DHT instance
    DistributedHashTable dht(stdHash);

    // Add some nodes to the DHT
    dht.AddNode("Node1");
    dht.AddNode("Node2");
    dht.AddNode("Node3");

    // Look up the responsible node for some keys
    std::cout << "Key 'apple' is mapped to node: " << dht.GetNodeForKey("apple") << std::endl;
    std::cout << "Key 'banana' is mapped to node: " << dht.GetNodeForKey("banana") << std::endl;
    std::cout << "Key 'cherry' is mapped to node: " << dht.GetNodeForKey("cherry") << std::endl;

    // Remove a node from the DHT
    dht.RemoveNode("Node2");

    // Look up the responsible node for a key again
    std::cout << "After removing Node2, key 'banana' is mapped to node: " << dht.GetNodeForKey("banana") << std::endl;

    return 0;
}
