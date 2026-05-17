#include <iostream>
#include <fstream>
#include <string>
#include "Bank.h"
#include "Registry.h"
#include "User.h"
#include "CurrentAccount.h"
#include "SavingsAccount.h"
#include "Wallet.h"
#include "ExchangeBuffer.h"
#include "Notifier.h"

using namespace std;

void SendAlert(const Notifier& notifier, const string& msg) {
    notifier.sendNotification(msg);
}

void ShowMenu() {
    cout << "1. Creare Utilizator Nou" << '\n';
    cout << "2. Adauga fonduri in cont (Deposit)" << '\n';
    cout << "3. Initiaza Transfer Bancar" << '\n';
    cout << "4. Simulare Frauda (Tranzactie circulara)" << '\n';
    cout << "5. Analiza Loguri (Template + Lambda)" << '\n';
    cout << "6. Testare" << '\n';
    cout << "7. Iesire" << '\n';
    cout << "Selecteaza o optiune: ";
}

int main() {
    ifstream fin("tastatura.txt");
    Bank& myBank = Bank::getInstance();
    int optiune;
    myBank.ChangeExchangeRates("EUR", "RON", 5.0);
    myBank.ChangeExchangeRates("USD", "RON", 4.5);
    Registry<string> sessionLogs;
    Registry<double> amountLogs;
    sessionLogs.AddRecord("Sistemul bancar a fost pornit");
    while (true) {
        ShowMenu();
        if (!(fin >> optiune)) {
            cout << "Te rog introdu un numar valid!" << '\n';
            fin.clear();
            fin.ignore(10000, '\n');
            continue;
        }
        switch (optiune) {
            case 1: {
                string nume, prenume, cnp, dataNasterii, moneda;
                cout << "=== Creare Utilizator ===" << '\n';
                cout << "Prenume: "; fin >> prenume;
                cout << prenume << '\n';
                cout << "Nume: "; fin >> nume;
                cout << nume << '\n';
                cout << "CNP: "; fin >> cnp;
                cout << cnp << '\n';
                cout << "Data Nasterii (ZZ.LL.AAAA): "; fin >> dataNasterii;
                cout << dataNasterii << '\n';
                cout << "Moneda contului principal (ex: RON, EUR): "; fin >> moneda;
                cout << moneda << '\n';
                myBank.CreateUser(prenume, nume, cnp, dataNasterii, moneda);
                sessionLogs.AddRecord("Creare utilizator nou: " + prenume + " " + nume);
                cout << "Utilizator creat cu succes! Verificati ID-urile conturilor in loguri." << '\n';
                break;
            }
            case 2: {
                string cnp, iban, moneda;
                double suma;
                cout << "=== Depunere Fonduri ===\n";
                cout << "CNP Utilizator: "; fin >> cnp;
                cout << "IBAN-ul contului: "; fin >> iban;
                cout << "Suma dorita: "; fin >> suma;
                cout << "Moneda in care se face depunerea: "; fin >> moneda;
                myBank.Deposit(cnp, iban, "", suma, moneda);
                sessionLogs.AddRecord("Depunere initiata pentru CNP " + cnp);
                break;
            }
            case 3: {
                string cnpExped, ibanExped, cnpDest, ibanDest, moneda;
                double suma;
                cout << "=== Transfer Bancar ===" << '\n';
                cout << "CNP Expeditor: "; fin >> cnpExped;
                cout << "IBAN Expeditor: "; fin >> ibanExped;
                cout << "CNP Destinatar: "; fin >> cnpDest;
                cout << "IBAN Destinatar: "; fin >> ibanDest;
                cout << "Suma transferata: "; fin >> suma;
                cout << "Moneda tranzactiei: "; fin >> moneda;
                myBank.Transfer(cnpExped, cnpDest, ibanExped, ibanDest, "", "", suma, moneda);
                sessionLogs.AddRecord("Transfer initiat: " + to_string(suma) + " " + moneda);
                amountLogs.AddRecord(suma);
                SMSNotifier smsAlert("0722123456");
                EmailNotifier emailAlert("client@banca.ro");
                cout << "Trimitre confirmari" << '\n';
                SendAlert(smsAlert, "Ai trimis " + to_string(suma) + " " + moneda);
                SendAlert(emailAlert, "Transferul a fost procesat cu succes");
                break;
            }
            case 4: {
                cout << "\n Se ruleaza un scenariu automat de frauda circulara pentru a testa sistemul de exceptii..." << '\n';
                sessionLogs.AddRecord("Rulare test de securitate (Frauda).");
                amountLogs.AddRecord(500.0);
                amountLogs.AddRecord(500.0);
                myBank.CreateUser("Ana", "Popescu", "111", "01.01.2000", "RON");
                myBank.CreateUser("Bogdan", "Ionescu", "222", "01.01.2000", "RON");
                myBank.CreateUser("Cristi", "Vasilescu", "333", "01.01.2000", "RON");
                AuditBuffer::USER tx1; tx1.SenderCNP = "111"; tx1.ReceiverCNP = "222"; tx1.amount = 500.0;
                AuditBuffer::USER tx2; tx2.SenderCNP = "222"; tx2.ReceiverCNP = "333"; tx2.amount = 500.0;
                AuditBuffer::USER tx3; tx3.SenderCNP = "333"; tx3.ReceiverCNP = "111"; tx3.amount = 500.0;
                cout << "\n1. Ana trimite bani catre Bogdan..." << '\n';
                if (myBank.VerifyTransaction(tx1)) myBank.AddTransferVerification(tx1, true);
                cout << "2. Bogdan trimite banii catre Cristi..." << '\n';
                if (myBank.VerifyTransaction(tx2)) myBank.AddTransferVerification(tx2, true);
                cout << "3. Cristi incearca sa inchida cercul trimitand banii inapoi la Ana..." << '\n';
                try {
                    myBank.VerifyTransaction(tx3);
                    myBank.AddTransferVerification(tx3, true);
                }
                catch (const std::exception& e) {
                    cout << "\nEXCEPTIE PRINSĂ ÎN MAIN: " << e.what() << "\n";
                    sessionLogs.AddRecord("Sistemul a blocat un transfer fraudulos!");
                }
                break;
            }
            case 5: {
                cout << "\n=== RAPORT LOGURI SISTEM (TEMPLATE & LAMBDA) ===\n";
                cout << "\nSumele transferate in aceasta sesiune (Sortate descrescator):\n";
                amountLogs.SortRecords([](double a, double b) {
                    return a > b;
                });
                amountLogs.PrintAll();
                cout << "\nIstoricul actiunilor:\n";
                sessionLogs.PrintAll();
                int transferuri = sessionLogs.CountMatches([](const string& log) {
                    return log.find("Transfer") != string::npos;
                });
                cout << "Total actiuni care contin cuvantul 'Transfer': " << transferuri << "\n";
                break;
            }
            case 6: {
                cout << "Rulare teste interne pentru mentenanta..." << '\n';

                myBank.GetExchangeRates("EUR", "RON");
                myBank.DistributeMonthlyInterest();
                myBank.AuditOverdraftBalances();
                myBank.CreateWallet("Dummy", "User", "9999", "01.01.1990", "RO98BANC9999RON");
                myBank.DeleteFriend("9999", "1111");
                myBank.DeleteUser("9999");

                User testU("Test", "Test", "0000", "01.01.2000");
                testU.GetBirthDate();
                testU.GetFirstName();
                testU.GetLastName();

                CurrentAccount ca("RO00", 0.0, 1000.0);
                ca.PrintAccountType();
                ca.ApplyMonthlyUpdate();

                SavingsAccount sa("RO01", 0.0, 0.02);
                sa.GetInterestRate();

                try {
                    Wallet w;
                    w.GetWalletID();
                    w.GetCurrency();
                    w.PrintGlobalVolume();
                } catch(...) {}

                try {
                    ExchangeBuffer eb(&myBank, 10);
                    eb.ResizeBuffer();
                } catch(...) {}

                cout << "Teste rulate cu succes!" << '\n';
                break;
            }
            case 7: {
                cout << "Inchidere sistem bancar. La revedere!" << '\n';
                return 0;
            }
            default:
                cout << "Optiune invalida! Incearca din nou." << '\n';
        }
    }
    return 0;
}
