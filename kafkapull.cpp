#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <csignal>

#include <librdkafka/rdkafkacpp.h>

static bool running = true;

// Kafka configuration
const std::string brokers = "localhost:9092";
const std::string topic = "test-topic";
const std::string group_id = "test-group";
const std::string offset_reset = "earliest";

// Signal handler for graceful termination
static void sigterm(int sig) {
    running = false;
}

// Kafka message delivery report callback
static void delivery_report(RdKafka::Message& message) {
    if (message.err()) {
        std::cerr << "Message delivery failed: " << message.errstr() << std::endl;
    } else {
        std::cout << "Message delivered to topic: " << message.topic_name() << ", partition: " << message.partition() << ", offset: " << message.offset() << std::endl;
    }
}

int main() {
    std::signal(SIGINT, sigterm);
    std::signal(SIGTERM, sigterm);

    std::string errstr;
    std::string err;

    // Kafka configuration
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    conf->set("bootstrap.servers", brokers, err);
    conf->set("group.id", group_id, err);
    conf->set("auto.offset.reset", offset_reset, err);

    // Create Kafka consumer
    RdKafka::Consumer* consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        delete conf;
        return 1;
    }

    // Subscribe to topic
    std::vector<std::string> topics = { topic };
    RdKafka::ErrorCode errCode = consumer->subscribe(topics);
    if (errCode != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to subscribe to topic: " << RdKafka::err2str(errCode) << std::endl;
        delete consumer;
        delete conf;
        return 1;
    }

    // Main event loop
    while (running) {
        // Poll for events
        RdKafka::Message* message = consumer->consume(1000); // Timeout: 1 second

        // Process the fetched message
        if (message) {
            if (message->err() == RdKafka::ERR_NO_ERROR) {
                // Message successfully fetched
                std::cout << "Received message: " << static_cast<const char*>(message->payload()) << std::endl;
            } else {
                // Error or end of partition reached
                std::cerr << "Error while consuming message: " << message->errstr() << std::endl;
            }

            // Commit the offset of the processed message
            consumer->commitSync(*message);

            delete message;
        }
    }

    // Unsubscribe from the topic
    consumer->unsubscribe();

    // Close the consumer
    delete consumer;

    // Destroy the global configuration
    delete conf;

    return 0;
}
