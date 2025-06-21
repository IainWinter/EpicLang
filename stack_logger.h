#pragma once

#include <cstdio>
#include <cstdarg>

class StackLogger {
public:
    StackLogger() : m_indent(0) {}

    void push() {
        m_indent++;
    }

    void pop() {
        if (m_indent > 0) {
            m_indent--;
        }
    }

    void operator()(const char* format, ...) const {
        for (int i = 0; i < m_indent; ++i) {
            printf("  ");
        }

        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }

private:
    int m_indent;
};