#include <iostream>
#include <vector>

// Replica data structure
struct Replica {
    std::string data;
};

// Leader-Follower Database
class LeaderFollowerDatabase {
private:
    Replica leader;
    std::vector<Replica> followers;

public:
    void writeData(const std::string& newData) {
        // Update the leader replica
        leader.data = newData;

        // Propagate updates to the followers
        for (auto& follower : followers) {
            follower.data = newData;
        }
    }

    std::string readData() {
        // Load balance read operations across followers
        if (!followers.empty()) {
            return followers[0].data;
        } else {
            return leader.data;
        }
    }

    void addFollower(const Replica& follower) {
        followers.push_back(follower);
    }
};

int main() {
    // Create a leader-follower database
    LeaderFollowerDatabase database;

    // Add followers
    Replica follower1, follower2;
    database.addFollower(follower1);
    database.addFollower(follower2);

    // Write data to the database
    database.writeData("New Value");

    // Read data from the database
    std::cout << "Data: " << database.readData() << std::endl;

    return 0;
}




