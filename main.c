#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define FILE_NAME "bank.dat"

int choice;
int accountNumber;
int getAccountNumber;
int getPin;
int pin;

char name[50];
char line[200];
char currency = '$';

float balance;
float amount;

void displayAccounts() {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("No account found. Please create an account first.");
        return;
    }

    if (fgets(line, sizeof(line), file) == NULL) {
        printf("\nNo accounts found. Retruning back to menu.\n");
        Sleep(1500);
        fclose(file);
        return;
    }

    printf("%-18s %-30s %-10s\n", "Account Number", "Name", "Balance");
    printf("----------------------------------------------------------\n");

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%*d|%[^|]|%c|%f", &accountNumber, name, &currency, &balance) == 4) {
            printf("%-18d %-30s %c%-10.2f\n", accountNumber, name, currency, balance);
        }
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
    scanf(" %[^\n]", name); // Read string with spaces
    balance = 0.0f; // Initialize balance to 0
    printf("Account created successfully!\n");
    saveAccount();
}

void depositMoney() {
    printf("Enter amount to deposit: ");
    scanf("%f", &amount);
    if (amount > 0) {
        balance += amount;
        printf("Deposit successful! New balance: %.2f\n", balance);
    } else {
        printf("Invalid deposit amount.\n");
    }
}

void withdrawMoney() {
    printf("Enter amount to withdraw: ");
    scanf("%f", &amount);
    if (amount > 0 && amount <= balance) {
        balance -= amount;
        printf("Withdrawal successful! New balance: %.2f\n", balance);
    } else {
        printf("Invalid withdrawal amount or insufficient funds.\n");
    }
}

void checkBalance() {

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("No account found. Please create an account first.");
        return;
    }

    fscanf(file, "%d", &accountNumber);
    fscanf(file, "%d", &pin);
    fgetc(file);
    fgets(name, sizeof(name), file);
    name[strcspn(name, "\n")] = '\0';
    fscanf(file, "%f", &balance);
    fclose(file);

    while (1) {
        printf("Enter account number: ");
        scanf("%d", &getAccountNumber);
        printf("Enter PIN: "); 
        scanf("%d", &getPin);

        if (getAccountNumber == accountNumber && getPin == pin) {
            printf("Current balance: %.2f\n", balance);
            break;
        }

        while (1) {
            if (getAccountNumber != accountNumber || getPin != pin) {
            printf("Invalid account number or PIN. Please try again.\n");
            printf("Enter account number: ");
            scanf("%d", &getAccountNumber);
            printf("Enter PIN: ");
            scanf("%d", &getPin);
            }

            if (getAccountNumber == accountNumber && getPin == pin) {
                printf("\nCurrent balance: %.2f\n", balance);
                printf("\nBlanced checked successfully. Returning back to menu.\n");
                break;
            }
        }
        break;
    }
}

void deleteAccount() {

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("No account found. Please create an account first.");
        return;
    }

    fscanf(file, "%d", &accountNumber);
    fscanf(file, "%d", &pin);
    fgetc(file);
    fgets(name, sizeof(name), file);
    name[strcspn(name, "\n")] = '\0';
    fscanf(file, "%f", &balance);
    fclose(file);

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
    Sleep(1500);
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