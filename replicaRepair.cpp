#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>


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
    // Remove the observer from the list


    auto start  = observers.begin();
    auto end = observers.end();

    // std::remove()



    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}


    void update(const std::string& key, const std::string& value) {
        data[key] = value;
        notifyObservers(key, value);
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
};

// Proxy class for the replica interface with active repair
class ActiveRepairProxy : public Replica {
private:
    FollowerReplica followerReplica;
    std::unordered_map<std::string, std::string> lastReadValues;
    std::vector<Observer*> observers;

public:
    void update(const std::string& key, const std::string& value) override {
        followerReplica.update(key, value);
        // Perform active repair on other replicas after write
        repairReplicas(key, value);
    }

    std::string read(const std::string& key) override {
        if (lastReadValues.count(key) > 0) {
            return lastReadValues[key];
        } else {
            std::string value = followerReplica.read(key);
            // Perform active repair on other replicas after read
            repairReplicas(key, value);
            lastReadValues[key] = value;
            return value;
        }
    }

    void registerObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void unregisterObserver(Observer* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }
    int getNumReplicas() const override {
        return followerReplica.getNumReplicas();
    }

private:
   private:
    void repairReplicas(const std::string& key, const std::string& value) {
        std::vector<std::string> inconsistentReplicas;

        // Check for inconsistent replicas
        for (int i = 0; i < getNumReplicas(); i++) {
            std::string replicaValue = followerReplica.read(key);
            if (replicaValue != value) {
                inconsistentReplicas.push_back(replicaValue);
            }
        }

        // Update inconsistent replicas with the latest value
        for (const auto& replicaValue : inconsistentReplicas) {
            followerReplica.update(key, value);
        }
    }

};

int main() {
    // Create replicas (observers)
    ReplicaObserver replica1("Replica 1");
    ReplicaObserver replica2("Replica 2");

    // Create the coordinator
    Coordinator coordinator;

    // Register the replicas as observers to the coordinator
    coordinator.registerObserver(&replica1);
    coordinator.registerObserver(&replica2);

    // Create an instance of the ActiveRepairProxy
    ActiveRepairProxy proxy;

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

    return 0;
}
