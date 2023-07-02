


#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <random>


namespace horizantal_scaling
{
// Stateless server resource
struct Server {
    int id;
    int capacity;

    Server(int id, int capacity) : id(id), capacity(capacity) {}
};

// Load balancer
class LoadBalancer {
private:
    std::vector<Server> replicas;

public:
    void addServer(Server server) {
        replicas.push_back(server);
    }

    int distributeRequest() {
        std::vector<int> capacities;
        capacities.reserve(replicas.size());

        // Get the available capacities of all replicas
        std::transform(replicas.begin(), replicas.end(), std::back_inserter(capacities),
                       [](const Server& server) { return server.capacity; });

        // Find the replica with the minimum capacity
        auto minCapacityIt = std::min_element(capacities.begin(), capacities.end());
        int minCapacityIndex = std::distance(capacities.begin(), minCapacityIt);

        // Update the capacity of the selected replica
        replicas[minCapacityIndex].capacity++;

        return replicas[minCapacityIndex].id;
    }
};

int main() {
    LoadBalancer loadBalancer;

    // Create and add server replicas
    loadBalancer.addServer(Server(1, 5));
    loadBalancer.addServer(Server(2, 3));
    loadBalancer.addServer(Server(3, 4));

    // Simulate incoming requests
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 10);

    for (int i = 0; i < 10; ++i) {
        int serverId = loadBalancer.distributeRequest();
        std::cout << "Request " << i + 1 << " handled by Server " << serverId << std::endl;
    }

    return 0;
}

}// end of namespace