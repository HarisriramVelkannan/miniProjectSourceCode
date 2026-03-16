// Enterprise Secure Banking System (v7.0) - UX & Centralized Security
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h> 

struct clientData {
    unsigned int acctNum; 
    unsigned int pin;     
    char thumbprint[15];  
    char accountType;     
    char lastName[15];    
    char firstName[10];   
    double balance;       
}; 

// Core functionality prototypes
void newRecord(FILE *fPtr);
void updateRecord(FILE *fPtr);
void transferFunds(FILE *fPtr);
void payBill(FILE *fPtr);           
void miniStatement(FILE *fPtr);
void searchAccount(FILE *fPtr); 
void changePin(FILE *fPtr);
void requestService(FILE *fPtr);    
void deleteRecord(FILE *fPtr);
void displayRecords(FILE *readPtr); 
void viewRequests(void);            
void applyInterest(FILE *fPtr);
void textFile(FILE *readPtr);
void clearInputBuffer(void);

// Security & UX Prototypes
int authenticateUser(void); 
int verify2FA(struct clientData *client); // NEW: Centralized 3-Attempt Security
void printHelp(void);                     // NEW: User Instructions Menu

// Sub-menu routing prototypes
unsigned int mainMenuChoice(void);
void transactionMenu(FILE *fPtr);
void servicesMenu(FILE *fPtr);
void adminMenu(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         
    unsigned int choice; 

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        printf("Creating new secure database...\n");
        cfPtr = fopen("credit.dat", "wb+");
        if (cfPtr == NULL) {
            printf("Fatal Error: Cannot create database.\n");
            exit(-1);
        }
        struct clientData blankClient = {0, 0, "", ' ', "", "", 0.0};
        for (int i = 0; i < 100; ++i) {
            fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
        }
        printf("System initialized with 100 blank records.\n");
    }

    // MAIN MENU LOOP
    while ((choice = mainMenuChoice()) != 5)
    {
        switch (choice)
        {
        case 1: transactionMenu(cfPtr); break;
        case 2: servicesMenu(cfPtr); break;
        case 3: adminMenu(cfPtr); break;
        case 4: printHelp(); break; // NEW: Help Menu routed here
        default: puts("Incorrect choice. Please try again."); break;
        } 
    }     

    fclose(cfPtr); 
    printf("\n>>> System safely shut down. Goodbye! <<<\n");
    return 0;
} 

// ======================= UX & HELP MODULE =======================

void printHelp(void) {
    printf("\n==========================================================\n");
    printf("           HOW TO USE THIS BANKING SYSTEM                 \n");
    printf("==========================================================\n");
    printf(" WELCOME! If you are new, here is how things work:\n\n");
    printf(" 1. GETTING AN ACCOUNT: Only Bank Administrators can\n");
    printf("    create accounts. Go to the Admin Panel (Option 3)\n");
    printf("    and log in to open one.\n\n");
    printf(" 2. TWO-FACTOR AUTHENTICATION (2FA): To protect your\n");
    printf("    money, every transaction requires two things:\n");
    printf("      - Your 4-digit PIN (e.g., 1234)\n");
    printf("      - Your Biometric Hash (a secret word you pick\n");
    printf("        during account creation, like 'THUMB99').\n\n");
    printf(" 3. ACCOUNT TYPES:\n");
    printf("      - Savings (S): Cannot be overdrawn. Earns 4%% interest.\n");
    printf("      - Current (C): Does not earn interest. Has a $500\n");
    printf("        overdraft limit for emergency spending.\n\n");
    printf(" 4. SECURITY LOCKOUT: If you enter the wrong PIN or\n");
    printf("    Hash 3 times, the system will temporarily block you.\n");
    printf("==========================================================\n");
    printf("Press Enter to return to the Main Menu...");
    clearInputBuffer(); 
    getchar(); // Pauses until the user presses Enter
}

// ======================= SECURITY MODULES =======================

