
#include <iostream>
#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace design_services_demo
{

    namespace http = boost::beast::http;

    void handleRequest(http::request<http::string_body> &request, http::response<http::string_body> &response)
    {
        // Extract skierID from the request URI
        std::string skierID = request.target().to_string().substr(8);

        if (skierID.empty())
        {
            // Request was poorly formatted, return error code
            response.result(http::status::bad_request);
        }
        else
        {
            // Read the skier profile from the database
            std::string skierProfile = getSkierProfile(skierID); // Assuming getSkierProfile function retrieves the profile

            // Add skier profile as JSON to HTTP response and return 200
            response.result(http::status::ok);
            response.set(http::field::content_type, "application/json");
            response.body() = skierProfile;
        }
    }

    int main()
    {
        boost::asio::io_context io;

        boost::asio::ip::tcp::acceptor acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));

        while (true)
        {
            boost::asio::ip::tcp::socket socket(io);
            acceptor.accept(socket);

            boost::beast::flat_buffer buffer;
            http::request<http::string_body> request;
            http::response<http::string_body> response;

            // Read the HTTP request
            http::read(socket, buffer, request);

            // Process the request and generate the response
            handleRequest(request, response);

            // Send the HTTP response
            http::write(socket, response);
        }

        return 0;
    }

}