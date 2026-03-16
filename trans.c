// Enterprise Secure Banking System (v8.1) - Localized for INR (₹)
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
void bankAnalytics(FILE *fPtr);     
void clearInputBuffer(void);

// Security & UX Prototypes
int authenticateUser(void); 
int verify2FA(struct clientData *client); 
void printHelp(void);                     

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

    while ((choice = mainMenuChoice()) != 5)
    {
        switch (choice)
        {
        case 1: transactionMenu(cfPtr); break;
        case 2: servicesMenu(cfPtr); break;
        case 3: adminMenu(cfPtr); break;
        case 4: printHelp(); break; 
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
    printf(" 1. 2FA SECURITY: Transactions require your PIN and Hash.\n");
    printf(" 2. ACCOUNT TYPES: Savings ('S') earns 4%% interest. \n");
    printf("    Current ('C') has a ₹10,000 overdraft limit.\n");
    printf(" 3. FEES: Bill payments incur a ₹25.00 bank processing fee.\n");
    printf(" 4. FOREX: International transfers automatically convert \n");
    printf("    foreign currencies into INR at current exchange rates.\n");
    printf("==========================================================\n");
    printf("Press Enter to return to the Main Menu...");
    clearInputBuffer(); getchar(); 
}

// ======================= SECURITY MODULES =======================

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
            attempts--; continue;
        }

        printf("[ ACTIVATING BIOMETRIC SENSOR ]\n");
        printf("Simulate thumbprint (Type your registered secret word): ");
        scanf("%14s", inputThumb);

        if (strcmp(inputThumb, client->thumbprint) != 0) {
            printf(">>> BIOMETRIC MATCH FAILED. <<<\n");
            attempts--; continue;
        }
        printf(">>> IDENTITY VERIFIED. ACCESS GRANTED. <<<\n\n");
        return 1; 
    }
    printf("\n>>> SECURITY LOCKOUT: Too many failed attempts. Returning to menu. <<<\n");
    return 0; 
}

int authenticateUser(void) {
    const char correctUsername[] = "admin";
    const char correctPassword[] = "secure123";
    char inputUsername[20], inputPassword[20];
    int attempts = 0;

    printf("\n--- SECURE ADMIN LOGIN REQUIRED ---\n");
    while (attempts < 3) {
        printf("Username: "); scanf("%19s", inputUsername); clearInputBuffer(); 
        printf("Password: "); scanf("%19s", inputPassword); clearInputBuffer();

        if (strcmp(inputUsername, correctUsername) == 0 && strcmp(inputPassword, correctPassword) == 0) {
            printf("\n>>> ACCESS GRANTED. Welcome to the Admin Panel. <<<\n");
            return 1;
        }
        printf("Invalid credentials. Attempt %d/3\n", ++attempts);
    }
    printf("\n>>> SECURITY LOCKOUT. <<<\n");
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
    printf("      VAULTFLOW ENTERPRISE BANKING (v8.1)        \n");
    printf("=================================================\n");
    printf("   1. Financial Transactions\n");
    printf("   2. Account Services & Inquiries\n");
    printf("   3. Administrator Panel\n");
    printf("   4. Help & Instructions\n");
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
        printf(" 2. Transfer Funds (Domestic & International)\n");
        printf(" 3. Pay Utility Bills (₹25.00 Fee Applies)\n");
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
        printf(" 3. View Bank Analytics Dashboard (Macro Report)\n"); 
        printf(" 4. Run Batch Process: Apply 4%% Interest\n");
        printf(" 5. View Pending Customer Service Requests\n");
        printf(" 6. Export Accounts Database (.txt)\n");
        printf(" 7. Logout & Return to Main Menu\n");
        printf("Choice: ");
        if (scanf("%u", &choice) != 1) { clearInputBuffer(); continue; }

        switch(choice) {
            case 1: newRecord(fPtr); break;
            case 2: displayRecords(fPtr); break;
            case 3: bankAnalytics(fPtr); break; 
            case 4: applyInterest(fPtr); break;
            case 5: viewRequests(); break;
            case 6: textFile(fPtr); break;
            case 7: printf(">>> Admin Logged Out. <<<\n"); return; 
            default: printf("Invalid option.\n");
        }
    }
}

// ======================= CORE FUNCTIONS =======================

