//
// Created by Luca Teodorescu on 20.03.2026.
//

#ifndef PROJECT_OOP_ACCOUNT_H
#define PROJECT_OOP_ACCOUNT_H

#include <string>
#include <iostream>

class Account {
protected:
    std::string IBAN;
    double balance;

public:
    Account();
    Account(const std::string& iban, double startBalance);
    virtual ~Account();

    Account(const Account& other);
    Account& operator=(const Account& other);

    std::string GetIBAN() const;
    double GetBalance() const;
    void Deposit(double amount);
    virtual bool Withdraw(double amount);

    virtual void PrintAccountType() const = 0;
    virtual void ApplyMonthlyUpdate() = 0;
};

#endif