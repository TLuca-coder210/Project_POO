#include <iostream>
#include <chrono>
#include <thread>
#include "Bank.h"

using namespace std;

int main() {
    Bank myBank;
    string cnpA = "CNP_ALEX";
    string cnpB = "CNP_BOGDAN";
    string cnpC = "CNP_CRISTI";
    double sumaTranzactie = 100.0;

    AuditBuffer::USER tx1;
    tx1.SenderCNP = cnpA;       tx1.ReceiverCNP = cnpB;
    tx1.SenderIBAN = "IBAN_A";  tx1.ReceiverIBAN = "IBAN_B";
    tx1.SenderWalletID = "W_A"; tx1.ReceiverWalletID = "W_B";
    tx1.amount = sumaTranzactie;

    if (myBank.VerifyTransaction(tx1)) {
        myBank.AddTransferVerification(tx1, true);
    }
    this_thread::sleep_for(chrono::milliseconds(200));

    AuditBuffer::USER tx2;
    tx2.SenderCNP = cnpB;       tx2.ReceiverCNP = cnpC;
    tx2.SenderIBAN = "IBAN_B";  tx2.ReceiverIBAN = "IBAN_C";
    tx2.SenderWalletID = "W_B"; tx2.ReceiverWalletID = "W_C";
    tx2.amount = sumaTranzactie;

    if (myBank.VerifyTransaction(tx2)) {
        myBank.AddTransferVerification(tx2, true);
    }
    this_thread::sleep_for(chrono::milliseconds(200));

    AuditBuffer::USER tx3;
    tx3.SenderCNP = cnpC;       tx3.ReceiverCNP = cnpA;
    tx3.SenderIBAN = "IBAN_C";  tx3.ReceiverIBAN = "IBAN_A";
    tx3.SenderWalletID = "W_C"; tx3.ReceiverWalletID = "W_A";
    tx3.amount = sumaTranzactie;

    if (myBank.VerifyTransaction(tx3)) {
        myBank.AddTransferVerification(tx3, true);
    }
    this_thread::sleep_for(chrono::milliseconds(200));
    return 0;
}
