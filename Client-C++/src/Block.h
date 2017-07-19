#ifndef PRIMEGEN_BLOCK_H
#define PRIMEGEN_BLOCK_H

#include <boost/multiprecision/cpp_int.hpp>
#include <vector>
#include <climits>

boost::multiprecision::cpp_int ZERO = 0;
boost::multiprecision::cpp_int TWO = 2;
boost::multiprecision::cpp_int MAX_INT = ULLONG_MAX;

class Block {
public:
    Block(const std::string& id, const std::string& blockStart, const std::string& blockEnd) {
        _blockStart.backend() = blockStart.c_str();
        _blockEnd.backend() = blockEnd.c_str();
        _id = id;
    }

    void generateResult() {
        if (_blockEnd.compare(MAX_INT) >= 0) {
            for (boost::multiprecision::cpp_int i = _blockStart;
                 i.compare(_blockEnd) <= 0;
                 i++) {

                if (isPrimeBig(i)) {
                    _result.push_back(i.convert_to<std::string>());
                }
            }
        } else {
            unsigned long long blockStart = _blockStart.convert_to<unsigned long long>();
            unsigned long long blockEnd = _blockEnd.convert_to<unsigned long long>();

            for (unsigned long long i = blockStart;
                 i <= blockEnd;
                 i++) {

                if (isPrime(i)) {
                    _result.push_back(std::to_string(i));
                }
            }
        }
    }

    const std::vector<std::string>& getResult() { return _result; }

    std::string getBlockStart() { return _blockStart.convert_to<std::string>(); }
    std::string getId() { return _id; }
    std::string getBlockEnd() { return _blockEnd.convert_to<std::string>(); }

private:
    bool isPrimeBig(boost::multiprecision::cpp_int p) {
        if (p % TWO == ZERO) return false;

        boost::multiprecision::cpp_int i = 3;
        boost::multiprecision::cpp_int p2 = p / TWO;
        while(i <= p2) {
            if (p % i == ZERO) return false;
            i += TWO;
        }

        return true;
    }

    bool isPrime(unsigned long long p) {
        if (p % 2 == 0) return false;

        unsigned long long i = 3;
        unsigned long long p2 = p / 2;
        while(i <= p2) {
            if (p % i == 0) return false;
            i += 2;
        }

        return true;
    }

    std::vector<std::string> _result;

    boost::multiprecision::cpp_int _blockStart;
    boost::multiprecision::cpp_int _blockEnd;
    std::string _id;
};

#endif //PRIMEGEN_BLOCK_H
