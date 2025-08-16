#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define FILE_NAME "bank.dat"

int choice;

int accountNumber;
int getAccountNumber;
int storedAccountNumber;

int pin;
int getPin;
int storedPin;

char name[50];
char storedName[50];

char line[200];

char currency = '$';
char storedCurrency = '$';

float balance;
float storedBalance;

float amount;

void displayAccounts() {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("No account found. Please create an account first.");
        return;
    }

    printf("\n%-18s %-30s %-10s\n", "Account Number", "Name", "Balance");
    printf("----------------------------------------------------------\n");

    int accountsFound = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%*d|%[^|]|%c|%f", &accountNumber, name, &currency, &balance) == 4) {
            printf("%-18d %-30s %c%-10.2f\n", accountNumber, name, currency, balance);
            accountsFound = 1;
        }
    }

    if (!accountsFound) {
        printf("\nNo accounts found.\n");
    }
    fclose(file);
}

void saveAccount() {
    FILE *file = fopen(FILE_NAME, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    fprintf(file, "%d|%d|%s|%c|%.2f\n", accountNumber, pin, name, currency, balance);
    fclose(file);
    printf("Account saved successfully!\n");
}

int generateAccountNumber() {
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }
    return 1000000 + rand() % 9000000;
}

void createAccount() {
    accountNumber = generateAccountNumber();
    printf("\nYour account number is: %d\n", accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    printf("Enter name: ");
    scanf(" %[^\n]", name);
    balance = 0.0f;
    printf("Account created successfully!\n");
    saveAccount();
}

void depositMoney() {

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("No account found. Please create an account first.");
        return;
    }

    printf("Enter account number: ");
    scanf("%d", &getAccountNumber);

    int accountsFound = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|", &storedAccountNumber) == 1) {
            if (storedAccountNumber == getAccountNumber) {
                accountsFound = 1;
                break;
            }
        }
    }

    if (!accountsFound) {
        printf("\nThis account does not exist in the database.\n");
        fclose(file);
        return;
    }

    printf("Enter PIN: ");
    scanf("%d", &getPin);

    rewind(file);

    char tempFileName[] = "temp.dat";
    FILE *tempFile = fopen(tempFileName, "w");
    if (tempFile == NULL) {
        perror("Error creating temp file");
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%d|%[^|]|%c|%f", &storedAccountNumber, &storedPin, storedName, &storedCurrency, &storedBalance) == 5) {
                if (storedAccountNumber == getAccountNumber && storedPin == getPin) {
                    printf("Enter amount to deposit: ");
                    scanf("%f", &amount);
                    if (amount > 0) {
                        storedBalance += amount;
                        printf("\nDeposit successful! New balance: %c%.2f\n", storedCurrency, storedBalance);
                    } else {
                        printf("Invalid deposit amount.\n");
                    }
                accountsFound = 1;
            }
            fprintf(tempFile, "%d|%d|%s|%c|%.2f\n", storedAccountNumber, storedPin, storedName, storedCurrency, storedBalance);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (accountsFound) {
        remove (FILE_NAME);
        rename(tempFileName, FILE_NAME);
    } else {
        printf("\nNo matching account found for the provided account number and PIN.\n");
        remove (tempFileName);
    }
}

void withdrawMoney() {

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("No account found. Please create an account first.");
        return;
    }

    printf("Enter account number: ");
    scanf("%d", &getAccountNumber);
    printf("Enter PIN: ");
    scanf("%d", &getPin);

    char tempFileName[] = "temp.dat";
    FILE *tempFile = fopen(tempFileName, "w");
    if (tempFile == NULL) {
        perror("Error creating temp file");
        fclose(file);
        return;
    }

    int accountsFound = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%d|%[^|]|%c|%f", &storedAccountNumber, &storedPin, storedName, &storedCurrency, &storedBalance) == 5) {
            if (storedAccountNumber == getAccountNumber && storedPin == getPin) {
                printf("Enter amount to withdraw: ");
                scanf("%f", &amount);
                if (amount > 0 && amount <= storedBalance) {
                    storedBalance -= amount;
                    printf("\nWithdrawal successful! New balance: %c%.2f\n", storedCurrency, storedBalance);
                    accountsFound = 1;
                } else if (amount > storedBalance) {
                    printf("Insufficient funds. Withdrawal failed.\n");
                    accountsFound = 1;
                } else {
                    printf("Invalid withdrawal amount.\n");
                    accountsFound = 1;
                }
            }

            fprintf(tempFile, "%d|%d|%s|%c|%.2f\n", storedAccountNumber, storedPin, storedName, storedCurrency, storedBalance);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove(FILE_NAME);
    rename(tempFileName, FILE_NAME);

    if (!accountsFound) {
        printf("Invalid account number or PIN.\n");
    }
}

void checkBalance() {

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("No account found. Please create an account first.");
        return;
    }

    printf("Enter account number: ");
    scanf("%d", &getAccountNumber);
    printf("Enter PIN: ");
    scanf("%d", &getPin);

    int accountsFound = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%d|%[^|]|%c|%f", &storedAccountNumber, &storedPin, storedName, &storedCurrency, &storedBalance) == 5) {
            if (storedAccountNumber == getAccountNumber && storedPin == getPin) {
                printf("\nAccount Holder: %s\n", storedName);
                printf("Current Balance: %c%.2f\n", storedCurrency, storedBalance);
                accountsFound = 1;
                break; 
            }
        }
    }
    if (!accountsFound) {
        printf("Invalid account number or PIN.\n");
    }
    fclose(file);
}

void deleteAccount() {

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("No account found. Please create an account first.");
        return;
    }

    printf("Enter account number to delete: ");
    scanf("%d", &getAccountNumber);
    printf("Enter PIN: ");
    scanf("%d", &getPin);

    if (getAccountNumber == accountNumber && getPin == pin) {
        FILE *file = fopen(FILE_NAME, "w");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }
        accountNumber = 0;
        pin = 0;
        strcpy(name, "");
        balance = 0.0f;
        printf("\nAccount deleted successfully!\n");
        printf("\nReturning back to menu.\n");
    } else {
        printf("Invalid account number or PIN. Account deletion failed.\n");
    }
}

int main() {
    printf("\nWelcome to the Bank Management System\n");
    Sleep(1000);
    while (1) {
        printf("\n1. Create Account\n");
        printf("2. Display Account(s)\n");
        printf("3. Deposit Money\n");
        printf("4. Withdraw Money\n");
        printf("5. Check Balance\n");
        printf("6. Delete Account\n");
        printf("7. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                displayAccounts();
                break;
            case 3:
                depositMoney();
                break;
            case 4:
                withdrawMoney();
                break;
            case 5:
                checkBalance();
                break;
            case 6:
                deleteAccount();
                break;
            case 7:
                printf("\nBank Management System has been closed\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}