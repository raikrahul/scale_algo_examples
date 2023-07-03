#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

// Define a message structure
struct Message {
    std::string topic;
    std::string content;
};

// Define a subscriber function type
using SubscriberFunc = std::function<void(const Message&)>;

// Define a broker class for message replication and partitioning
class MessageBroker {
public:
    MessageBroker(size_t numPartitions)
        : numPartitions(numPartitions), currentPartition(0) {
        initializePartitions();
    }

    // Publish an event to all interested subscribers
    void publish(const Message& message) {
        size_t partition = getPartition(message.topic);
        for (const auto& subscriber : subscribers[partition][message.topic]) {
            subscriber(message);
        }
        replicate(message); // Replicate the message to the follower broker
    }

    // Subscribe to a topic with a callback function
    void subscribe(const std::string& topic, const SubscriberFunc& callback) {
        size_t partition = getPartition(topic);
        subscribers[partition][topic].push_back(callback);
    }

    // Replicate a message to the follower broker
    void replicate(const Message& message) {
        // Code for replicating the message to the follower broker
        std::cout << "Message replicated: " << message.content << std::endl;
    }

private:
    size_t numPartitions;
    size_t currentPartition;
    std::unordered_map<size_t, std::unordered_map<std::string, std::vector<SubscriberFunc>>> subscribers;
    std::unordered_map<std::string, size_t> topicPartitions;

    void initializePartitions() {
        for (size_t i = 0; i < numPartitions; ++i) {
            partitions.push_back({});
        }
    }

    size_t getPartition(const std::string& topic) {
        if (topicPartitions.find(topic) != topicPartitions.end()) {
            return topicPartitions[topic];
        }
        else {
            size_t partition = currentPartition;
            currentPartition = (currentPartition + 1) % numPartitions;
            topicPartitions[topic] = partition;
            return partition;
        }
    }

    std::vector<std::unordered_map<std::string, std::vector<SubscriberFunc>>> partitions;
};

// Example usage
int main() {
    const size_t numPartitions = 3;
    MessageBroker broker(numPartitions);

    // Subscriber 1 (partition 0)
    broker.subscribe("topic1", [](const Message& message) {
        std::cout << "Subscriber 1 received message: " << message.content << std::endl;
    });

    // Subscriber 2 (partition 1)
    broker.subscribe("topic2", [](const Message& message) {
        std::cout << "Subscriber 2 received message: " << message.content << std::endl;
    });

    // Publish messages
    Message message1{ "topic1", "Hello, subscribers!" };
    broker.publish(message1);

    Message message2{ "topic2", "Greetings, everyone!" };
    broker.publish(message2);

    return 0;
}

