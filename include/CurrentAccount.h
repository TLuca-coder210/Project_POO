//
// Created by Luca Teodorescu on 17.05.2026.
//

#ifndef PROJECT_OOP_CURRENTACCOUNT_H
#define PROJECT_OOP_CURRENTACCOUNT_H

#include "Account.h"

class CurrentAccount : public Account {
private:
    double overdraftLimit;

public:
    CurrentAccount();
    CurrentAccount(const std::string& iban, double startBalance, double limit);

    CurrentAccount(const CurrentAccount& other);
    CurrentAccount& operator=(const CurrentAccount& other);

    ~CurrentAccount() override;

    double GetOverdraftLimit() const;

    bool Withdraw(double amount) override;
    void PrintAccountType() const override;
    void ApplyMonthlyUpdate() override;
};

#endif