void bankAnalytics(FILE *fPtr) {
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    int totalAccounts = 0;
    int savingsCount = 0, currentCount = 0;
    double totalLiquidity = 0.0;
    double totalDebt = 0.0;

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr)) {
        if (client.acctNum != 0) {
            totalAccounts++;
            if (client.accountType == 'S') savingsCount++;
            else currentCount++;

            if (client.balance >= 0) totalLiquidity += client.balance;
            else totalDebt += (client.balance * -1); 
        }
    }

    printf("\n=================================================\n");
    printf("         GLOBAL BANK ANALYTICS DASHBOARD         \n");
    printf("=================================================\n");
    printf(" Total Active Accounts : %d\n", totalAccounts);
    printf("   -> Savings Accounts : %d\n", savingsCount);
    printf("   -> Current Accounts : %d\n", currentCount);
    printf("-------------------------------------------------\n");
    printf(" Total Bank Liquidity  : ₹%.2f (Assets)\n", totalLiquidity);
    printf(" Total Outstanding Debt: ₹%.2f (Overdrafts)\n", totalDebt);
    printf("=================================================\n");
}

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
    printf("\n%-6s%-6s%-16s%-11s%12s\n", "Acct", "Type", "Last Name", "First Name", "Balance(₹)");
    printf("----------------------------------------------------\n");
    while (fread(&client, sizeof(struct clientData), 1, readPtr)) {
        if (client.acctNum != 0)
            printf("%-6u%-6c%-16s%-11s%12.2f\n", client.acctNum, client.accountType, client.lastName, client.firstName, client.balance);
    }
    printf("----------------------------------------------------\n");
}

