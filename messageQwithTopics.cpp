#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <vector>
#include <set>

class Message {
public:
    std::string topic;
    std::string data;

    Message(const std::string& topic, const std::string& data) : topic(topic), data(data) {}
};

class TopicQueue {
private:
    std::queue<Message> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;

public:
    void push(const Message& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(message);
        cv_.notify_one();  // Notify a waiting consumer
    }

    Message pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !queue_.empty(); });  // Wait until queue is not empty

        Message message = queue_.front();
        queue_.pop();
        return message;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

class MessageBroker {
private:
    std::unordered_map<std::string, std::vector<TopicQueue>> topics_;
    std::unordered_map<std::string, std::set<int>> isrList_;

public:
    void createTopic(const std::string& topic, int replicationFactor) {
        if (topics_.count(topic) > 0) {
            throw std::runtime_error("Topic already exists: " + topic);
        }

        std::vector<TopicQueue> queues(replicationFactor);
        topics_.emplace(topic, queues);
        isrList_.emplace(topic, std::set<int>());
    }

    void publish(const std::string& topic, const std::string& data) {
        if (topics_.count(topic) == 0) {
            throw std::runtime_error("Topic not found: " + topic);
        }

        std::vector<TopicQueue>& topicQueues = topics_.at(topic);
        std::set<int>& isr = isrList_.at(topic);

        for (int i = 0; i < topicQueues.size(); i++) {
            if (isr.count(i) > 0) {  // Check if the replica is in the ISR
                Message message(topic, data);
                topicQueues[i].push(message);
            }
        }
    }

    Message consume(const std::string& topic) {
        if (topics_.count(topic) == 0) {
            throw std::runtime_error("Topic not found: " + topic);
        }

        std::vector<TopicQueue>& topicQueues = topics_.at(topic);
        std::set<int>& isr = isrList_.at(topic);

        for (int i = 0; i < topicQueues.size(); i++) {
            if (isr.count(i) > 0 && !topicQueues[i].empty()) {  // Check if the replica is in the ISR and has messages
                return topicQueues[i].pop();
            }
        }

        throw std::runtime_error("No messages available for topic: " + topic);
    }

    void addReplicaToISR(const std::string& topic, int replicaId) {
        if (topics_.count(topic) == 0) {
            throw std::runtime_error("Topic not found: " + topic);
        }

        std::set<int>& isr = isrList_.at(topic);
        isr.insert(replicaId);
    }

    void removeReplicaFromISR(const std::string& topic, int replicaId) {
        if (topics_.count(topic) == 0) {
            throw std::runtime_error("Topic not found: " + topic);
        }

        std::set<int>& isr = isrList_.at(topic);
        isr.erase(replicaId);
    }
};

int main() {
    MessageBroker broker;

    // Create topics with replication factor
    broker.createTopic("topic1", 3);
    broker.createTopic("topic2", 2);

    // Add replicas to ISR
    broker.addReplicaToISR("topic1", 0);
    broker.addReplicaToISR("topic1", 1);
    broker.addReplicaToISR("topic1", 2);
    broker.addReplicaToISR("topic2", 0);
    broker.addReplicaToISR("topic2", 1);

    // Publish messages to different topics
    broker.publish("topic1", "Message 1 for topic 1");
    broker.publish("topic2", "Message 1 for topic 2");
    broker.publish("topic1", "Message 2 for topic 1");

    // Consume messages from different topics
    try {
        Message message1 = broker.consume("topic1");
        std::cout << "Topic 1: " << message1.data << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error consuming message from topic1: " << e.what() << std::endl;
    }

    try {
        Message message2 = broker.consume("topic2");
        std::cout << "Topic 2: " << message2.data << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error consuming message from topic2: " << e.what() << std::endl;
    }

    try {
        Message message3 = broker.consume("topic1");
        std::cout << "Topic 1: " << message3.data << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error consuming message from topic1: " << e.what() << std::endl;
    }

    return 0;
}
