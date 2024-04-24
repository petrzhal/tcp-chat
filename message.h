#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include <chrono>
#include <cstring>

#define MSG_SIZE 512
#define NAME_SIZE 32

struct Message {
    char content[MSG_SIZE]{};
    std::chrono::system_clock::time_point time{};
    char name[NAME_SIZE]{};

    Message() = default;

    Message(const char *content, std::chrono::system_clock::time_point time, const char* name) {
        strcpy_s(this->content, content);
        strcpy_s(this->name, name);
        this->time = time;
    }

    Message(const Message& other) {
        strcpy_s(this->content, other.content);
        strcpy_s(this->name, other.name);
        this->time = other.time;
    }

    Message& operator=(const Message& other) {
        if (this != &other) {
            memset(this->content, 0, MSG_SIZE);
            memset(this->name, 0, NAME_SIZE);
            strcpy_s(this->content, other.content);
            strcpy_s(this->name, other.name);
            this->time = other.time;
        }
        return *this;
    }

    ~Message() = default;
};

#endif // MESSAGE_H
