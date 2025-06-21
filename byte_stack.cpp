#include "byte_stack.h"

#include "byte_code_enum_translation.h"
#include "byte_code_printer.h"

#include <string>

void ByteStack::push_string(std::string_view val) {
    write_string(val);
    write_type(Type::STRING);
}

void ByteStack::push_bool(bool val) {
    write(val);
    write_type(Type::BOOL);
}

void ByteStack::push_int(int val) {
    write(val);
    write_type(Type::INT);
}

void ByteStack::push_float(const float& val) {
    write(val);
    write_type(Type::FLOAT);
}

const Type& ByteStack::top_value_type(size_t item_index) const {
    size_t head = get_item_offset(item_index);
    return read<Type>(head);
}

std::string_view ByteStack::top_as_string(size_t item) const {
    size_t head = get_value_offset(item); // this could wrap, but get_item_offset adds 1 so it's fine

    const size_t& string_len = read<size_t>(head);
    head -= sizeof(size_t);
    head -= string_len;

    return std::string_view(m_buffer.data() + head, string_len);
}

const bool& ByteStack::top_as_bool(size_t item) const {
    size_t head = get_value_offset(item);
    return read<bool>(head);
}

const int& ByteStack::top_as_int(size_t item) const {
    size_t head = get_value_offset(item);
    return read<int>(head);
}

const float& ByteStack::top_as_float(size_t item) const{
    size_t head = get_value_offset(item);
    return read<float>(head);
}

void ByteStack::pop(size_t item_count) {
    size_t head = get_item_offset(item_count);
    m_buffer.erase(m_buffer.begin() + head, m_buffer.end());
}

size_t ByteStack::size() const {
    return m_buffer.size();
}

bool ByteStack::equals(const ByteStack& other) const {
    return m_buffer == other.m_buffer;
}

void ByteStack::write_type(Type type) {
    write<char>(static_cast<char>(type));
}

void ByteStack::write_string(const std::string_view& value) {
    m_buffer.insert(m_buffer.end(), value.begin(), value.end());
    write<size_t>(value.size());
}

size_t ByteStack::get_item_offset(size_t item_index) const {
    size_t head = m_buffer.size();

    for (size_t i = 0; i < item_index; i++) {
        if (head < sizeof(Type)) {
            return 0;
        }

        const Type& type = read<Type>(head);
        head -= sizeof(Type);

        switch (type) {
            case Type::STRING: { 
                const size_t& string_len = read<size_t>(head);
                head -= sizeof(size_t);
                head -= string_len;
                break;
            }
            case Type::BOOL: {
                head -= sizeof(bool);
                break;
            }
            case Type::INT: {
                head -= sizeof(int);
                break;
            }
            case Type::FLOAT: { 
                head -= sizeof(float);
                break;
            }
        }
    }

    return head;
}

size_t ByteStack::get_value_offset(size_t item_index) const {
    size_t head = get_item_offset(item_index);
    head -= sizeof(Type);
    return head;
}

void ByteStack::print() const {
    size_t head = m_buffer.size();

    while (true) {
        if (head < sizeof(Type)) {
            return;
        }

        const Type& type = read<Type>(head);
        head -= sizeof(Type);

        printf("%s ", type_to_string(type).data());

        switch (type) {
            case Type::STRING: { 
                const size_t& string_len = read<size_t>(head);
                head -= sizeof(size_t);
                head -= string_len;

                std::string_view value(m_buffer.data() + head, string_len);
                print_string(value);

                break;
            }
            case Type::BOOL: {
                bool value = read<bool>(head);
                print_bool(value);
                head -= sizeof(bool);
                break;
            }
            case Type::INT: {
                int value = read<int>(head);
                print_int(value);
                head -= sizeof(int);
                break;
            }
            case Type::FLOAT: { 
                float value = read<float>(head);
                print_float(value);
                head -= sizeof(float);
                break;
            }
        }

        printf("\n");
    }
}