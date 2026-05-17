//
// Created by Luca Teodorescu on 17.05.2026.
//

#ifndef PROJECT_OOP_SAVINGSACCOUNT_H
#define PROJECT_OOP_SAVINGSACCOUNT_H

#include "Account.h"

class SavingsAccount : public Account {
private:
    double interestRate;

public:
    SavingsAccount();
    SavingsAccount(const std::string& iban, double startBalance, double rate);

    SavingsAccount(const SavingsAccount& other);
    SavingsAccount& operator=(const SavingsAccount& other);

    ~SavingsAccount() override;

    double GetInterestRate() const;
    void AddInterest();

    void PrintAccountType() const override;
    void ApplyMonthlyUpdate() override;
};

#endif