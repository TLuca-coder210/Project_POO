//
// Created by Luca Teodorescu on 20.03.2026.
//

#include "User.h"
#include "Account.h"
#include <iostream>

User::User(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate) {
    this->FirstName = FirstName;
    this->LastName = LastName;
    this->CNP = CNP;
    this->BirthDate = BirthDate;
    Account *new_account = new Account(this);
    string IBAN = new_account->GetIBAN();
    if (!AllAccounts.contains(IBAN)) {
        AllAccounts[IBAN] = new_account;
    }
    else {
        delete new_account;
        cout << "Eroare critică: IBAN duplicat generat!" << '\n';
    }
}

User::User(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate, const string &currency) {
    this->FirstName = FirstName;
    this->LastName = LastName;
    this->CNP = CNP;
    this->BirthDate = BirthDate;
    Account *new_account = new Account(this, currency);
    string IBAN = new_account->GetIBAN();
    if (AllAccounts.find(IBAN) == AllAccounts.end()) {
        AllAccounts[IBAN] = new_account;
    }
}

string User::GetBirthDate() {
    return this->BirthDate;
}

string User::GetFirstName() {
    return this->FirstName;
}

string User::GetLastName() {
    return this->LastName;
}

string User::GetCNP() {
    return this->CNP;
}

void User::AddAccount(const string &currency) {
    Account *new_account = new Account(this, currency);
    string IBAN = new_account->GetIBAN();
    if (AllAccounts.find(IBAN) == AllAccounts.end()) {
        AllAccounts[IBAN] = new_account;
    }
}

void User::DeleteAccount() {
    for (const auto &it : AllAccounts) {
        Account *account = it.second;
        account->DeleteWallet();
    }
    AllAccounts.clear();
}

void User::DeleteAccount(const string &IBAN) {
    auto it = AllAccounts.find(IBAN);
    if (it != AllAccounts.end()) {
        delete it->second;
        AllAccounts.erase(it);
        cout << "Contul cu IBAN-ul " << IBAN << " a fost sters cu succes din sistem.\n";
    }
    else {
        cout << "Eroare: Acest IBAN nu apartine utilizatorului curent!\n";
    }
}

void User::AddWallet(const string &IBAN) {
    if (AllAccounts.find(IBAN) == AllAccounts.end()) {
        Account *new_account = new Account(this);
        new_account->addWallet();
    }
    else {
        Account *new_account = AllAccounts[IBAN];
        new_account->addWallet();
    }
}

void User::DeleteWallet(const string &IBAN, const string &WalletID) {
    if (AllAccounts.find(IBAN) != AllAccounts.end()) {
        Account *account = AllAccounts[IBAN];
        account->DeleteWallet(WalletID);
    }
    else {
        cout << "IBAN invalid" << '\n';
    }
}

Account *User::GetAccount(const string &IBAN) {
    if (AllAccounts.find(IBAN) != AllAccounts.end()) {
        Account *account = AllAccounts[IBAN];
        return account;
    }
    return nullptr;
}

void User::SendFriendInvitation(const string &CNP_) {
    if (SentFriendInvitations.find(CNP_) == SentFriendInvitations.end()) {
        SentFriendInvitations[CNP_] = true;
    }
    else {
        cout << "Invitation already exists" << endl;
    }
}

bool User::AcceptFriendInvitation(const string &CNP_) {
    if (AcceptedFriendInvitations.find(CNP_) == AcceptedFriendInvitations.end()) {
        AcceptedFriendInvitations[CNP_] = true;
        return true;
    }
    else {
        cout << "Invitation already accepted" << '\n';
        return false;
    }
}

void User::AddFriend(const string &CNP_) {
    Friends[CNP_] = true;
}

void User::EraseFriend(const string &CNP_) {
    if (Friends.find(CNP_) != Friends.end()) {
        Friends.erase(CNP_);
    }
}

bool User::CheckFriend(const string &CNP_) {
    if (Friends.find(CNP_) != Friends.end()) {
        return true;
    }
    return false;
}

User::~User() {
    for (auto &it : AllAccounts) {
        delete it.second;
    }
    AllAccounts.clear();
}
