#include <iostream>
#include <string>
#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

// DefaultPartitioner:

// Description: The DefaultPartitioner is an algorithm provided by the Kafka API for partitioning events.
// // Purpose: It determines the partition to which an event should be allocated.
// The algorithm uses hashing on the event key to compute a hash value, which is then used to choose the appropriate partition.
// This helps in directing events with the same key to the same partition, enabling consumers to process related events together

class DefaultPartitioner
{
public:
    int partition(const std::string &key, int numPartitions)
    {
        std::hash<std::string> hasher;
        std::size_t hashValue = hasher(key);
        return static_cast<int>(hashValue % numPartitions);
    }
};


// The rebalancing algorithm is used in Kafka to 
// redistribute partitions among consumers when there are changes in the consumer group, such as new consumers joining, 
// existing consumers leaving, or new partitions being added.
// Purpose: The algorithm ensures that all partitions are consumed and that each consumer is assigned one or more partitions. 
// It helps in maintaining load balancing and 
// fault tolerance within the consumer group by adjusting the 
// partition assignments based on the current group membership and partition distribution.




class Rebalancer {
public:
    void rebalance(const std::vector<std::string>& consumers, std::vector<std::string>& partitions) {
        std::sort(partitions.begin(), partitions.end());
        std::sort(consumers.begin(), consumers.end());

        std::unordered_map<std::string, std::vector<std::string>> assignedPartitions;
        std::vector<std::string> unassignedPartitions;

        // Find unassigned partitions
        std::set_difference(partitions.begin(), partitions.end(),
                            consumers.begin(), consumers.end(),
                            std::back_inserter(unassignedPartitions));

        // Calculate the number of partitions each consumer should have
        int partitionsPerConsumer = partitions.size() / consumers.size();
        int additionalPartitions = partitions.size() % consumers.size();

        // Assign partitions to consumers
        auto partitionIt = partitions.begin();
        for (const auto& consumer : consumers) {
            int numPartitions = partitionsPerConsumer + (additionalPartitions-- > 0 ? 1 : 0);

            assignedPartitions[consumer].insert(assignedPartitions[consumer].end(), partitionIt, partitionIt + numPartitions);
            partitionIt += numPartitions;
        }

        // Print assigned partitions for each consumer
        for (const auto& consumer : consumers) {
            std::cout << "Consumer: " << consumer << ", Partitions: ";
            for (const auto& partition : assignedPartitions[consumer]) {
                std::cout << partition << " ";
            }
            std::cout << std::endl;
        }

        // Print unassigned partitions
        if (!unassignedPartitions.empty()) {
            std::cout << "Unassigned partitions: ";
            for (const auto& partition : unassignedPartitions) {
                std::cout << partition << " ";
            }
            std::cout << std::endl;
        }
    }
};












int main()
{
    DefaultPartitioner partitioner;
    std::string eventKey = "example_key";
    int numPartitions = 8;

    int partition = partitioner.partition(eventKey, numPartitions);
    std::cout << "Event key: " << eventKey << std::endl;
    std::cout << "Partition: " << partition << std::endl;


    Rebalancer rebalancer;
    std::vector<std::string> consumers = {"Consumer1", "Consumer2", "Consumer3"};
    std::vector<std::string> partitions = {"Partition1", "Partition2", "Partition3", "Partition4", "Partition5"};

    rebalancer.rebalance(consumers, partitions);
    return 0;
}