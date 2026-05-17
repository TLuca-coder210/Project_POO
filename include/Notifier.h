//
// Created by Luca Teodorescu on 17.05.2026.
//

#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <iostream>
#include <string>

class Notifier {
public:
    virtual ~Notifier() = default;
    virtual void sendNotification(const std::string& message) const = 0;
};

class SMSNotifier : public Notifier {
private:
    std::string phoneNumber;
public:
    explicit SMSNotifier(const std::string& phone) : phoneNumber(phone) {}
    void sendNotification(const std::string& message) const override {
        std::cout << "SMS catre " << phoneNumber << ": " << message << '\n';
    }
};

class EmailNotifier : public Notifier {
private:
    std::string emailAddress;
public:
    explicit EmailNotifier(const std::string& email) : emailAddress(email) {}
    void sendNotification(const std::string& message) const override {
        std::cout << "EMAIL catre " << emailAddress << ": " << message << '\n';
    }
};

#endif // NOTIFIER_H