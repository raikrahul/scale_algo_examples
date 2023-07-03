#include <iostream>
#include "MessageQueue.h"

using namespace std;

int main() {
    // Create a message broker
    MessageBroker broker;

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
