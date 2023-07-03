#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

// Base class for replicas using CRTP
template <typename Derived>
class Replica {
public:
    void update(const std::string& key, const std::string& value) {
        static_cast<Derived*>(this)->updateImpl(key, value);
    }

    std::string read(const std::string& key) {
        return static_cast<Derived*>(this)->readImpl(key);
    }
};

// Concrete implementation of the replica interface
class FollowerReplica : public Replica<FollowerReplica> {
private:
    std::unordered_map<std::string, std::string> data;

public:
    void updateImpl(const std::string& key, const std::string& value) {
        data[key] = value;
    }

    std::string readImpl(const std::string& key) {
        return data[key];
    }

    int getNumReplicas() const { return data.size(); }
};

// Proxy class for the replica interface using CRTP
template <typename FollowerReplicaType, int W, int R>
class TunableConsistencyProxy : public Replica<TunableConsistencyProxy<FollowerReplicaType, W, R>> {
private:
    FollowerReplicaType followerReplica;
    std::string lastUpdatedKey;
    std::string lastUpdatedValue;

public:
    void updateImpl(const std::string& key, const std::string& value) {
        followerReplica.update(key, value);
        lastUpdatedKey = key;
        lastUpdatedValue = value;
        // For simplicity, we assume synchronous updates to all replicas here.
        // In a real system, this would be asynchronous with a confirmation mechanism.
    }

    std::string readImpl(const std::string& key) {
        if (key == lastUpdatedKey) {
            return lastUpdatedValue;
        } else {
            // Read from R replicas and determine the latest value
            std::vector<std::string> values;
            std::unordered_map<std::string, int> valueCounts;

            // Read from R replicas
            for (int i = 0; i < R; ++i) {
                std::string value = followerReplica.read(key);
                values.push_back(value);
                valueCounts[value]++;
            }

            // Determine the latest value using quorum comparison
            std::string latestValue;
            int latestValueCount = 0;

            for (const auto& entry : valueCounts) {
                if (entry.second > latestValueCount) {
                    latestValue = entry.first;
                    latestValueCount = entry.second;
                }
            }

            return latestValue;
        }
    }
};

// Example usage
int main() {
    // Using the tunable consistency proxy with W = R = quorum
    using TunableProxy = TunableConsistencyProxy<FollowerReplica, 2, 2>;

    // Create an instance of the tunable consistency proxy
    TunableProxy proxy;

    // Update the data
    proxy.update("key1", "value1");
    proxy.update("key2", "value2");

    // Read the data
    std::cout << "Value of key1: " << proxy.read("key1") << std::endl; // Should return "value1"
    std::cout << "Value of key2: " << proxy.read("key2") << std::endl; // Should return "value2"

    return 0;
}
