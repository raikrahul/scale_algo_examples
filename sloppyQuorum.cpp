#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

// Base class for replicas using CRTP
template <typename Derived, int numReplicas>
class Replica {
public:
    void update(const std::string& key, const std::string& value) {
        static_cast<Derived*>(this)->updateImpl(key, value);
    }

    std::string read(const std::string& key) {
        return static_cast<Derived*>(this)->readImpl(key);
    }

    int getNumReplicas() const { return numReplicas; }
};

// Concrete implementation of the replica interface
class FollowerReplica : public Replica<FollowerReplica, 1> {
private:
    std::unordered_map<std::string, std::string> data;

public:
    void updateImpl(const std::string& key, const std::string& value) {
        data[key] = value;
    }

    std::string readImpl(const std::string& key) {
        return data[key];
    }

    // int getNumReplicas() const { return data.size(); }
};

// Proxy class for the replica interface using CRTP
// template <typename FollowerReplicaType, int W, int R, int numReplicas = FollowerReplica::getNumReplicas() >
// // class TunableConsistencyProxy : public Replica<TunableConsistencyProxy<FollowerReplicaType, W, R>, FollowerReplicaType::getNumReplicas() > {
// class TunableConsistencyProxy : public Replica<TunableConsistencyProxy<FollowerReplicaType, W, R>, FollowerReplicaType::getNumReplicas()> {
template <typename FollowerReplicaType, int W, int R, int NumReplicas>
class TunableConsistencyProxy : public Replica<TunableConsistencyProxy<FollowerReplicaType, W, R, NumReplicas>, NumReplicas> {
private:
    FollowerReplicaType followerReplica;
    std::string lastUpdatedKey;
    std::string lastUpdatedValue;

public:
    void updateImpl(const std::string& key, const std::string& value) {
        followerReplica.update(key, value);
        lastUpdatedKey = key;
        lastUpdatedValue = value;
        // For simplicity, we assume synchronous updates to all replicas here.
        // In a real system, this would be asynchronous with a confirmation mechanism.
    }

    std::string readImpl(const std::string& key) {
        if (key == lastUpdatedKey) {
            return lastUpdatedValue;
        } else if (R == 1) {
            // Read from only one replica
            return followerReplica.read(key);
        } else if (R == NumReplicas) {
            // Read from all replicas to get the latest value
            std::vector<std::string> values;
            for (int i = 0; i < NumReplicas; ++i) {
                values.push_back(followerReplica.read(key));
            }

            // Determine the latest value
            std::string latestValue = values[0];
            for (int i = 1; i < NumReplicas; ++i) {
                if (values[i] > latestValue) {
                    latestValue = values[i];
                }
            }

            return latestValue;
        } else {
            // Read from R replicas and determine the latest value
            // (In practice, you may use a quorum-based approach here for better fault tolerance)
            std::vector<std::string> values;
            for (int i = 0; i < R; ++i) {
                values.push_back(followerReplica.read(key));
            }

            // Determine the latest value
            std::string latestValue = values[0];
            for (int i = 1; i < R; ++i) {
                if (values[i] > latestValue) {
                    latestValue = values[i];
                }
            }

            return latestValue;
        }
    }
};

// Proxy class for the replica interface with sloppy quorums and hinted handoff
template <typename FollowerReplicaType, int W, int R>
class SloppyQuorumProxy : public Replica<SloppyQuorumProxy<FollowerReplicaType, W, R>,3 > {
private:
    FollowerReplicaType followerReplica;
    std::unordered_map<std::string, std::string> hintedUpdates;

public:
    void updateImpl(const std::string& key, const std::string& value) {
        if (followerReplica.getNumReplicas() >= W) {
            followerReplica.update(key, value);
        } else {
            hintedUpdates[key] = value;
        }
    }

    std::string readImpl(const std::string& key) {
        std::string value = followerReplica.read(key);

        if (hintedUpdates.find(key) != hintedUpdates.end()) {
            value = hintedUpdates[key];
        }

        return value;
    }
};

int main() {
    // Create and test the proxies
    TunableConsistencyProxy<FollowerReplica, 2, 2,3 > consistencyProxy;
    SloppyQuorumProxy<FollowerReplica, 2, 2> sloppyQuorumProxy;

    consistencyProxy.update("K", "v1");
    sloppyQuorumProxy.update("K", "v1");

    consistencyProxy.update("K", "v2");

    std::cout << "Consistency Proxy - Read K: " << consistencyProxy.read("K") << std::endl;
    std::cout << "Sloppy Quorum Proxy - Read K: " << sloppyQuorumProxy.read("K") << std::endl;

    return 0;
}
