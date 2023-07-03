#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

// Base class for replicas using CRTP
template <typename Derived>
class Replica {
public:
    void update(int replicaId, int value) {
        static_cast<Derived*>(this)->updateImpl(replicaId, value);
    }

    int getValue(int replicaId) {
        return static_cast<Derived*>(this)->getValueImpl(replicaId);
    }
};

// Concrete implementation of the replica interface
class RealReplica : public Replica<RealReplica> {
private:
    std::vector<int> replicas;

public:
    RealReplica(int numReplicas) : replicas(numReplicas, 0) {}

    void updateImpl(int replicaId, int value) {
        replicas[replicaId] = value;
    }

    int getValueImpl(int replicaId) {
        return replicas[replicaId];
    }
};

// Proxy class for the replica interface using CRTP
template <typename RealReplicaType>
class ReplicaProxy : public Replica<ReplicaProxy<RealReplicaType>> {
private:
    RealReplicaType realReplica;

public:
    ReplicaProxy(int numReplicas) : realReplica(numReplicas) {}

    void updateImpl(int replicaId, int value) {
        // Perform additional operations before forwarding the update to the real replica
        // For example, you can add caching or access control logic here
        std::cout << "Performing additional operations before update" << std::endl;

        realReplica.update(replicaId, value);
    }

    int getValueImpl(int replicaId) {
        // Perform additional operations before retrieving the value from the real replica
        // For example, you can add lazy loading or caching logic here
        std::cout << "Performing additional operations before getting value" << std::endl;

        return realReplica.getValue(replicaId);
    }
};

int main() {
    ReplicaProxy<RealReplica> replicaProxy(3);

    // Update replicas through the proxy
    replicaProxy.update(0, 10);
    replicaProxy.update(1, 20);
    replicaProxy.update(2, 30);

    // Get values through the proxy
    std::cout << "Replica 0 value: " << replicaProxy.getValue(0) << std::endl;
    std::cout << "Replica 1 value: " << replicaProxy.getValue(1) << std::endl;
    std::cout << "Replica 2 value: " << replicaProxy.getValue(2) << std::endl;

    return 0;
}
