#ifndef PRIMEGEN_MANAGER_H
#define PRIMEGEN_MANAGER_H

#include "Connection.h"
#include "Block.h"

#include <rapidjson/document.h>

class Manager {
public:
    Manager(std::string target_address, unsigned port, unsigned threadCount = std::thread::hardware_concurrency()) :
        _connection(target_address, port) {

        _threadCount = threadCount;
        if (_threadCount <= 0) _threadCount = 1;
    }

    void run() {
        runOnceMultiThread();
    }

    void runOnceMultiThread() {
        std::vector<std::unique_ptr<std::thread>> tv;
        for (unsigned i = 0; i < _threadCount; i++) {
            tv.emplace_back(new std::thread([this]() {
                while(true) {
                    runOnceSingleThread();
                }
            }));
        }
        for (std::unique_ptr<std::thread>& t : tv) {
            (*t).join();
        }
    }

    void runOnceSingleThread() {
        rapidjson::Document document;
        document.Parse(_connection.requestBlock().c_str());

        std::string type = document["type"].GetString();
        if (type == "BLOCK_REQUEST_RESPONSE") {
            Block block(document["block_start"].GetString(), document["block_end"].GetString());
            const std::vector<std::string>& result = block.generateResult();
            _connection.blockResult(result, document["id"].GetString(), document["block_start"].GetString(), document["block_end"].GetString());
        }
    }

private:
    Connection _connection;
    unsigned _threadCount;
};

#endif //PRIMEGEN_MANAGER_H