// Centralized 2FA checker - Reduces redundant code and adds 3-try lockout!
int verify2FA(struct clientData *client) {
    unsigned int inputPin;
    char inputThumb[15];
    int attempts = 3;

    while (attempts > 0) {
        printf("\n--- IDENTITY VERIFICATION (%d attempts remaining) ---\n", attempts);
        printf("Enter your 4-digit PIN: ");
        if (scanf("%u", &inputPin) != 1) { clearInputBuffer(); attempts--; continue; }

        if (inputPin != client->pin) {
            printf(">>> INCORRECT PIN. <<<\n");
            attempts--;
            continue;
        }

        printf("[ ACTIVATING BIOMETRIC SENSOR ]\n");
        printf("Simulate thumbprint (Type your registered secret word): ");
        scanf("%14s", inputThumb);

        if (strcmp(inputThumb, client->thumbprint) != 0) {
            printf(">>> BIOMETRIC MATCH FAILED. <<<\n");
            attempts--;
            continue;
        }

        printf(">>> IDENTITY VERIFIED. ACCESS GRANTED. <<<\n\n");
        return 1; // Success
    }
    printf("\n>>> SECURITY LOCKOUT: Too many failed attempts. Returning to menu. <<<\n");
    return 0; // Fail
}

int authenticateUser(void) {
    const char correctUsername[] = "admin";
    const char correctPassword[] = "secure123";
    char inputUsername[20], inputPassword[20];
    int attempts = 0;

    printf("\n--- SECURE ADMIN LOGIN REQUIRED ---\n");
    while (attempts < 3) {
        printf("Username: ");
        scanf("%19s", inputUsername); clearInputBuffer(); 
        printf("Password: ");
        scanf("%19s", inputPassword); clearInputBuffer();

        if (strcmp(inputUsername, correctUsername) == 0 && strcmp(inputPassword, correctPassword) == 0) {
            printf("\n>>> ACCESS GRANTED. Welcome to the Admin Panel. <<<\n");
            return 1;
        }
        printf("Invalid credentials. Attempt %d/3\n", ++attempts);
    }
    printf("\n>>> SECURITY LOCKOUT. Returning to Main Menu. <<<\n");
    return 0; 
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ======================= MENU ROUTERS =======================

unsigned int mainMenuChoice(void) {
    unsigned int choice; 
    printf("\n=================================================\n");
    printf("     ENTERPRISE SECURE BANKING SYSTEM (v7.0)     \n");
    printf("=================================================\n");
    printf("   1. Financial Transactions\n");
    printf("   2. Account Services & Inquiries\n");
    printf("   3. Administrator Panel\n");
    printf("   4. Help & Instructions (New Users Start Here!)\n");
    printf("   5. Exit System\n");
    printf("=================================================\n");
    printf("Select Category: ");
    if (scanf("%u", &choice) != 1) { clearInputBuffer(); return 0; }
    return choice;
}

void transactionMenu(FILE *fPtr) {
    unsigned int choice;
    while(1) {
        printf("\n--- [ FINANCIAL TRANSACTIONS ] ---\n");
        printf(" 1. Deposit / Withdraw Funds\n");
        printf(" 2. Transfer Funds to Another Account\n");
        printf(" 3. Pay Utility Bills\n");
        printf(" 4. Return to Main Menu\n");
        printf("Choice: ");
        if (scanf("%u", &choice) != 1) { clearInputBuffer(); continue; }

        switch(choice) {
            case 1: updateRecord(fPtr); break;
            case 2: transferFunds(fPtr); break;
            case 3: payBill(fPtr); break;
            case 4: return; 
            default: printf("Invalid option.\n");
        }
    }
}

void servicesMenu(FILE *fPtr) {
    unsigned int choice;
    while(1) {
        printf("\n--- [ ACCOUNT SERVICES & INQUIRIES ] ---\n");
        printf(" 1. Check Balance & Account Details\n");
        printf(" 2. Print Mini-Statement\n");
        printf(" 3. Change Account PIN\n");
        printf(" 4. Customer Service Desk (Requests)\n");
        printf(" 5. Close Account\n");
        printf(" 6. Return to Main Menu\n");
        printf("Choice: ");
        if (scanf("%u", &choice) != 1) { clearInputBuffer(); continue; }

        switch(choice) {
            case 1: searchAccount(fPtr); break;
            case 2: miniStatement(fPtr); break;
            case 3: changePin(fPtr); break;
            case 4: requestService(fPtr); break;
            case 5: deleteRecord(fPtr); break;
            case 6: return; 
            default: printf("Invalid option.\n");
        }
    }
}

void adminMenu(FILE *fPtr) {
    if (authenticateUser() == 0) return; 

    unsigned int choice;
    while(1) {
        printf("\n--- [ ADMINISTRATOR PANEL ] ---\n");
        printf(" 1. Open New Account\n");
        printf(" 2. View All Active Accounts\n");
        printf(" 3. Run Batch Process: Apply 4%% Interest\n");
        printf(" 4. View Pending Customer Service Requests\n");
        printf(" 5. Export Accounts Database (.txt)\n");
        printf(" 6. Logout & Return to Main Menu\n");
        printf("Choice: ");
        if (scanf("%u", &choice) != 1) { clearInputBuffer(); continue; }

        switch(choice) {
            case 1: newRecord(fPtr); break;
            case 2: displayRecords(fPtr); break;
            case 3: applyInterest(fPtr); break;
            case 4: viewRequests(); break;
            case 5: textFile(fPtr); break;
            case 6: printf(">>> Admin Logged Out. <<<\n"); return; 
            default: printf("Invalid option.\n");
        }
    }
}

// ======================= CORE FUNCTIONS =======================

void textFile(FILE *readPtr) {
    FILE *writePtr; 
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    if ((writePtr = fopen("accounts.txt", "w")) == NULL) puts("File error.");
    else {
        rewind(readPtr);
        fprintf(writePtr, "%-6s%-6s%-16s%-11s%10s\n", "Acct", "Type", "Last Name", "First Name", "Balance");
        while (fread(&client, sizeof(struct clientData), 1, readPtr)) {
            if (client.acctNum != 0)
                fprintf(writePtr, "%-6u%-6c%-16s%-11s%10.2f\n", client.acctNum, client.accountType, client.lastName, client.firstName, client.balance);
        }
        fclose(writePtr);
        printf(">>> Export to accounts.txt complete. <<<\n");
    }
}

void displayRecords(FILE *readPtr) {
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    rewind(readPtr); 
    printf("\n%-6s%-6s%-16s%-11s%10s\n", "Acct", "Type", "Last Name", "First Name", "Balance");
    printf("--------------------------------------------------\n");
    while (fread(&client, sizeof(struct clientData), 1, readPtr)) {
        if (client.acctNum != 0)
            printf("%-6u%-6c%-16s%-11s%10.2f\n", client.acctNum, client.accountType, client.lastName, client.firstName, client.balance);
    }
    printf("--------------------------------------------------\n");
}

void searchAccount(FILE *fPtr) {
    unsigned int account;
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    printf("\nEnter account number to check details (1-100): ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf(">>> Account #%u is empty. <<<\n", account); return; }

    // Privacy upgrade: 2FA required to view balance now!
    if (!verify2FA(&client)) return; 

    printf("\n======= ACCOUNT DETAILS =======\n");
    printf("Account Number : %u\n", client.acctNum);
    printf("Account Holder : %s %s\n", client.firstName, client.lastName);
    printf("Account Type   : %s\n", (client.accountType == 'S') ? "Savings" : "Current");
    printf("Current Balance: $%.2f\n", client.balance);
    printf("===============================\n");
}

void miniStatement(FILE *fPtr) {
    unsigned int account;
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    char searchStr[30], line[150];
    int found = 0;

    printf("\nEnter account number: ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf("Account does not exist.\n"); return; }
    if (!verify2FA(&client)) return; // Centralized Security Call!

    FILE *logPtr = fopen("transactions_log.txt", "r");
    if (logPtr == NULL) { printf(">>> No transactions found. <<<\n"); return; }

    printf("\n======= MINI STATEMENT FOR ACCT %u =======\n", account);
    sprintf(searchStr, "Acct: %u |", account);
    while (fgets(line, sizeof(line), logPtr) != NULL) {
        if (strstr(line, searchStr) != NULL) { printf("%s", line); found = 1; }
    }
    if (!found) printf("No recent transaction history.\n");
    printf("==========================================\n");
    fclose(logPtr);
}

void applyInterest(FILE *fPtr) {
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    int count = 0;
    FILE *logPtr = fopen("transactions_log.txt", "a");

    rewind(fPtr); 
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0 && client.accountType == 'S' && client.balance > 0) {
            double interest = client.balance * 0.04; 
            client.balance += interest;
            fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
            fseek(fPtr, 0, SEEK_CUR); 
            if (logPtr != NULL) fprintf(logPtr, "Acct: %u | Transaction: +%.2f (Interest) | New Balance: %.2f\n", client.acctNum, interest, client.balance);
            count++;
        }
    }
    if (logPtr != NULL) fclose(logPtr);
    printf("\n>>> BATCH SUCCESS: 4%% Annual Interest applied to %d Savings accounts! <<<\n", count);
}

void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;   
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};

    printf("\nEnter account to update (1-100): ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf("Account does not exist.\n"); return; }
    if (!verify2FA(&client)) return; // Centralized Security Call!

    printf("Current Balance: $%.2f. Enter amount (+ deposit, - withdraw): ", client.balance);
    scanf("%lf", &transaction);
    double limit = (client.accountType == 'C') ? -500.00 : 0.00;

    if (client.balance + transaction < limit) {
        printf(">>> DENIED: Insufficient funds. Overdraft limit reached. <<<\n");
    } else {
        client.balance += transaction;
        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Updated successfully. New Balance: $%.2f\n", client.balance);
        FILE *logPtr = fopen("transactions_log.txt", "a");
        if (logPtr != NULL) {
            fprintf(logPtr, "Acct: %u | Transaction: %+.2f | New Balance: %.2f\n", client.acctNum, transaction, client.balance);
            fclose(logPtr);
        }
    }
}

