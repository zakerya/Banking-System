#ifndef ACCOUNT_H
#define ACCOUNT_H

typedef struct {
    int accountNumber;
    int pin;
    char name[50];
    char currency;
    float balance;
} Account;

#endif