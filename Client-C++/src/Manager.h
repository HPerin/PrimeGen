#ifndef PRIMEGEN_MANAGER_H
#define PRIMEGEN_MANAGER_H

#include "Connection.h"
#include "Block.h"

#include <rapidjson/document.h>
#include <queue>

class Manager {
public:
    Manager(std::string target_address, unsigned port, unsigned threadCount = std::thread::hardware_concurrency()) :
        _connection(target_address, port) {

        _threadCount = threadCount;
        if (_threadCount <= 0) _threadCount = 1;
    }

    void run() {
        std::vector<std::unique_ptr<std::thread>> tv;
        for (unsigned i = 0; i < _threadCount; i++) {
            tv.emplace_back(new std::thread([this]() {
                while(true) {
                    std::shared_ptr<Block> block;
                    {
                        std::unique_lock<std::mutex> l(_queueMutex);
                        if (_inputQueue.size() > 0) {
                            block = _inputQueue.front();
                            _inputQueue.pop_front();
                        } else continue;
                    }

                    block->generateResult();
                    {
                        std::unique_lock<std::mutex> l(_queueMutex);
                        _outQueue.emplace_back(block);
                    }
                }
            }));
        }

        tv.emplace_back(new std::thread([this]() {
            while(true) {
                while (_outQueue.size() > 0) {
                    std::shared_ptr<Block> block;
                    {
                        std::unique_lock<std::mutex> l(_queueMutex);
                        block = _outQueue.front();
                        _outQueue.pop_front();
                    }

                    _connection.blockResult(block->getResult(), block->getId(), block->getBlockStart(),
                                            block->getBlockEnd());
                }
                usleep(1000);
            }
        }));

        while(true) {
            while (_inputQueue.size() < (_threadCount * 20)) createBlock();
            usleep(1);
        }

        for (std::unique_ptr<std::thread>& t : tv) {
            (*t).join();
        }
    }

//    void runOnceMultiThread() {
//        std::vector<std::unique_ptr<std::thread>> tv;
//        for (unsigned i = 0; i < _threadCount; i++) {
//            tv.emplace_back(new std::thread([this]() {
//                while(true) {
//                    runOnceSingleThread();
//                }
//            }));
//        }
//        for (std::unique_ptr<std::thread>& t : tv) {
//            (*t).join();
//        }
//    }

    void createBlock() {
        rapidjson::Document document;
        document.Parse(_connection.requestBlock().c_str());

        std::string type = document["type"].GetString();
        if (type == "BLOCK_REQUEST_RESPONSE") {
            std::unique_lock<std::mutex> l(_queueMutex);
            _inputQueue.emplace_back(std::make_shared<Block>(document["id"].GetString(), document["block_start"].GetString(), document["block_end"].GetString()));
        }
    }

//    void runOnceSingleThread() {
//        rapidjson::Document document;
//        document.Parse(_connection.requestBlock().c_str());
//
//        std::string type = document["type"].GetString();
//        if (type == "BLOCK_REQUEST_RESPONSE") {
//            Block block(document["block_start"].GetString(), document["block_end"].GetString());
//            const std::vector<std::string>& result = block.generateResult();
//            _connection.blockResult(result, document["id"].GetString(), document["block_start"].GetString(), document["block_end"].GetString());
//        }
//    }

private:
    Connection _connection;
    unsigned _threadCount;
    std::deque<std::shared_ptr<Block>> _inputQueue;
    std::deque<std::shared_ptr<Block>> _outQueue;
    std::mutex _queueMutex;
};

#endif //PRIMEGEN_MANAGER_H
