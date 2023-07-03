#include <iostream>
#include <string>
#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>

int main() {
    // Establish connection and channel
    AMQP::TcpConnection connection(AMQP::Address("amqp://guest:guest@localhost/"));
    AMQP::TcpChannel channel(&connection);

    // Enable publisher confirms
    channel.confirmSelect();

    // Declare exchange and queue
    std::string exchangeName = "my_exchange";
    std::string queueName = "my_queue";
    std::string routingKey = "my_routing_key";

    channel.declareExchange(exchangeName, AMQP::direct);
    channel.declareQueue(queueName);
    channel.bindQueue(exchangeName, queueName, routingKey);

    // Set queue as persistent
    channel.setQueueArgument(queueName, "x-message-ttl", AMQP::TableValue(60000)); // Set TTL to 60 seconds

    // Publish a persistent message
    std::string message = "Hello, World!";
    AMQP::Envelope envelope(message);
    envelope.setDeliveryMode(2); // Persistent message
    channel.publish(exchangeName, routingKey, envelope);

    // Enable manual consumer acknowledgments
    channel.setQos(0, 1, false); // Fetch only one message at a time
    channel.consume(queueName, AMQP::noack); // Disable automatic acknowledgments

    // Consume messages
    std::string consumerTag = channel.consumeMessage(queueName, AMQP::noack);
    while (true) {
        AMQP::Message consumedMessage;
        if (channel.consumeMessage(consumerTag, consumedMessage)) {
            std::string receivedMessage = consumedMessage.message();
            std::cout << "Received message: " << receivedMessage << std::endl;

            // Acknowledge message manually
            channel.ack(consumedMessage);
        }
    }

    return 0;
}
