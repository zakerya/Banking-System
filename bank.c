#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILE_NAME "bank.dat"

int generateAccountNumber() {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }

    int tries = 0;
    while (tries < 100) {
        int candidate = 1000000 + rand() % 9000000;

        // ensure uniqueness
        FILE *f = fopen(FILE_NAME, "r");
        int found = 0;
        if (f) {
            char line[256];
            while (fgets(line, sizeof(line), f)) {
                int acc;
                if (sscanf(line, "%d|", &acc) == 1) {
                    if (acc == candidate) { found = 1; break; }
                }
            }
            fclose(f);
        }
        if (!found) return candidate;
        tries++;
    }
    return 1000000 + rand() % 9000000; // fallback
}

void cmd_create(int pin, const char *name) {
    int accountNumber = generateAccountNumber();
    float balance = 0.0f;
    FILE *f = fopen(FILE_NAME, "a");
    if (!f) {
        perror("Error opening file for writing");
        return;
    }
    fprintf(f, "%d|%d|%s|%c|%.2f\n", accountNumber, pin, name, '$', balance);
    fclose(f);
    printf("Created account %d\n", accountNumber);
}

void cmd_display() {
    FILE *f = fopen(FILE_NAME, "r");
    if (!f) {
        printf("NOACCOUNTS: No accounts found.\n");
        return;
    }
    printf("Account Number | PIN | Name | Currency | Balance\n");
    printf("------------------------------------------------\n");
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        int acc, pin;
        char name[128];
        char cur;
        float bal;
        if (sscanf(line, "%d|%d|%127[^|]|%c|%f", &acc, &pin, name, &cur, &bal) == 5) {
            printf("%d | %d | %s | %c | %.2f\n", acc, pin, name, cur, bal);
        }
    }
    fclose(f);
}

void update_record_and_replace(FILE *in, FILE *out, int match_acc, int match_pin, float delta, int is_deposit, int *found_flag, int *insufficient_flag) {
    char line[256];
    while (fgets(line, sizeof(line), in)) {
        int acc, pin;
        char name[128];
        char cur;
        float bal;
        if (sscanf(line, "%d|%d|%127[^|]|%c|%f", &acc, &pin, name, &cur, &bal) == 5) {
            if (acc == match_acc && pin == match_pin) {
                *found_flag = 1;
                if (is_deposit) {
                    bal += delta;
                    printf("OK: Deposit successful. New balance: %c%.2f\n", cur, bal);
                } else {
                    if (delta > bal) {
                        *insufficient_flag = 1;
                        printf("FAIL: Insufficient funds. Balance: %c%.2f\n", cur, bal);
                    } else {
                        bal -= delta;
                        printf("OK: Withdrawal successful. New balance: %c%.2f\n", cur, bal);
                    }
                }
            }
            fprintf(out, "%d|%d|%s|%c|%.2f\n", acc, pin, name, cur, bal);
        }
    }
}

void cmd_deposit(int account, int pin, float amount) {
    FILE *f = fopen(FILE_NAME, "r");
    if (!f) { printf("NOACCOUNTS: No accounts found.\n"); return; }
    FILE *tmp = fopen("temp.dat", "w");
    if (!tmp) { perror("Error creating temp file"); fclose(f); return; }

    int found = 0, insufficient = 0;
    update_record_and_replace(f, tmp, account, pin, amount, 1, &found, &insufficient);

    fclose(f);
    fclose(tmp);

    if (found) {
        remove(FILE_NAME);
        rename("temp.dat", FILE_NAME);
    } else {
        remove("temp.dat");
        printf("FAIL: Account or PIN not found.\n");
    }
}

void cmd_withdraw(int account, int pin, float amount) {
    FILE *f = fopen(FILE_NAME, "r");
    if (!f) { printf("NOACCOUNTS: No accounts found.\n"); return; }
    FILE *tmp = fopen("temp.dat", "w");
    if (!tmp) { perror("Error creating temp file"); fclose(f); return; }

    int found = 0, insufficient = 0;
    update_record_and_replace(f, tmp, account, pin, amount, 0, &found, &insufficient);

    fclose(f);
    fclose(tmp);

    if (found && !insufficient) {
        remove(FILE_NAME);
        rename("temp.dat", FILE_NAME);
    } else if (found && insufficient) {
        remove("temp.dat");
        // message already printed in helper
    } else {
        remove("temp.dat");
        printf("FAIL: Account or PIN not found.\n");
    }
}

