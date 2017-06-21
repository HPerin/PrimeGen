#ifndef PRIMEGEN_CONNECTION_H
#define PRIMEGEN_CONNECTION_H

#include <boost/asio.hpp>
#include <thread>
#include <mutex>

class Connection {
public:
    Connection() :
            _socket(_ioService) {

        boost::asio::ip::tcp::resolver resolver(_ioService);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), "127.0.0.1", "3000");
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

        _socket.connect(*iterator);
    }

    std::string requestSnapshot(const std::string& since) {
        std::unique_lock<std::mutex> l(_mutex);

        std::string outMessage = "{\"type\":\"SNAPSHOT_REQUEST\",\"since\":\"" + since + "\"}$";
        _socket.write_some(boost::asio::buffer(outMessage.data(), outMessage.size()));

        boost::asio::streambuf messageStream;
        boost::asio::read_until(_socket, messageStream, "$");

        std::string message{
                boost::asio::buffers_begin(messageStream.data()),
                boost::asio::buffers_begin(messageStream.data()) + messageStream.size() - 1
        };

        return message;
    }

    std::string requestBlock() {
        std::unique_lock<std::mutex> l(_mutex);

        std::string outMessage = "{\"type\":\"BLOCK_REQUEST\"}$";
        _socket.write_some(boost::asio::buffer(outMessage.data(), outMessage.size()));

        boost::asio::streambuf messageStream;
        boost::asio::read_until(_socket, messageStream, "$");

        std::string message{
                boost::asio::buffers_begin(messageStream.data()),
                boost::asio::buffers_begin(messageStream.data()) + messageStream.size() - 1
        };

        return message;
    }

    std::string blockResult(const std::vector<std::string>& result, std::string id, std::string blockStart, std::string blockEnd) {
        std::unique_lock<std::mutex> l(_mutex);
        
        std::stringstream outMessage;
        outMessage << "{\"type\":\"BLOCK_RESULT\",\"id\":\"" << id << "\",\"block_start\":\"" << blockStart << "\",\"block_end\":\"" << blockEnd << "\",\"data\":[";
        for (size_t i = 0; i < result.size(); i++) {
            if (i != 0) outMessage << ",";
            outMessage << "\"" << result[i] << "\"";
        }
        outMessage << "]}$";
        _socket.write_some(boost::asio::buffer(outMessage.str().data(), outMessage.str().size()));

        boost::asio::streambuf messageStream;
        boost::asio::read_until(_socket, messageStream, "$");

        std::string message{
                boost::asio::buffers_begin(messageStream.data()),
                boost::asio::buffers_begin(messageStream.data()) + messageStream.size() - 1
        };

        return message;
    }

private:
    boost::asio::io_service _ioService;
    boost::asio::ip::tcp::socket _socket;
    std::mutex _mutex;
};

#endif //PRIMEGEN_CONNECTION_H
