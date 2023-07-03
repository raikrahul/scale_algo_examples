#include <iostream>
#include <vector>
#include <algorithm>

class LogicalClock {
public:
    LogicalClock() : value(0) {}

    void increment() {
        value++;
    }

    void setClock(int clock) {
        value = std::max(value, clock) + 1;
    }

    int getClock() const {
        return value;
    }

private:
    int value;
};

class VersionVector {
public:
    VersionVector(int numReplicas) : vectors(numReplicas, 0) {}

    void updateClock(int replicaId, int clock) {
        vectors[replicaId] = std::max(vectors[replicaId], clock);
    }

    bool checkConflicts(const VersionVector& other) const {
        for (size_t i = 0; i < vectors.size(); ++i) {
            if (vectors[i] < other.vectors[i]) {
                return true;
            }
        }
        return false;
    }

private:
    std::vector<int> vectors;
};

class DatabaseObject {
public:
    DatabaseObject(const std::string& data, int version = 1) : data(data), version(version) {}

    void updateVersion(int newVersion) {
        version = newVersion;
    }

    int getVersion() const {
        return version;
    }

private:
    std::string data;
    int version;
};

class DatabaseObjectBuilder {
public:
    DatabaseObjectBuilder& setData(const std::string& data) {
        object.data = data;
        return *this;
    }

    DatabaseObjectBuilder& setVersion(int version) {
        object.updateVersion(version);
        return *this;
    }

    DatabaseObject build() const {
        return object;
    }

private:
    DatabaseObject object;
};

class Replica {
public:
    Replica(int id, int numReplicas) : replicaId(id), versionVector(numReplicas) {}

    void processUpdate(DatabaseObject updatedObject, const VersionVector& receivedVersionVector) {
        int newVersion = updatedObject.getVersion();
        versionVector.updateClock(replicaId, newVersion);
        if (!versionVector.checkConflicts(receivedVersionVector)) {
            databaseObject = updatedObject;
            std::cout << "Replica " << replicaId << " updated successfully. Version: " << newVersion << std::endl;
        } else {
            std::cout << "Conflict detected in Replica " << replicaId << ". Update rejected." << std::endl;
        }
    }

private:
    int replicaId;
    DatabaseObject databaseObject;
    VersionVector versionVector;
};

int main() {
    const int numReplicas = 3;
    std::vector<Replica> replicas;

    // Initialize replicas
    for (int i = 0; i < numReplicas; ++i) {
        replicas.emplace_back(i, numReplicas);
    }

    // Simulate concurrent updates at replicas 2 and 3
    DatabaseObjectBuilder builder;
    builder.setData("John Doe");
    
    // Replica 2 receives an update
    DatabaseObject obj1 = builder.setVersion(1).build();
    replicas[2].processUpdate(obj1, VersionVector(numReplicas));

    // Replica 3 receives an update
    DatabaseObject obj2 = builder.setVersion(1).build();
    replicas[3].processUpdate(obj2, VersionVector(numReplicas));

    // Replica 2 and Replica 3 exchange version vectors
    VersionVector vector2 = replicas[2].getVersionVector();
    VersionVector vector3 = replicas[3].getVersionVector();

    // Replica 2 checks for conflicts
    if (vector2.checkConflicts(vector3)) {
        std::cout << "Conflict detected between Replica 2 and Replica 3." << std::endl;
    } else {
        std::cout << "No conflicts between Replica 2 and Replica 3." << std::endl;
    }

    return 0;
}
