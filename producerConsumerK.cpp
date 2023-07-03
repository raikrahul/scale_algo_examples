#include <cppkafka/cppkafka.h>
#include <iostream>

class ProducerCallback : public cppkafka::DeliveryReportCb
{
public:
    void dr_cb(cppkafka::Message &message) override
    {
        if (message.get_error())
        {
            // Handle delivery failure
            std::cout << "Failed to deliver message: " << message.get_error().to_string() << std::endl;
        }
        else
        {
            // Handle successful delivery
            std::cout << "Message delivered: " << message.get_payload() << std::endl;
        }
    }
};



// Consumer callback class
class ConsumerCallback : public cppkafka::Consumer::RebalanceCb {
public:
    void on_rebalance_start(const cppkafka::TopicPartitionList& topic_partitions) override {
        // Handle rebalance start event
        std::cout << "Rebalance started" << std::endl;
    }

    void on_rebalance_assigned(const cppkafka::TopicPartitionList& topic_partitions) override {
        // Handle partition assignment event
        std::cout << "Partition assignment:" << std::endl;
        for (const auto& topic_partition : topic_partitions) {
            std::cout << "Topic: " << topic_partition.get_topic() << ", Partition: " << topic_partition.get_partition() << std::endl;
        }
    }
};
// Producer function to publish events asynchronously
void produceEventsAsync(const std::string &topic, const std::string &brokers)
{
    cppkafka::Configuration config = {
        {"metadata.broker.list", brokers},
        {"queue.buffering.max.messages", "100000"}, // Maximum number of messages in the local buffer
        {"queue.buffering.max.ms", "5"}             // Maximum time to wait for a batch in milliseconds
    };
    cppkafka::Producer producer(config);

    // Set the delivery report callback
    ProducerCallback delivery_report_cb;
    producer.set_delivery_report_callback(&delivery_report_cb);

    std::string key, value;
    // Your logic to generate key and value for each event
    // ...

    // Create a message builder with the topic and payload
    cppkafka::MessageBuilder builder(topic)
        .key(key)
        .payload(value);

    // Produce the event asynchronously
    producer.produce(builder);

    // Wait for the delivery report callback to be triggered
    producer.flush();
}

// Producer function to publish events

void produceEvents(const std::string &topic, const std::string &brokers)
{
   

cppkafka::Configuration config = {
        { "metadata.broker.list", brokers },
        { "queue.buffering.max.messages", "100000" },
        { "queue.buffering.max.ms", "5" },
        { "acks", "1" },                     // Acknowledge events after they are persisted to the topic
        { "enable.idempotence", "true" }     // Enable exactly-once delivery semantics
    };







    cppkafka::Producer producer(config);


    ProducerCallback delivery_report_cb;
    producer.set_delivery_report_callback(&delivery_report_cb);


    std::string key, value;
    // Your logic to generate key, value, and timestamp for each lift ride event
    // ...

    cppkafka::MessageBuilder builder(topic)
        .partition(0) // Specify the partition number
        .key(key)
        .payload(value);

    // Publish the event
    producer.produce(builder);
    producer.flush(); // Wait for the event to be sent
}

// Consumer function to read events from a topic
void consumeEvents(const std::string &topic, const std::string &brokers)
{
    cppkafka::Configuration config = {
        {"metadata.broker.list", brokers},
        {"group.id", "my-group"} // Specify a consumer group ID
    };
    cppkafka::Consumer consumer(config);

    // Subscribe to the topic
    consumer.subscribe({topic});

    while (true) {
        cppkafka::Message message = consumer.poll();
        if (message) {
            if (!message.get_error()) {
                // Process the received event
                std::cout << "Received event: " << message.get_payload() << std::endl;

                // Manually commit the offset
                consumer.commit(message);
            }
            else {
                // Handle error
                std::cout << "Error: " << message.get_error().to_string() << std::endl;
            }
        }
    }
}

int main()
{
    // Specify the Kafka topic and broker list
    std::string topic = "lift-ride-events";
    std::string brokers = "localhost:9092";

    // Produce events
    produceEvents(topic, brokers);

    // Consume events
    consumeEvents(topic, brokers);

    return 0;
}

// g++ -std=c++17 -o myprogram myprogram.cpp -lcppkafka -lrdkafka++