#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

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
};

// Proxy class for the replica interface using CRTP
template <typename FollowerReplicaType>
class RYOWProxy : public Replica<RYOWProxy<FollowerReplicaType>> {
private:
    FollowerReplicaType followerReplica;
    std::string lastUpdatedKey;
    std::string lastUpdatedValue;

public:
    void updateImpl(const std::string& key, const std::string& value) {
        followerReplica.update(key, value);
        lastUpdatedKey = key;
        lastUpdatedValue = value;
    }

    std::string readImpl(const std::string& key) {
        if (key == lastUpdatedKey) {
            return lastUpdatedValue;
        } else {
            return followerReplica.read(key);
        }
    }
};

int main() {
    RYOWProxy<FollowerReplica> proxy;

    // Update credit card information
    proxy.update("credit_card", "new_card_details");

    // Read credit card information
    std::string creditCardInfo = proxy.read("credit_card");

    // Display the result
    std::cout << "Credit card information: " << creditCardInfo << std::endl;

    return 0;
}
