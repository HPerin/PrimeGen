#ifndef PRIMEGEN_BLOCK_H
#define PRIMEGEN_BLOCK_H

#include <boost/multiprecision/cpp_int.hpp>
#include <vector>

boost::multiprecision::cpp_int ZERO = 0;
boost::multiprecision::cpp_int TWO = 2;

class Block {
public:
    Block(const std::string& blockStart, const std::string& blockEnd) {
        _blockStart.backend() = blockStart.c_str();
        _blockEnd.backend() = blockEnd.c_str();
    }

    const std::vector<std::string>& generateResult() {
        for (boost::multiprecision::cpp_int i = _blockStart;
             i.compare(_blockEnd) <= 0;
             i++) {

            if (isPrime(i)) {
                _result.push_back(i.convert_to<std::string>());
            }
        }
        return _result;
    }

private:
    bool isPrime(boost::multiprecision::cpp_int p) {
        if (p % TWO == ZERO) return false;

        boost::multiprecision::cpp_int i = 3;
        boost::multiprecision::cpp_int p2 = p / TWO;
        while(i <= p2) {
            if (p % i == ZERO) return false;
            i += TWO;
        }

        return true;
    }

    std::vector<std::string> _result;

    boost::multiprecision::cpp_int _blockStart;
    boost::multiprecision::cpp_int _blockEnd;
};

#endif //PRIMEGEN_BLOCK_H
