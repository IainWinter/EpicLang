#pragma once

#include <vector>
#include <string_view>

#include "byte_code_enum.h"

class ByteStack {
public:
    void push_string(std::string_view val);
    void push_bool(bool val);
    void push_int(int val);
    void push_float(const float& val);

    const Type& top_value_type(size_t item_index = 0) const;

    std::string_view top_as_string(size_t item_index = 0) const;
    const bool& top_as_bool(size_t item_index = 0) const;
    const int& top_as_int(size_t item_index = 0) const;
    const float& top_as_float(size_t item_index = 0) const;

    void pop(size_t item_count = 1);

    size_t size() const;

    bool equals(const ByteStack& other) const;

    void print() const;

private:
    void write_type(Type type);
    
    void write_string(const std::string_view& value);

    template<typename T>
    void write(const T& value) {
        const char* ptr = reinterpret_cast<const char*>(&value);
        m_buffer.insert(m_buffer.end(), ptr, ptr + sizeof(T));
    }

    template<typename T>
    const T& read(size_t head) const {
        return *reinterpret_cast<const T*>(m_buffer.data() + head - sizeof(T));
    }

    size_t get_item_offset(size_t item_index) const;

    size_t get_value_offset(size_t item_index) const;

private:
    std::vector<char> m_buffer;
};