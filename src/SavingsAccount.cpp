//
// Created by Luca Teodorescu on 17.05.2026.
//

#include "SavingsAccount.h"

SavingsAccount::SavingsAccount() : Account(), interestRate(0.0) {}

SavingsAccount::SavingsAccount(const std::string& iban, double startBalance, double rate)
    : Account(iban, startBalance), interestRate(rate) {}

SavingsAccount::SavingsAccount(const SavingsAccount& other)
    : Account(other), interestRate(other.interestRate) {}

SavingsAccount& SavingsAccount::operator=(const SavingsAccount& other) {
    if (this != &other) {
        Account::operator=(other);
        interestRate = other.interestRate;
    }
    return *this;
}

SavingsAccount::~SavingsAccount() {}
double SavingsAccount::GetInterestRate() const { return interestRate; }

void SavingsAccount::AddInterest() {
    double interest = balance * interestRate;
    balance += interest;
    std::cout << "Dobanda de " << interest << " a fost adaugata contului " << IBAN << std::endl;
}

void SavingsAccount::PrintAccountType() const {
    std::cout << "Cont Economii -> IBAN: " << IBAN << " | Sold: " << balance << " | Rata Dobanda: " << interestRate * 100 << "%" << std::endl;
}

void SavingsAccount::ApplyMonthlyUpdate() {
    AddInterest();
}