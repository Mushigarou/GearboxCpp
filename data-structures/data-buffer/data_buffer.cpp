#include "data_buffer.hpp"

DataBuffer::DataBuffer() {

    return;
}


DataBuffer::DataBuffer(const DataBuffer &rhs) {
    if (this != &rhs) {
        
    }
    return;
}

DataBuffer& operator=(const DataBuffer &rhs) {
    
    return *this;
}


~DataBuffer::DataBuffer() {
    return;
}

template<typename T>
void operator<<(const T object) {
    return;
}