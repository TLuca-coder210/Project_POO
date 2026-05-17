//
// Created by Luca Teodorescu on 20.03.2026.
//

#include "Account.h"

Account::Account() : IBAN("UNKNOWN"), balance(0.0) {}
Account::Account(const std::string& iban, double startBalance) : IBAN(iban), balance(startBalance) {}
Account::~Account() {}

Account::Account(const Account& other) : IBAN(other.IBAN), balance(other.balance) {}
Account& Account::operator=(const Account& other) {
    if (this != &other) {
        IBAN = other.IBAN;
        balance = other.balance;
    }
    return *this;
}

const std::string& Account::GetIBAN() const { return IBAN; }
double Account::GetBalance() const { return balance; }
void Account::Deposit(double amount) { balance += amount; }
bool Account::Withdraw(double amount) {
    if (balance >= amount) {
        balance -= amount;
        return true;
    }
    return false;
}
