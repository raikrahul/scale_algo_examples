#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <thread>

// Base class for replicas using CRTP
template <typename Derived>
class Replica
{
public:
    void update(const std::string &key, const std::string &value)
    {
        static_cast<Derived *>(this)->updateImpl(key, value);
    }

    std::string read(const std::string &key)
    {
        return static_cast<Derived *>(this)->readImpl(key);
    }
};

// Concrete implementation of the replica interface
class FollowerReplica : public Replica<FollowerReplica>
{
private:
    std::unordered_map<std::string, std::string> data;
    static int numReplicas; // Static member variable

public:
    void updateImpl(const std::string &key, const std::string &value)
    {
        data[key] = value;
    }

    std::string readImpl(const std::string &key)
    {
        return data[key];
    }

    static int getNumReplicas() { return numReplicas; }
};

// Initialize the static member variable
int FollowerReplica::numReplicas = 0;

// Concrete implementation of the HackerReplica
class HackerReplica : public Replica<HackerReplica>
{
private:
    std::unordered_map<std::string, std::string> data;
    static int numReplicas; // Static member variable
    

public:
    void updateImpl(const std::string &key, const std::string &value)
    {
        // Store the data for ethical hacking analysis (simulated behavior)
        std::cout << "Storing data for ethical hacking analysis: "
                  << key << " = " << value << std::endl;
        // Delay the update to simulate longer processing time
        std::this_thread::sleep_for(std::chrono::seconds(5));
        // Update the data in the HackerReplica
        data[key] = value;
    }

    std::string readImpl(const std::string &key)
    {
        return data[key];
    }
     static int getNumReplicas() { return numReplicas; }
};
int HackerReplica::numReplicas = 0;


// Proxy class for the replica interface using CRTP
template <typename FollowerReplicaType, int W, int R>
class TunableConsistencyProxy : public Replica<TunableConsistencyProxy<FollowerReplicaType, W, R>>
{
private:
    FollowerReplicaType followerReplica;
    std::string lastUpdatedKey;
    std::string lastUpdatedValue;

public:
    void updateImpl(const std::string &key, const std::string &value)
    {
        followerReplica.update(key, value);
        lastUpdatedKey = key;
        lastUpdatedValue = value;
        // For simplicity, we assume synchronous updates to all replicas here.
        // In a real system, this would be asynchronous with a confirmation mechanism.
    }

    std::string readImpl(const std::string &key)
    {
        if (key == lastUpdatedKey)
        {
            return lastUpdatedValue;
        }
        else if (R == 1)
        {
            // Read from only one replica
            return followerReplica.read(key);
        }
        else if (R == FollowerReplicaType::getNumReplicas())
        {
            // Read from all replicas to get the latest value
            std::vector<std::string> values;
            for (int i = 0; i < FollowerReplicaType::getNumReplicas(); ++i)
            {
                values.push_back(followerReplica.read(key));
            }

            // Determine the latest value
            std::string latestValue = values[0];
            for (int i = 1; i < FollowerReplicaType::getNumReplicas(); ++i)
            {
                if (values[i] > latestValue)
                {
                    latestValue = values[i];
                }
            }

            return latestValue;
        }
        else
        {
            // Read from R replicas and determine the latest value
            // (In practice, you may use a quorum-based approach here for better fault tolerance)
            std::vector<std::string> values;
            for (int i = 0; i < R; ++i)
            {
                values.push_back(followerReplica.read(key));
            }

            // Determine the latest value
            std::string latestValue = values[0];
            for (int i = 1; i < R; ++i)
            {
                if (values[i] > latestValue)
                {
                    latestValue = values[i];
                }
            }

            return latestValue;
        }
    }
};

// Example usage
int main()
{
    // Using the tunable consistency proxy with W = 3 and R = 1
    using TunableProxy = TunableConsistencyProxy<FollowerReplica, 3, 1>;

    // Create an instance of the tunable consistency proxy
    TunableProxy proxy;

    // Update the data
    proxy.update("key1", "value1");
    proxy.update("key2", "value2");

    // Read the data
    std::cout << "Value of key1: " << proxy.read("key1") << std::endl; // Should return "value1"
    std::cout << "Value of key2: " << proxy.read("key2") << std::endl; // Should return "value2"

    // Create an instance of the HackerReplica
    // HackerReplica hackerReplica;

    using TunableProxyForHacking = TunableConsistencyProxy<HackerReplica, 1, 1>;
    TunableProxyForHacking hackerProxy;

    // Update the data in the HackerReplica
    hackerProxy.update("sensitive_key", "sensitive_value");

    // Read the data from the HackerReplica
    std::cout << "Value of sensitive_key from HackerReplica: "
              << hackerProxy.read("sensitive_key") << std::endl;

    return 0;
}
