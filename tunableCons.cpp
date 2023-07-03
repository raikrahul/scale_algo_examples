#include <iostream>
#include <unordered_map>
#include <vector>

class Database {
private:
    std::unordered_map<std::string, std::string> data;
    std::vector<std::string> replicas;

public:
    Database(const std::vector<std::string>& replicaList) : replicas(replicaList) {}

    void update(const std::string& key, const std::string& value, int numReplicasToUpdate) {
        // Simulate updating the specified number of replicas
        for (int i = 0; i < numReplicasToUpdate; i++) {
            data[replicas[i] + key] = value;
        }
    }

    std::string read(const std::string& key, int numReplicasToRead) {
        // Find the latest value by reading from the specified number of replicas
        std::string latestValue = "";
        for (int i = 0; i < numReplicasToRead; i++) {
            std::string replicaKey = replicas[i] + key;
            if (data.count(replicaKey) > 0) {
                latestValue = data[replicaKey];
            }
        }
        return latestValue;
    }
};

int main() {
    std::vector<std::string> replicaList = {"Replica1_", "Replica2_", "Replica3_"};
    Database database(replicaList);

    // Set tunable consistency parameters
    int N = replicaList.size();  // Total number of replicas
    int W = 3;                   // Number of replicas to update
    int R = 1;                   // Number of replicas to read

    // Perform a write operation
    database.update("credit_card", "new_card_details", W);

    // Perform a read operation
    std::string creditCardInfo = database.read("credit_card", R);

    // Display the result
    std::cout << "Credit card information: " << creditCardInfo << std::endl;

    return 0;
}
