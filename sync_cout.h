#ifndef SYNC_COUT_H
#define SYNC_COUT_H

#include <iostream>
#include <sstream>
#include <mutex>

class sync_cout: public std::ostringstream
{
public:
    sync_cout() = default;

    ~sync_cout() {
        std::lock_guard<std::mutex> guard(_mutexPrint);
        std::cout << this->str();
    }

private:
    std::mutex _mutexPrint;
};

#endif // SYNC_COUT_H
