#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <string_view>
#include <algorithm>
using namespace std::literals::string_literals;
std::string IP_ADDRESS = "127.0.0.1"s;
unsigned int PORT_NUMBER = 8001;
constexpr unsigned int MAX_MSG_LENGTH = 1024;
std::size_t read_complete(std::string_view msg, boost::system::error_code const& err, std::size_t bytes) {
	if (err) return 0;
	bool found = std::find(std::begin(msg), std::end(msg), '\n') < std::begin(msg) + bytes;
	//we read one by one until we get to enter, no buffering.
	return found ? 0 : 1;
}
int main() {
	boost::asio::io_service service;
	boost::asio::ip::tcp::socket sock(service);
	std::cout << "connecting...\n";
	boost::system::error_code err;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(IP_ADDRESS), PORT_NUMBER);
	//try to connect to the server side...and catch any error that might occur
	sock.connect(endpoint, err);
	if (err) {
		std::cout << "ERROR: " << err.message();
	}
	else {
		//sock.open();
		std::cout << "You are connected and chatting as client...";
		std::string outgoing_message_buffer;
		char incoming_message_buffer[MAX_MSG_LENGTH];
		while (sock.is_open()) {
			std::cout << "\n\n                         Client: ";
			getline(std::cin, outgoing_message_buffer);
			outgoing_message_buffer += "\n";
			sock.write_some(boost::asio::buffer(outgoing_message_buffer));
			std::cout << "\n\nServer: ";
			auto bytes = boost::asio::read(sock, boost::asio::buffer(incoming_message_buffer), std::bind(read_complete, incoming_message_buffer
				, std::placeholders::_1, std::placeholders::_2));
			std::string msg_received(incoming_message_buffer, incoming_message_buffer + bytes);
			std::cout << msg_received;
			std::cin.clear();
		}
	}
}