void cmd_check(int account, int pin) {
    FILE *f = fopen(FILE_NAME, "r");
    if (!f) { printf("NOACCOUNTS: No accounts found.\n"); return; }
    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        int acc, p;
        char name[128];
        char cur;
        float bal;
        if (sscanf(line, "%d|%d|%127[^|]|%c|%f", &acc, &p, name, &cur, &bal) == 5) {
            if (acc == account && p == pin) {
                found = 1;
                printf("OK: Account Holder: %s\nBalance: %c%.2f\n", name, cur, bal);
                break;
            }
        }
    }
    if (!found) printf("FAIL: Account or PIN not found.\n");
    fclose(f);
}

void cmd_delete(int account, int pin) {
    FILE *f = fopen(FILE_NAME, "r");
    if (!f) { printf("NOACCOUNTS: No accounts found.\n"); return; }
    FILE *tmp = fopen("temp.dat", "w");
    if (!tmp) { perror("Error creating temp file"); fclose(f); return; }

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        int acc, p;
        char name[128];
        char cur;
        float bal;
        if (sscanf(line, "%d|%d|%127[^|]|%c|%f", &acc, &p, name, &cur, &bal) == 5) {
            if (acc == account && p == pin) {
                found = 1;
                // skip writing -> delete
                continue;
            }
            fprintf(tmp, "%d|%d|%s|%c|%.2f\n", acc, p, name, cur, bal);
        }
    }
    fclose(f);
    fclose(tmp);

    if (found) {
        remove(FILE_NAME);
        rename("temp.dat", FILE_NAME);
        printf("OK: Account %d deleted.\n", account);
    } else {
        remove("temp.dat");
        printf("FAIL: Account or PIN not found.\n");
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        printf("Commands:\n");
        printf("  create <pin> <name>\n");
        printf("  display\n");
        printf("  deposit <account> <pin> <amount>\n");
        printf("  withdraw <account> <pin> <amount>\n");
        printf("  check <account> <pin>\n");
        printf("  delete <account> <pin>\n");
        return 1;
    }

    if (strcmp(argv[1], "create") == 0) {
        if (argc < 4) { printf("ERR: create needs <pin> <name>\n"); return 1; }
        int pin = atoi(argv[2]);
        // join remaining args as name
        char name[200] = "";
        for (int i = 3; i < argc; ++i) {
            strcat(name, argv[i]);
            if (i < argc - 1) strcat(name, " ");
        }
        cmd_create(pin, name);
        return 0;
    } else if (strcmp(argv[1], "display") == 0) {
        cmd_display();
        return 0;
    } else if (strcmp(argv[1], "deposit") == 0) {
        if (argc != 5) { printf("ERR: deposit <account> <pin> <amount>\n"); return 1; }
        int account = atoi(argv[2]);
        int pin = atoi(argv[3]);
        float amt = (float)atof(argv[4]);
        cmd_deposit(account, pin, amt);
        return 0;
    } else if (strcmp(argv[1], "withdraw") == 0) {
        if (argc != 5) { printf("ERR: withdraw <account> <pin> <amount>\n"); return 1; }
        int account = atoi(argv[2]);
        int pin = atoi(argv[3]);
        float amt = (float)atof(argv[4]);
        cmd_withdraw(account, pin, amt);
        return 0;
    } else if (strcmp(argv[1], "check") == 0) {
        if (argc != 4) { printf("ERR: check <account> <pin>\n"); return 1; }
        int account = atoi(argv[2]);
        int pin = atoi(argv[3]);
        cmd_check(account, pin);
        return 0;
    } else if (strcmp(argv[1], "delete") == 0) {
        if (argc != 4) { printf("ERR: delete <account> <pin>\n"); return 1; }
        int account = atoi(argv[2]);
        int pin = atoi(argv[3]);
        cmd_delete(account, pin);
        return 0;
    } else {
        printf("ERR: Unknown command.\n");
        return 1;
    }
}
