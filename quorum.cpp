#include <SimpleAmqpClient/SimpleAmqpClient.h>

int main() {
    std::string hostname = "localhost";
    int port = 5672;
    std::string username = "guest";
    std::string password = "guest";
    std::string exchange = "";
    std::string routingKey = "my_quorum_queue";
    int numberOfReplicas = 3; // Number of replicas in the quorum queue
    int quorumSize = 3;       // Minimum number of replicas that need to persist a message

    try {
        AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create(hostname, port, username, password);
        channel->DeclareQueue("my_quorum_queue", true, false, false, true,
                              numberOfReplicas, quorumSize); // Declare the quorum queue

        // Publish a message to the quorum queue
        std::string message = "Hello, RabbitMQ!";
        channel->BasicPublish(exchange, routingKey, AmqpClient::BasicMessage::Create(message));

        // Consume messages from the quorum queue
        AmqpClient::Envelope::ptr_t envelope;
        channel->BasicConsume("my_quorum_queue", "", true, false, false);
        channel->BasicConsumeMessage("my_quorum_queue", envelope, 0);
        std::cout << "Received message: " << envelope->Message()->Body() << std::endl;
    } catch (const std::exception &e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
