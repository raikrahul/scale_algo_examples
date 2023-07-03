#include <iostream>
#include <queue>
#include <fstream>
#include <functional>


namespace Persistent{
// Message structure
struct Message {
    std::string content;
};

// Message broker
class MessageBroker {
private:
    std::queue<Message> messageQueue;
    std::string persistenceFilePath;

public:
    MessageBroker(const std::string& persistenceFilePath)
        : persistenceFilePath(persistenceFilePath) {
        // Load persistent messages from disk
        loadPersistentMessages();
    }

    // Add message to the queue and persist it to disk
    void addMessage(const Message& message) {
        messageQueue.push(message);
        persistMessage(message);
    }

    // Retrieve next message from the queue
    Message getNextMessage() {
        Message message = messageQueue.front();
        messageQueue.pop();
        return message;
    }

    // Check if the queue is empty
    bool isQueueEmpty() {
        return messageQueue.empty();
    }

    // Load persistent messages from disk
    void loadPersistentMessages() {
        std::ifstream file(persistenceFilePath);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                messageQueue.push({ line });
            }
            file.close();
        }
    }

    // Persist message to disk
    void persistMessage(const Message& message) {
        std::ofstream file(persistenceFilePath, std::ios_base::app);
        if (file.is_open()) {
            file << message.content << std::endl;
            file.close();
        }
    }
};

// Producer class
class Producer {
private:
    MessageBroker& broker;

public:
    Producer(MessageBroker& broker) : broker(broker) {}

    // Send message to the broker's queue
    void sendMessage(const Message& message) {
        broker.addMessage(message);
        std::cout << "Message sent: " << message.content << std::endl;
    }
};

// Consumer class
class Consumer {
private:
    MessageBroker& broker;
    std::function<void(const Message&)> callback;

public:
    Consumer(MessageBroker& broker, const std::function<void(const Message&)>& callback)
        : broker(broker), callback(callback) {}

    // Retrieve message from the broker's queue and process it
    void retrieveMessage() {
        if (!broker.isQueueEmpty()) {
            Message message = broker.getNextMessage();
            callback(message);
        }
    }
};

// Example callback function for message processing
void processMessage(const Message& message) {
    std::cout << "Message received: " << message.content << std::endl;
}

int main() {
    // Create a message broker with persistence
    std::string persistenceFilePath = "messages.txt";
    MessageBroker broker(persistenceFilePath);

    // Create producers and consumers
    Producer producer1(broker);
    Producer producer2(broker);
    Consumer consumer1(broker, processMessage);
    Consumer consumer2(broker, processMessage);

    // Send messages from producers to the broker
    producer1.sendMessage({"Hello from producer 1!"});
    producer2.sendMessage({"Hello from producer 2!"});

    // Retrieve and process messages from the broker's queue
    consumer1.retrieveMessage();
    consumer2.retrieveMessage();

    return 0;
}


}// end of namespace
