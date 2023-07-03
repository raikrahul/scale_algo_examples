#include <amqp.h>
#include <amqp_framing.h>

// Producer
void producerProcess() {
  // Create a new AMQP connection and channel
  amqp_connection_state_t conn = amqp_new_connection();
  amqp_socket_t* socket = amqp_tcp_socket_new(conn);
  // Set up connection parameters
  const char* hostname = "localhost";
  int port = 5672;
  const char* username = "guest";
  const char* password = "guest";
  const char* exchangeName = "my_exchange";
  const char* routingKey = "France";
  
  // Connect to the broker
  amqp_socket_open(socket, hostname, port);
  amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, username, password);
  amqp_channel_open(conn, 1);
  amqp_get_rpc_reply(conn);

  // Declare the exchange
  amqp_exchange_declare(conn, 1, amqp_cstring_bytes(exchangeName), amqp_cstring_bytes("direct"),
                        0, 0, 0, 0, amqp_empty_table);
  amqp_get_rpc_reply(conn);

  // Publish a message to the exchange with the routing key
  const char* message = "Hello, France!";
  amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchangeName), amqp_cstring_bytes(routingKey),
                     0, 0, NULL, amqp_cstring_bytes(message));
  amqp_get_rpc_reply(conn);

  // Close the channel and connection
  amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
  amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
  amqp_destroy_connection(conn);
}

// Consumer
void consumerProcess() {
  // Create a new AMQP connection and channel
  amqp_connection_state_t conn = amqp_new_connection();
  amqp_socket_t* socket = amqp_tcp_socket_new(conn);
  // Set up connection parameters
  const char* hostname = "localhost";
  int port = 5672;
  const char* username = "guest";
  const char* password = "guest";
  const char* exchangeName = "my_exchange";
  const char* routingKey = "France";

  // Connect to the broker
  amqp_socket_open(socket, hostname, port);
  amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, username, password);
  amqp_channel_open(conn, 1);
  amqp_get_rpc_reply(conn);

  // Create an anonymous queue
  amqp_queue_declare_ok_t* queueDeclareOk = amqp_queue_declare(
      conn, 1, amqp_empty_bytes, 0, 1, 0, 0, amqp_empty_table);
  amqp_get_rpc_reply(conn);
  amqp_bytes_t queueName = amqp_bytes_malloc_dup(queueDeclareOk->queue);
  
  // Bind the queue to the exchange with the routing key
  amqp_queue_bind(conn, 1, queueName, amqp_cstring_bytes(exchangeName),
                  amqp_cstring_bytes(routingKey), amqp_empty_table);
  amqp_get_rpc_reply(conn);

  // Consume messages from the queue
  amqp_basic_consume(conn, 1, queueName, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
  amqp_get_rpc_reply(conn);

  // Process incoming messages
  while (1) {
    amqp_envelope_t envelope;
    amqp_maybe_release_buffers(conn);
    amqp_consume_message(conn, &envelope, NULL, 0);

    // Handle the received message
    if (envelope.message.body.len > 0) {
      printf("Received message: %.*s\n", (int)envelope.message.body.len, (char*)envelope.message.body.bytes);
    }

    amqp_destroy_envelope(&envelope);
  }

  // Close the channel and connection
  amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
  amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
  amqp_destroy_connection(conn);
}

