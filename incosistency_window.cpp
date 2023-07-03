#include <iostream>
#include <vector>
#include <memory>

// Base class for observers using CRTP
template <typename Derived>
class Observer {
public:
    void update(const std::vector<int>& replicas) {
        static_cast<Derived*>(this)->updateImpl(replicas);
    }
};

// Example observer class
class ReplicaObserver : public Observer<ReplicaObserver> {
public:
    void updateImpl(const std::vector<int>& replicas) {
        std::cout << "ReplicaObserver received an update:" << std::endl;
        for (int i = 0; i < replicas.size(); ++i) {
            std::cout << "Replica " << i << ": " << replicas[i] << std::endl;
        }
        std::cout << std::endl;
    }
};

// Subject class representing the replica system
class ReplicaSystem {
private:
    std::vector<int> replicas;
    std::vector<std::shared_ptr<Observer<ReplicaObserver>>> observers;

public:
    ReplicaSystem(int numReplicas) : replicas(numReplicas, 0) {}

    void addObserver(const std::shared_ptr<Observer<ReplicaObserver>>& observer) {
        observers.push_back(observer);
    }

    void updateReplica(int replicaId, int value) {
        replicas[replicaId] = value;
        notifyObservers();
    }

    void notifyObservers() {
        for (const auto& observer : observers) {
            observer->update(replicas);
        }
    }
};

int main() {
    ReplicaSystem replicaSystem(3);

    std::shared_ptr<ReplicaObserver> observer1 = std::make_shared<ReplicaObserver>();
    replicaSystem.addObserver(observer1);

    // Simulating updates to replicas
    replicaSystem.updateReplica(0, 10);
    replicaSystem.updateReplica(1, 20);
    replicaSystem.updateReplica(2, 30);

    return 0;
}
