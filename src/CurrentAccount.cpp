//
// Created by Luca Teodorescu on 17.05.2026.
//

#include "CurrentAccount.h"

CurrentAccount::CurrentAccount() : Account(), overdraftLimit(0.0) {}

CurrentAccount::CurrentAccount(const std::string& iban, double startBalance, double limit)
    : Account(iban, startBalance), overdraftLimit(limit) {}

CurrentAccount::CurrentAccount(const CurrentAccount& other)
    : Account(other), overdraftLimit(other.overdraftLimit) {}

CurrentAccount& CurrentAccount::operator=(const CurrentAccount& other) {
    if (this != &other) {
        Account::operator=(other);
        overdraftLimit = other.overdraftLimit;
    }
    return *this;
}

CurrentAccount::~CurrentAccount() {}
double CurrentAccount::GetOverdraftLimit() const { return overdraftLimit; }

bool CurrentAccount::Withdraw(double amount) {
    if (balance + overdraftLimit >= amount) {
        balance -= amount;
        return true;
    }
    std::cout << "Eroare: Limita de overdraft a fost depasita!" << std::endl;
    return false;
}

void CurrentAccount::PrintAccountType() const {
    std::cout << "Cont Curent -> IBAN: " << IBAN << " | Sold: " << balance << " | Overdraft: " << overdraftLimit << std::endl;
}

void CurrentAccount::ApplyMonthlyUpdate() {
    balance -= 10.0;
}