void newRecord(FILE *fPtr) {
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    unsigned int account; 
    
    printf("\nAssign Account Number (1-100): ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum != 0) { printf("Account already exists.\n"); return; }

    printf("Select Account Type ('S' for Savings, 'C' for Current): ");
    scanf(" %c", &client.accountType);
    client.accountType = toupper(client.accountType); 
    if (client.accountType != 'S' && client.accountType != 'C') { printf("Invalid Type.\n"); return; }

    printf("Set a 4-digit PIN: "); scanf("%u", &client.pin);
    printf("Register Biometric Hash (Pick a secret word, e.g., 'THUMB01'): "); 
    scanf("%14s", client.thumbprint);
    
    printf("Enter Last Name, First Name, Initial Balance: ");
    scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

    client.acctNum = account;
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    printf(">>> Account %u created successfully. <<<\n", account);
} 

void transferFunds(FILE *fPtr) {
    unsigned int srcAccount, destAccount;
    double amount;
    struct clientData srcClient = {0, 0, "", ' ', "", "", 0.0};
    struct clientData destClient = {0, 0, "", ' ', "", "", 0.0};

    printf("\nEnter YOUR account number (Source): ");
    if(scanf("%u", &srcAccount) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (srcAccount - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&srcClient, sizeof(struct clientData), 1, fPtr);
    
    if (srcClient.acctNum == 0) { printf("Source account does not exist.\n"); return; }
    if (!verify2FA(&srcClient)) return; // Centralized Security Call!

    printf("Enter DESTINATION account: ");
    if(scanf("%u", &destAccount) != 1) { clearInputBuffer(); return; }
    if (srcAccount == destAccount) { printf("Cannot transfer to same account.\n"); return; }
    
    fseek(fPtr, (destAccount - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&destClient, sizeof(struct clientData), 1, fPtr);
    if (destClient.acctNum == 0) { printf("Destination account does not exist.\n"); return; }

    printf("Balance: $%.2f. Amount to transfer: ", srcClient.balance); scanf("%lf", &amount);
    double limit = (srcClient.accountType == 'C') ? -500.00 : 0.00;

    if (amount <= 0) printf("Invalid amount.\n");
    else if (srcClient.balance - amount < limit) printf(">>> TRANSFER DENIED: Insufficient funds.\n");
    else {
        srcClient.balance -= amount;
        fseek(fPtr, (srcAccount - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&srcClient, sizeof(struct clientData), 1, fPtr);
        destClient.balance += amount;
        fseek(fPtr, (destAccount - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&destClient, sizeof(struct clientData), 1, fPtr);
        
        printf(">>> Transfer successful! New balance: $%.2f <<<\n", srcClient.balance);
        FILE *logPtr = fopen("transactions_log.txt", "a");
        if (logPtr != NULL) {
            fprintf(logPtr, "Acct: %u | Transaction: -%.2f (Transfer to %u) | New Balance: %.2f\n", srcAccount, amount, destAccount, srcClient.balance);
            fprintf(logPtr, "Acct: %u | Transaction: +%.2f (Transfer from %u) | New Balance: %.2f\n", destAccount, amount, srcAccount, destClient.balance);
            fclose(logPtr);
        }
    }
}

void payBill(FILE *fPtr) {
    unsigned int account, billType;
    char billName[20]; double amount;
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};

    printf("\nEnter account number: ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf("Account does not exist.\n"); return; }
    if (!verify2FA(&client)) return; // Centralized Security Call!

    printf("\n1. Electricity  2. Water  \nChoice: "); scanf("%u", &billType);
    if (billType == 1) strcpy(billName, "Electricity Bill");
    else if (billType == 2) strcpy(billName, "Water Bill");
    else { printf("Invalid choice.\n"); return; }

    printf("Enter amount for %s: ", billName); scanf("%lf", &amount);
    double limit = (client.accountType == 'C') ? -500.00 : 0.00;

    if (amount <= 0 || client.balance - amount < limit) printf(">>> PAYMENT DENIED.\n");
    else {
        client.balance -= amount;
        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf(">>> Paid $%.2f successfully! <<<\n", amount);
        FILE *logPtr = fopen("transactions_log.txt", "a");
        if (logPtr) { fprintf(logPtr, "Acct: %u | Transaction: -%.2f (Paid: %s) | New Balance: %.2f\n", client.acctNum, amount, billName, client.balance); fclose(logPtr); }
    }
}
// ======================= UPGRADED CUSTOMER SERVICE DESK =======================
void requestService(FILE *fPtr) {
    unsigned int account, serviceType;
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    
    // 1. User-Friendly Instructions & Welcome Screen
    printf("\n=================================================\n");
    printf("         CUSTOMER SERVICE & SUPPORT DESK         \n");
    printf("=================================================\n");
    printf(" Welcome to the 24/7 Support Desk. Submitting a\n");
    printf(" request here will log a secure ticket with our\n");
    printf(" human representatives. \n");
    printf(" Standard Response Time: 24 Business Hours.\n");
    printf("-------------------------------------------------\n");

    printf("\nEnter your account number to begin: ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf(">>> ERROR: Account does not exist. <<<\n"); return; }
    
    // 2. Centralized Security Call
    if (!verify2FA(&client)) return; 

    // 3. Expanded Service Menu
    printf("\n--- WHAT DO YOU NEED HELP WITH TODAY? ---\n");
    printf(" 1. Request New Checkbook (Standard Mail)\n");
    printf(" 2. Report Stolen / Lost Debit Card (URGENT)\n");
    printf(" 3. Request Loan Consultation (Auto / Home / Personal)\n");
    printf(" 4. Update Personal Details (Address / Phone)\n");
    printf(" 5. Dispute a Suspicious Transaction (Fraud Alert)\n");
    printf(" 6. Cancel & Return to Main Menu\n");
    printf("Choice: ");
    if(scanf("%u", &serviceType) != 1) { clearInputBuffer(); return; }

    if (serviceType == 6) {
        printf("\n>>> Request cancelled. Returning to menu. <<<\n");
        return;
    }

    FILE *reqPtr = fopen("service_requests.txt", "a");
    if (!reqPtr) {
        printf("\n>>> ERROR: Support database is currently offline. Please try again later. <<<\n");
        return;
    }

    // 4. Professional Routing & Clear Confirmation Messages
    switch(serviceType) {
        case 1: 
            fprintf(reqPtr, "[TICKET] Acct: %u | Type: CHECKBOOK | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf("\n>>> TICKET LOGGED: A new checkbook will be mailed to your registered address in 3-5 business days. <<<\n");
            break;
        case 2: 
            fprintf(reqPtr, "[URGENT] Acct: %u | Type: STOLEN CARD | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf("\n>>> ALARM TRIGGERED: Your card is instantly locked. Our fraud team will call you within 10 minutes! <<<\n");
            break;
        case 3: 
            fprintf(reqPtr, "[TICKET] Acct: %u | Type: LOAN CONSULT | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf("\n>>> TICKET LOGGED: A dedicated loan officer will call you tomorrow between 9 AM and 5 PM. <<<\n");
            break;
        case 4:
            fprintf(reqPtr, "[TICKET] Acct: %u | Type: INFO UPDATE | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf("\n>>> TICKET LOGGED: For security reasons, an agent will contact you by phone to update your address/phone number. <<<\n");
            break;
        case 5:
            fprintf(reqPtr, "[URGENT] Acct: %u | Type: FRAUD DISPUTE | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf("\n>>> TICKET LOGGED: Your account is flagged for review. A fraud specialist will investigate your recent transactions. <<<\n");
            break;
        default:
            printf("\n>>> Invalid choice. Request cancelled. <<<\n");
    }
    fclose(reqPtr);
}
void viewRequests(void) {
    char line[150];
    FILE *reqPtr = fopen("service_requests.txt", "r");
    if (!reqPtr) { printf("\n>>> No pending requests. <<<\n"); return; }
    printf("\n======= PENDING REQUESTS =======\n");
    while (fgets(line, sizeof(line), reqPtr)) printf("%s", line);
    printf("================================\n");
    fclose(reqPtr);
}

void changePin(FILE *fPtr) {
    unsigned int account, newPin; 
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    
    printf("\nEnter account number: ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf("Account does not exist.\n"); return; }
    if (!verify2FA(&client)) return; // Centralized Security Call!

    printf("Enter NEW 4-digit PIN: "); scanf("%u", &newPin);
    client.pin = newPin;
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    printf(">>> PIN changed successfully. <<<\n");
}

void deleteRecord(FILE *fPtr) {
    struct clientData client;                                       
    struct clientData blank = {0, 0, "", ' ', "", "", 0}; 
    unsigned int account; 
    
    printf("\nEnter account to close: ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) { printf("Account does not exist.\n"); return; }
    if (client.balance < 0) { printf(">>> CANNOT CLOSE: Account overdrawn by $%.2f. <<<\n", (client.balance * -1)); return; }

    if (!verify2FA(&client)) return; // Centralized Security Call!

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);
    printf(">>> Account closed. Remaining $%.2f dispensed. <<<\n", client.balance);
}