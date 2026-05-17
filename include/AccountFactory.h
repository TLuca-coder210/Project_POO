//
// Created by Luca Teodorescu on 17.05.2026.
//

#ifndef PROJECT_OOP_ACCOUNTFACTORY_H
#define PROJECT_OOP_ACCOUNTFACTORY_H

#include <string>
#include "Account.h"
#include "CurrentAccount.h"
#include "SavingsAccount.h"

class AccountFactory {
public:
    static Account* CreateAccount(const std::string& accountType, const std::string& iban, double startBalance) {
        if (accountType == "Current" || accountType == "Curent") {
            return new CurrentAccount(iban, startBalance, 1000.0);
        }
        else if (accountType == "Savings" || accountType == "Economii") {
            return new SavingsAccount(iban, startBalance, 0.02);
        }
        return nullptr;
    }
};

#endif //PROJECT_OOP_ACCOUNTFACTORY_H