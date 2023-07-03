#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <cmath>

// Abstract base class for observers
class Observer {
public:
    virtual void notifyUpdate(const std::string& key, const std::string& value) = 0;
};

// Concrete observer representing a replica
class ReplicaObserver : public Observer {
private:
    std::string name;

public:
    explicit ReplicaObserver(const std::string& name) : name(name) {}

    void notifyUpdate(const std::string& key, const std::string& value) override {
        std::cout << "Replica " << name << " received update for key: " << key << ", value: " << value << std::endl;
    }
};

class Coordinator {
private:
    std::unordered_map<std::string, std::string> data;
    std::vector<Observer*> observers;

public:
    void registerObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void unregisterObserver(Observer* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void update(const std::string& key, const std::string& value) {
        data[key] = value;
        notifyObservers(key, value);
    }

    std::unordered_map<std::string, std::string> getData() const {
        return data;
    }

private:
    void notifyObservers(const std::string& key, const std::string& value) {
        for (Observer* observer : observers) {
            observer->notifyUpdate(key, value);
        }
    }
};

// Base class for replicas
class Replica {
public:
    virtual void update(const std::string& key, const std::string& value) = 0;
    virtual std::string read(const std::string& key) = 0;
    virtual int getNumReplicas() const = 0;
    virtual std::unordered_map<std::string, std::string> getData() const = 0;
    virtual void repair(const std::unordered_map<std::string, std::string>& repairedData) = 0;
};

// Concrete implementation of the replica interface
class FollowerReplica : public Replica {
private:
    std::unordered_map<std::string, std::string> data;

public:
    void update(const std::string& key, const std::string& value) override {
        data[key] = value;
    }

    std::string read(const std::string& key) override {
        return data[key];
    }

    int getNumReplicas() const override {
        return data.size();
    }

    std::unordered_map<std::string, std::string> getData() const override {
        return data;
    }

    void repair(const std::unordered_map<std::string, std::string>& repairedData) override {
        data = repairedData;
    }
};

// Proxy class for the replica interface with passive repair using Merkle tree
class PassiveRepairProxy : public Replica {
private:
    FollowerReplica followerReplica;
    std::unordered_map<std::string, std::string> lastReadValues;
    std::vector<Observer*> observers;

public:
    void registerObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void unregisterObserver(Observer* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void update(const std::string& key, const std::string& value) override {
        followerReplica.update(key, value);
        // Perform passive repair on other replicas after write
        repairReplicas();
    }

    std::string read(const std::string& key) override {
        if (lastReadValues.count(key) > 0) {
            return lastReadValues[key];
        } else {
            std::string value = followerReplica.read(key);
            // Perform passive repair on other replicas after read
            repairReplicas();
            lastReadValues[key] = value;
            return value;
        }
    }

    int getNumReplicas() const override {
        return followerReplica.getNumReplicas();
    }

    std::unordered_map<std::string, std::string> getData() const override {
        return followerReplica.getData();
    }

    void repair(const std::unordered_map<std::string, std::string>& repairedData) override {
        followerReplica.repair(repairedData);
        lastReadValues.clear();
    }

private:
    void repairReplicas() {
        std::unordered_map<std::string, std::string> repairedData = followerReplica.getData();

        for (const auto& observer : observers) {
            Replica* replica = dynamic_cast<Replica*>(observer);
            if (replica && replica != &followerReplica) {
                std::unordered_map<std::string, std::string> replicaData = replica->getData();
                if (replicaData != repairedData) {
                    std::cout << "Performing passive repair on replica..." << std::endl;
                    replica->repair(repairedData);
                }
            }
        }
    }
};

// Function to calculate the hash of a given string using SHA-256
std::string calculateHash(const std::string& str) {
    // Placeholder implementation, use a proper hashing algorithm like SHA-256
    std::hash<std::string> hasher;
    return std::to_string(hasher(str));
}

// Function to calculate the Merkle tree root hash for a collection of hashes
std::string calculateMerkleRoot(const std::vector<std::string>& hashes) {
    if (hashes.empty()) {
        return "";
    }

    if (hashes.size() == 1) {
        return hashes[0];
    }

    std::vector<std::string> parents;
    for (size_t i = 0; i < hashes.size(); i += 2) {
        std::string hash1 = hashes[i];
        std::string hash2 = (i + 1 < hashes.size()) ? hashes[i + 1] : hashes[i];
        std::string parentHash = calculateHash(hash1 + hash2);
        parents.push_back(parentHash);
    }

    return calculateMerkleRoot(parents);
}

// Function to check if two Merkle tree root hashes match
bool verifyMerkleRoot(const std::string& hash1, const std::string& hash2) {
    return hash1 == hash2;
}

int main() {
    // Create replicas (observers)
    ReplicaObserver replica1("Replica 1");
    ReplicaObserver replica2("Replica 2");

    // Create the coordinator
    Coordinator coordinator;

    // Register the replicas as observers to the coordinator
    coordinator.registerObserver(&replica1);
    coordinator.registerObserver(&replica2);

    // Create an instance of the PassiveRepairProxy
    PassiveRepairProxy proxy;

    // Register the replicas as observers to the proxy as well
    proxy.registerObserver(&replica1);
    proxy.registerObserver(&replica2);

    // Perform updates and reads using the proxy (and coordinator)
    proxy.update("key1", "value1");
    proxy.update("key2", "value2");
    proxy.update("key3", "value3");

    std::cout << proxy.read("key1") << std::endl; // Output: value1
    std::cout << proxy.read("key2") << std::endl; // Output: value2
    std::cout << proxy.read("key3") << std::endl; // Output: value3

    // Unregister replica1 from the coordinator and the proxy
    coordinator.unregisterObserver(&replica1);
    proxy.unregisterObserver(&replica1);

    // Perform more updates and reads using the proxy (and coordinator) after replica1 is unregistered
    proxy.update("key4", "value4");
    proxy.update("key5", "value5");

    std::cout << proxy.read("key4") << std::endl; // Output: value4
    std::cout << proxy.read("key5") << std::endl; // Output: value5

    // Perform passive repair on replicas
    std::unordered_map<std::string, std::string> coordinatorData = coordinator.getData();
    std::unordered_map<std::string, std::string> proxyData = proxy.getData();
    if (coordinatorData != proxyData) {
        std::vector<std::string> coordinatorHashes;
        std::vector<std::string> proxyHashes;

        for (const auto& [key, value] : coordinatorData) {
            coordinatorHashes.push_back(calculateHash(key + value));
        }

        for (const auto& [key, value] : proxyData) {
            proxyHashes.push_back(calculateHash(key + value));
        }

        std::string coordinatorMerkleRoot = calculateMerkleRoot(coordinatorHashes);
        std::string proxyMerkleRoot = calculateMerkleRoot(proxyHashes);

        if (!verifyMerkleRoot(coordinatorMerkleRoot, proxyMerkleRoot)) {
            std::cout << "Performing passive repair on replicas..." << std::endl;
            proxy.repair(coordinatorData);
        }
    }

    return 0;
}