void searchAccount(FILE *fPtr) {
    unsigned int account;
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    printf("\nEnter account number to check details (1-100): ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf(">>> Account #%u is empty. <<<\n", account); return; }
    if (!verify2FA(&client)) return; 

    printf("\n======= ACCOUNT DETAILS =======\n");
    printf("Account Number : %u\n", client.acctNum);
    printf("Account Holder : %s %s\n", client.firstName, client.lastName);
    printf("Account Type   : %s\n", (client.accountType == 'S') ? "Savings" : "Current");
    printf("Current Balance: ₹%.2f\n", client.balance);
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
    if (!verify2FA(&client)) return; 

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
            if (logPtr != NULL) fprintf(logPtr, "Acct: %u | Transaction: +₹%.2f (Interest) | New Balance: ₹%.2f\n", client.acctNum, interest, client.balance);
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
    if (!verify2FA(&client)) return; 

    printf("Current Balance: ₹%.2f. Enter amount (+ deposit, - withdraw): ", client.balance);
    scanf("%lf", &transaction);
    
    // Realistic Indian Checking Account Overdraft Limit
    double limit = (client.accountType == 'C') ? -10000.00 : 0.00; 

    if (client.balance + transaction < limit) {
        printf(">>> DENIED: Insufficient funds. Overdraft limit reached. <<<\n");
    } else {
        client.balance += transaction;
        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Updated successfully. New Balance: ₹%.2f\n", client.balance);
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
    
    printf("Enter Last Name, First Name, Initial Balance (in ₹): ");
    scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

    client.acctNum = account;
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    printf(">>> Account %u created successfully. <<<\n", account);
} 

// UPGRADED FEATURE: FOREX & INTERNATIONAL TRANSFERS (INR BASE)
void transferFunds(FILE *fPtr) {
    unsigned int srcAccount, destAccount, currencyChoice;
    double amount, finalAmount;
    struct clientData srcClient = {0, 0, "", ' ', "", "", 0.0};
    struct clientData destClient = {0, 0, "", ' ', "", "", 0.0};

    printf("\nEnter YOUR account number (Source): ");
    if(scanf("%u", &srcAccount) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (srcAccount - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&srcClient, sizeof(struct clientData), 1, fPtr);
    
    if (srcClient.acctNum == 0) { printf("Source account does not exist.\n"); return; }
    if (!verify2FA(&srcClient)) return; 

    printf("Enter DESTINATION account: ");
    if(scanf("%u", &destAccount) != 1) { clearInputBuffer(); return; }
    if (srcAccount == destAccount) { printf("Cannot transfer to same account.\n"); return; }
    
    fseek(fPtr, (destAccount - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&destClient, sizeof(struct clientData), 1, fPtr);
    if (destClient.acctNum == 0) { printf("Destination account does not exist.\n"); return; }

    printf("\n--- SELECT CURRENCY FOR TRANSFER ---\n");
    printf("1. Indian Rupee (INR) - Standard\n");
    printf("2. US Dollar (USD) - Exchange Rate: 83.00 INR\n");
    printf("3. Euro (EUR) - Exchange Rate: 90.00 INR\n");
    printf("Choice: ");
    scanf("%u", &currencyChoice);

    printf("Enter amount to transfer (in chosen currency): "); 
    scanf("%lf", &amount);

    if (amount <= 0) { printf("Invalid amount.\n"); return; }

    // FOREX Math Conversion (Converting TO INR for deduction)
    if (currencyChoice == 1) finalAmount = amount; // INR
    else if (currencyChoice == 2) finalAmount = amount * 83.00; // USD to INR
    else if (currencyChoice == 3) finalAmount = amount * 90.00; // EUR to INR
    else { printf("Invalid currency.\n"); return; }

    if (currencyChoice != 1) {
        printf(">>> FOREX ALERT: Foreign currency converted to ₹%.2f INR <<<\n", finalAmount);
    }

    double limit = (srcClient.accountType == 'C') ? -10000.00 : 0.00;

    if (srcClient.balance - finalAmount < limit) printf(">>> TRANSFER DENIED: Insufficient funds.\n");
    else {
        srcClient.balance -= finalAmount;
        fseek(fPtr, (srcAccount - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&srcClient, sizeof(struct clientData), 1, fPtr);
        
        destClient.balance += finalAmount;
        fseek(fPtr, (destAccount - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&destClient, sizeof(struct clientData), 1, fPtr);
        
        printf(">>> Transfer successful! New balance: ₹%.2f <<<\n", srcClient.balance);
        FILE *logPtr = fopen("transactions_log.txt", "a");
        if (logPtr != NULL) {
            fprintf(logPtr, "Acct: %u | Transaction: -%.2f (Transfer to %u) | New Balance: %.2f\n", srcAccount, finalAmount, destAccount, srcClient.balance);
            fprintf(logPtr, "Acct: %u | Transaction: +%.2f (Transfer from %u) | New Balance: %.2f\n", destAccount, finalAmount, srcAccount, destClient.balance);
            fclose(logPtr);
        }
    }
}

// UPGRADED FEATURE: BANK REVENUE (TRANSACTION FEES)
void payBill(FILE *fPtr) {
    unsigned int account, billType;
    char billName[20]; double amount;
    double bankFee = 25.00; // Realistic Indian Bank Fee
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};

    printf("\nEnter account number: ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf("Account does not exist.\n"); return; }
    if (!verify2FA(&client)) return; 

    printf("\n1. Electricity  2. Water  \nChoice: "); scanf("%u", &billType);
    if (billType == 1) strcpy(billName, "Electricity Bill");
    else if (billType == 2) strcpy(billName, "Water Bill");
    else { printf("Invalid choice.\n"); return; }

    printf("Enter amount for %s: ₹", billName); scanf("%lf", &amount);
    double limit = (client.accountType == 'C') ? -10000.00 : 0.00;
    double totalDeduction = amount + bankFee;

    printf("Amount: ₹%.2f + Bank Fee: ₹%.2f = Total: ₹%.2f\n", amount, bankFee, totalDeduction);

    if (amount <= 0 || client.balance - totalDeduction < limit) printf(">>> PAYMENT DENIED: Insufficient funds for bill + fee.\n");
    else {
        client.balance -= totalDeduction;
        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf(">>> Paid successfully! <<<\n");
        FILE *logPtr = fopen("transactions_log.txt", "a");
        if (logPtr) { 
            fprintf(logPtr, "Acct: %u | Transaction: -%.2f (Paid: %s + Fee) | New Balance: %.2f\n", client.acctNum, totalDeduction, billName, client.balance); 
            fclose(logPtr); 
        }
    }
}

void requestService(FILE *fPtr) {
    unsigned int account, serviceType;
    struct clientData client = {0, 0, "", ' ', "", "", 0.0};
    
    printf("\n=================================================\n");
    printf("         CUSTOMER SERVICE & SUPPORT DESK         \n");
    printf("=================================================\n");
    printf(" Welcome. Standard Response Time: 24 Business Hours.\n");
    
    printf("\nEnter your account number: ");
    if(scanf("%u", &account) != 1) { clearInputBuffer(); return; }
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) { printf("Account does not exist.\n"); return; }
    if (!verify2FA(&client)) return; 

    printf("\n--- WHAT DO YOU NEED HELP WITH TODAY? ---\n");
    printf(" 1. Request New Checkbook\n");
    printf(" 2. Report Stolen / Lost Debit Card (URGENT)\n");
    printf(" 3. Request Loan Consultation\n");
    printf(" 4. Dispute a Suspicious Transaction\n");
    printf("Choice: ");
    if(scanf("%u", &serviceType) != 1) { clearInputBuffer(); return; }

    FILE *reqPtr = fopen("service_requests.txt", "a");
    if (!reqPtr) return;

    switch(serviceType) {
        case 1: 
            fprintf(reqPtr, "[TICKET] Acct: %u | Type: CHECKBOOK | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf(">>> TICKET LOGGED: A checkbook will be mailed to you. <<<\n"); break;
        case 2: 
            fprintf(reqPtr, "[URGENT] Acct: %u | Type: STOLEN CARD | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf(">>> ALARM TRIGGERED: Card locked. We will call you in 10 minutes! <<<\n"); break;
        case 3: 
            fprintf(reqPtr, "[TICKET] Acct: %u | Type: LOAN CONSULT | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf(">>> TICKET LOGGED: A loan officer will call you tomorrow. <<<\n"); break;
        case 4:
            fprintf(reqPtr, "[URGENT] Acct: %u | Type: FRAUD DISPUTE | Holder: %s %s\n", client.acctNum, client.firstName, client.lastName);
            printf(">>> TICKET LOGGED: A fraud specialist will investigate immediately. <<<\n"); break;
        default: printf(">>> Invalid choice. Request cancelled. <<<\n");
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
    if (!verify2FA(&client)) return; 

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
    if (client.balance < 0) { printf(">>> CANNOT CLOSE: Account overdrawn by ₹%.2f. <<<\n", (client.balance * -1)); return; }

    if (!verify2FA(&client)) return; 

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);
    printf(">>> Account closed. Remaining ₹%.2f dispensed. <<<\n", client.balance);
}