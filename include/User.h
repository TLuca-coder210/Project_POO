//
// Created by Luca Teodorescu on 20.03.2026.
//

#ifndef PROJECT_OOP_USER_H
#define PROJECT_OOP_USER_H

#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Account;

class User {
    string FirstName, LastName, CNP, BirthDate;
    map<string, Account*> AllAccounts;
    map<string, bool> SentFriendInvitations;
    map<string, bool> AcceptedFriendInvitations;
    map<string, bool> Friends;
public:
    User(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate);
    User(const string &FirstName, const string &LastName, const string &CNP, const string &BirthDate, const string &currency);
    ~User();
    void AddAccount(const string &currency = "RON");
    void DeleteAccount();
    void DeleteAccount(const string &IBAN);
    void AddWallet(const string &IBAN);
    void DeleteWallet(const string &IBAN, const string &WalletID);
    string GetBirthDate();
    string GetFirstName();
    string GetLastName();
    string GetCNP();
    Account *GetAccount(const string &IBAN);
    [[nodiscard]]const map<string, Account*>& GetAllAccounts() const;
    void SendFriendInvitation(const string &CNP);
    bool AcceptFriendInvitation(const string &CNP);
    void AddFriend(const string &CNP);
    void EraseFriend(const string &CNP);
    bool CheckFriend(const string &CNP);
};

#endif //PROJECT_OOP_USER_H