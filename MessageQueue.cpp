#include "MessageQueue.h"

MessageQueue::MessageQueue()
{
    //ctor
}

MessageQueue::~MessageQueue()
{
    //dtor
}


#include <iostream>
#include <queue>
#include <functional>

// Message structure
struct Message {
    std::string content;
};

// Message broker
class MessageBroker {
private:
    std::queue<Message> messageQueue;

public:
    // Add message to the queue
    void addMessage(const Message& message) {
        messageQueue.push(message);
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

