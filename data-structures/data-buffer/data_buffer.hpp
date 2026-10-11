#ifndef DATA_BUFFER_CPP
#define DATA_BUFFER_CPP

#include "data_buffer.cpp"
#include <vector>


class DataBuffer {
private:
    // stdvector<int> arr;

public:
    DataBuffer();
    DataBuffer(const DataBuffer &rhs);
    DataBuffer& operator=(const DataBuffer &rhs);
    ~DataBuffer();

    template<typename T>
    void operator<<(const T object);
};

#endif
