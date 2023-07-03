
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
namespace pubsub {
// Define a message structure

// Define a message structure
struct Message {
  std::string topic;
  std::string content;
};

// Define a subscriber function type
using SubscriberFunc = std::function<void(const Message &)>;

class MessageBroker {
public:
  MessageBroker(size_t numPartitions) : partitions(numPartitions) {}

  // Publish an event to all interested subscribers
  void publish(const Message& message) {
  std::lock_guard<std::mutex> lock(partitionsMutex);
  size_t partition = hashTopic(message.topic) % partitions.size();
  
  if (subscribers.find(partition) != subscribers.end()) {
    if (subscribers[partition].find(message.topic) != subscribers[partition].end()) {
      for (const auto& subscriber : subscribers[partition][message.topic]) {
        subscriber(message);
      }
    }
  }
}

  // Subscribe to a topic with a callback function
  void subscribe(const std::string &topic, const SubscriberFunc &callback) {
    std::lock_guard<std::mutex> lock(partitionsMutex);
    size_t partition = hashTopic(topic) % partitions.size();

    size_t part = hashTopic(topic) % partitions.size();

    subscribers[part][topic].push_back(callback);
  }

private:
  std::unordered_map<
      size_t, std::unordered_map<std::string, std::vector<SubscriberFunc>>>
      subscribers;
  std::unordered_map<std::string, size_t> partitions;
  std::mutex partitionsMutex;

  size_t hashTopic(const std::string &topic) {
    std::hash<std::string> hasher;
    return hasher(topic);
  }
};

// Example usage
int main() {
  const size_t numPartitions = 3;
  MessageBroker broker(numPartitions);

  // Subscriber 1
  broker.subscribe("topic1", [](const Message &message) {
    std::cout << "Subscriber 1 received message: " << message.content
              << std::endl;
  });

  // Subscriber 2
  broker.subscribe("topic2", [](const Message &message) {
    std::cout << "Subscriber 2 received message: " << message.content
              << std::endl;
  });

  // Publish messages
  Message message1{"topic1", "Hello, subscribers!"};
  broker.publish(message1);

  Message message2{"topic2", "Greetings, everyone!"};
  broker.publish(message2);

  return 0;
}

} // namespace pubsub