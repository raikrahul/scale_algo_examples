#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class VersionVector {
public:
    VersionVector(const std::vector<std::pair<std::string, int>>& replicas) : replicas(replicas) {}

    void updateClock(const std::string& replica) {
        auto it = std::find_if(replicas.begin(), replicas.end(),
                               [&](const std::pair<std::string, int>& pair) {
                                   return pair.first == replica;
                               });

        if (it != replicas.end()) {
            it->second++;
        }
    }

    bool isConcurrent(const VersionVector& other) const {
        for (const auto& pair : replicas) {
            auto it = std::find_if(other.replicas.begin(), other.replicas.end(),
                                   [&](const std::pair<std::string, int>& p) {
                                       return p.first == pair.first;
                                   });

            if (it != other.replicas.end() && it->second > pair.second) {
                return true;
            }
        }

        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const VersionVector& vector) {
        for (const auto& pair : vector.replicas) {
            os << "[" << pair.first << "," << pair.second << "] ";
        }
        return os;
    }

private:
    std::vector<std::pair<std::string, int>> replicas;
};

int main() {
    std::vector<std::pair<std::string, int>> replicas = {{"r1", 0}, {"r2", 0}, {"r3", 0}};

    VersionVector v1(replicas);
    VersionVector v2(replicas);
    VersionVector v3(replicas);

    // Update v2 and v3 concurrently
    v2.updateClock("r2");
    v3.updateClock("r3");

    std::cout << "v2: " << v2 << std::endl;
    std::cout << "v3: " << v3 << std::endl;

    // Check for concurrency
    if (v2.isConcurrent(v3)) {
        std::cout << "Concurrent updates detected!" << std::endl;
    } else {
        std::cout << "No concurrency detected." << std::endl;
    }

    return 0;
}
