#include <iostream>
#include <vector>
#include <algorithm>

// Replica data structure
struct Replica {
    std::string data;
};

// Leaderless Database
class LeaderlessDatabase {
private:
    std::vector<Replica> replicas;

public:
    void writeData(const std::string& newData) {
        // Update all replicas
        for (auto& replica : replicas) {
            replica.data = newData;
        }
    }

    std::string readData() {
        // Select a replica to read from (random selection for simplicity)
        if (!replicas.empty()) {
            int randomIndex = rand() % replicas.size();
            return replicas[randomIndex].data;
        } else {
            return "";
        }
    }

    void addReplica(const Replica& replica) {
        replicas.push_back(replica);
    }
};

int main() {
    // Create a leaderless database
    LeaderlessDatabase database;

    // Add replicas
    Replica replica1, replica2, replica3;
    database.addReplica(replica1);
    database.addReplica(replica2);
    database.addReplica(replica3);

    // Write data to the database
    database.writeData("New Value");

    // Read data from the database
    std::cout << "Data: " << database.readData() << std::endl;

    return 0;
}
