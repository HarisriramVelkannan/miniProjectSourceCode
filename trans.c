// VaultFlow Enterprise Banking System (FINAL: Mutex, Hash, QuickSort)
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h> 
#include <windows.h> // PHASE 2: OS-Level Concurrency & Mutex Locks

// UI COLOR CODES
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define CYAN "\x1b[36m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

// GLOBAL OS MUTEX LOCK
HANDLE dbMutex;

// DATABASE SCHEMAS
struct clientData {
    unsigned int acctNum; 
    unsigned int pin;     // Irreversible Hash
    char thumbprint[15];  
    char accountType;     
    char lastName[15];    
    char firstName[10];   
    double balance;       
}; 

struct biometricData {
    unsigned int acctNum;      
    char secureHash[65];       
    int minutiaePoints;        
    char status[10];           
};

struct TransactionNode {
    char logData[150];
    struct TransactionNode *next;
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
void exportToExcel(FILE *readPtr);  
void bankAnalytics(FILE *fPtr);     
void clearInputBuffer(void);

// Advanced Enterprise Prototypes
void unlockAccount(void);           
void displayTopAccounts(FILE *fPtr);
void swapAccounts(struct clientData* a, struct clientData* b);
int partition(struct clientData arr[], int low, int high);
void quickSort(struct clientData arr[], int low, int high);
unsigned int secureHashPIN(unsigned int pin); // PHASE 3: Hashing Algorithm
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

    // Initialize OS Mutex Lock for Concurrency Protection
    dbMutex = CreateMutex(NULL, FALSE, "VaultFlowDBMutex");

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        printf(RED "FATAL ERROR: credit.dat missing. Run db_seeder.c first." RESET "\n");
        return -1;
    }
    FILE *bioCheck = fopen("biometrics.dat", "rb");
    if (bioCheck == NULL) {
        printf(RED "FATAL ERROR: biometrics.dat missing. Run bio_db_setup.c first." RESET "\n");
        return -1;
    }
    fclose(bioCheck);

    while ((choice = mainMenuChoice()) != 5) {
        switch (choice) {
            case 1: transactionMenu(cfPtr); break;
            case 2: servicesMenu(cfPtr); break;
            case 3: adminMenu(cfPtr); break;
            case 4: printHelp(); break; 
            default: puts(RED "Incorrect choice. Please try again." RESET); break;
        } 
    }     

    fclose(cfPtr); 
    CloseHandle(dbMutex); // Release Mutex to OS
    printf(GREEN "\n>>> System safely shut down. Goodbye! <<<" RESET "\n");
    return 0;
} 

void printHelp(void) {
    printf(CYAN "\n==========================================================\n" RESET);
    printf("           VAULTFLOW ENTERPRISE ARCHITECTURE              \n");
    printf(CYAN "==========================================================\n" RESET);
    printf(" 1. CONCURRENCY: Financial transactions are protected by\n");
    printf("    Windows OS Mutex Locks to prevent Race Conditions.\n");
    printf(" 2. CRYPTOGRAPHY: PINs are mathematically secured using\n");
    printf("    the DJB2 One-Way Hashing Algorithm.\n");
    printf(" 3. BIG DATA: Leaderboards use an O(N log N) Quick Sort.\n");
    printf(CYAN "==========================================================\n" RESET);
    printf("Press Enter to return to the Main Menu...");
    clearInputBuffer(); getchar(); 
}

// ======================= SECURITY MODULES =======================

unsigned int secureHashPIN(unsigned int pin) {
    unsigned int hash = 5381;
    char pinStr[10];
    sprintf(pinStr, "%u", pin); 
    for (int i = 0; pinStr[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + pinStr[i]; 
    }
    return hash;
}

int verify2FA(struct clientData *client) {
    unsigned int inputPin, hashedInput;
    char inputThumb[65];
    int attempts = 3;
    struct biometricData bioRecord;

    FILE *bioPtr = fopen("biometrics.dat", "rb+");
    if (bioPtr == NULL) return 0;

    fseek(bioPtr, (client->acctNum - 1) * sizeof(struct biometricData), SEEK_SET);
    fread(&bioRecord, sizeof(struct biometricData), 1, bioPtr);

    if (strcmp(bioRecord.status, "ACTIVE") != 0) {
         printf(RED "\n>>> CRITICAL: Account is LOCKED due to fraud. Please see a Bank Admin. <<<" RESET "\n");
         fclose(bioPtr); return 0;
    }

    while (attempts > 0) {
        printf(YELLOW "\n--- IDENTITY VERIFICATION (%d attempts remaining) ---" RESET "\n", attempts);
        printf("Enter your 4-digit PIN: ");
        if (scanf("%u", &inputPin) != 1) { clearInputBuffer(); attempts--; continue; }

        // PHASE 3: Hash user input and compare to the hash stored on the hard drive
        hashedInput = secureHashPIN(inputPin);

        if (hashedInput != client->pin) {
            printf(RED ">>> INCORRECT PIN. <<<" RESET "\n");
            attempts--; continue;
        }

        printf(CYAN "[ ACTIVATING BIOMETRIC SENSOR ]\n" RESET);
        printf("Simulate thumbprint (Enter Secure Hash): ");
        scanf("%64s", inputThumb);

        if (strcmp(inputThumb, bioRecord.secureHash) != 0) {
            printf(RED ">>> BIOMETRIC MATCH FAILED. <<<" RESET "\n");
            attempts--; continue;
        }
        
        printf(GREEN ">>> IDENTITY VERIFIED. ACCESS GRANTED. <<<" RESET "\n\n");
        fclose(bioPtr); return 1; 
    }

    printf(RED "\n>>> SECURITY BREACH DETECTED: Permanently Locking Account. <<<" RESET "\n");
    strcpy(bioRecord.status, "LOCKED");
    fseek(bioPtr, (client->acctNum - 1) * sizeof(struct biometricData), SEEK_SET);
    fwrite(&bioRecord, sizeof(struct biometricData), 1, bioPtr);
    fclose(bioPtr);
    return 0; 
}

int authenticateUser(void) {
    const char correctUsername[] = "admin";
    const char correctPassword[] = "secure123";
    char inputUsername[20], inputPassword[20];
    int attempts = 0;

    printf(YELLOW "\n--- SECURE ADMIN LOGIN REQUIRED ---" RESET "\n");
    while (attempts < 3) {
        printf("Username: "); scanf("%19s", inputUsername); clearInputBuffer(); 
        printf("Password: "); scanf("%19s", inputPassword); clearInputBuffer();

        if (strcmp(inputUsername, correctUsername) == 0 && strcmp(inputPassword, correctPassword) == 0) {
            printf(GREEN "\n>>> ACCESS GRANTED. Welcome to the Admin Panel. <<<" RESET "\n");
            return 1;
        }
        printf(RED "Invalid credentials. Attempt %d/3\n" RESET, ++attempts);
    }
    printf(RED "\n>>> SECURITY LOCKOUT. <<<" RESET "\n");
    return 0; 
}

void clearInputBuffer(void) { int c; while ((c = getchar()) != '\n' && c != EOF); }

// ======================= MENU ROUTERS =======================
unsigned int mainMenuChoice(void) {
    unsigned int choice; 
    printf(CYAN "\n=================================================\n" RESET);
    printf("      VAULTFLOW ENTERPRISE BANKING (v11.0)       \n");
    printf(CYAN "=================================================\n" RESET);
    printf("   1. Financial Transactions\n");
    printf("   2. Account Services & Inquiries\n");
    printf("   3. Administrator Panel\n");
    printf("   4. Help & Instructions\n");
    printf("   5. Exit System\n");
    printf(CYAN "=================================================\n" RESET);
    printf("Select Category: ");
    if (scanf("%u", &choice) != 1) { clearInputBuffer(); return 0; }
    return choice;
}

void transactionMenu(FILE *fPtr) {
    unsigned int choice;
    while(1) {
        printf(CYAN "\n--- [ FINANCIAL TRANSACTIONS ] ---" RESET "\n");
        printf(" 1. Deposit / Withdraw Funds\n");
        printf(" 2. Transfer Funds (Domestic & International)\n");
        printf(" 3. Pay Utility Bills\n");
        printf(" 4. Return to Main Menu\n");
        printf("Choice: ");
        if (scanf("%u", &choice) != 1) { clearInputBuffer(); continue; }

        switch(choice) {
            case 1: updateRecord(fPtr); break;
            case 2: transferFunds(fPtr); break;
            case 3: payBill(fPtr); break;
            case 4: return; 
            default: printf(RED "Invalid option.\n" RESET);
        }
    }
}

void servicesMenu(FILE *fPtr) {
    unsigned int choice;
    while(1) {
        printf(CYAN "\n--- [ ACCOUNT SERVICES & INQUIRIES ] ---" RESET "\n");
        printf(" 1. Check Balance & Account Details\n");
        printf(" 2. Print Mini-Statement (Linked List Memory)\n");
        printf(" 3. Change Account PIN\n");
        printf(" 4. Customer Service Desk (Auto-Loans & Support)\n");
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
            default: printf(RED "Invalid option.\n" RESET);
        }
    }
}

void adminMenu(FILE *fPtr) {
    if (authenticateUser() == 0) return; 
    unsigned int choice;
    while(1) {
        printf(CYAN "\n--- [ ADMINISTRATOR PANEL ] ---" RESET "\n");
        printf(" 1. Open New Account\n");
        printf(" 2. View All Active Accounts\n");
        printf(" 3. View Bank Analytics Dashboard\n"); 
        printf(" 4. View Top 10 Wealthiest Accounts (Quick Sort)\n"); 
        printf(" 5. Run Batch Process: Apply 4%% Interest\n");
        printf(" 6. View Pending Customer Service Requests\n");
        printf(" 7. Unlock Frozen Account (Security Override)\n");     
        printf(" 8. Export Data to Microsoft Excel (.csv)\n");
        printf(" 9. Logout & Return to Main Menu\n");
        printf("Choice: ");
        if (scanf("%u", &choice) != 1) { clearInputBuffer(); continue; }

        switch(choice) {
            case 1: newRecord(fPtr); break;
            case 2: displayRecords(fPtr); break;
            case 3: bankAnalytics(fPtr); break; 
            case 4: displayTopAccounts(fPtr); break; 
            case 5: applyInterest(fPtr); break;
            case 6: viewRequests(); break;
            case 7: unlockAccount(); break;
            case 8: exportToExcel(fPtr); break;
            case 9: printf(GREEN ">>> Admin Logged Out. <<<" RESET "\n"); return; 
            default: printf(RED "Invalid option.\n" RESET);
        }
    }
}

// ======================= ADVANCED TECH & DSA MODULE =======================

void swapAccounts(struct clientData* a, struct clientData* b) {
    struct clientData t = *a; *a = *b; *b = t;
}

int partition(struct clientData arr[], int low, int high) {
    double pivot = arr[high].balance; 
    int i = (low - 1); 
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].balance >= pivot) {
            i++; swapAccounts(&arr[i], &arr[j]);
        }
    }
    swapAccounts(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(struct clientData arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high); 
        quickSort(arr, low, pi - 1);        
        quickSort(arr, pi + 1, high);       
    }
}

void displayTopAccounts(FILE *fPtr) {
    struct clientData accounts[100]; int count = 0;
    rewind(fPtr);
    while (fread(&accounts[count], sizeof(struct clientData), 1, fPtr)) {
        if (accounts[count].acctNum != 0) count++;
    }
    if (count == 0) { printf(YELLOW ">>> No accounts found in database. <<<" RESET "\n"); return; }
    
    quickSort(accounts, 0, count - 1);

    printf(CYAN "\n=================================================\n" RESET);
    printf("        TOP WEALTHIEST ACCOUNTS LEADERBOARD      \n");
    printf(CYAN "=================================================\n" RESET);
    for(int i = 0; i < count && i < 10; i++) {
        printf(" #%d | Acct %u | %s %s | Rs. %.2f\n", 
               i+1, accounts[i].acctNum, accounts[i].firstName, accounts[i].lastName, accounts[i].balance);
    }
    printf(CYAN "=================================================\n" RESET);
}

void unlockAccount(void) {
    unsigned int targetAccount; struct biometricData bioRecord;
    printf("\nEnter Account Number to unlock: ");
    if(scanf("%u", &targetAccount) != 1) { clearInputBuffer(); return; }

    FILE *bioPtr = fopen("biometrics.dat", "rb+");
    if (!bioPtr) return;

    fseek(bioPtr, (targetAccount - 1) * sizeof(struct biometricData), SEEK_SET);
    fread(&bioRecord, sizeof(struct biometricData), 1, bioPtr);

    if (bioRecord.acctNum == 0) printf(YELLOW ">>> Biometric record does not exist. <<<" RESET "\n");
    else if (strcmp(bioRecord.status, "ACTIVE") == 0) printf(YELLOW ">>> Account is already ACTIVE. <<<" RESET "\n");
    else {
        strcpy(bioRecord.status, "ACTIVE");
        fseek(bioPtr, (targetAccount - 1) * sizeof(struct biometricData), SEEK_SET);
        fwrite(&bioRecord, sizeof(struct biometricData), 1, bioPtr);
        printf(GREEN ">>> SUCCESS: Account %u has been UNLOCKED. <<<" RESET "\n", targetAccount);
    }
    fclose(bioPtr);
}

// ======================= CORE BUSINESS LOGIC =======================

void updateRecord(FILE *fPtr) {
    unsigned int account; double transaction; struct clientData client = {0};
    printf("\nEnter account: "); scanf("%u", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0 || !verify2FA(&client)) return; 
    
    printf("Amount (+ deposit, - withdraw): "); scanf("%lf", &transaction);
    double limit = (client.accountType == 'C') ? -10000.00 : 0.00; 
    if (client.balance + transaction < limit) {
        printf(RED ">>> DENIED: Overdraft Limit Exceeded. <<<" RESET "\n"); return;
    }

    // PHASE 2: MUTEX LOCK INITIATED FOR SAFE FILE WRITE
    WaitForSingleObject(dbMutex, INFINITE); 
    client.balance += transaction;
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    ReleaseMutex(dbMutex); 

    printf(GREEN "Updated! New Balance: Rs. %.2f\n" RESET, client.balance);
    FILE *logPtr = fopen("transactions_log.txt", "a");
    if (logPtr) { fprintf(logPtr, "Acct: %u | Transaction: %+.2f | New Balance: %.2f\n", client.acctNum, transaction, client.balance); fclose(logPtr); }
}

void transferFunds(FILE *fPtr) {
    unsigned int srcAccount, destAccount; double amount;
    struct clientData srcClient = {0}, destClient = {0};
    printf("\nSource account: "); scanf("%u", &srcAccount);
    fseek(fPtr, (srcAccount - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&srcClient, sizeof(struct clientData), 1, fPtr);
    if (srcClient.acctNum == 0 || !verify2FA(&srcClient)) return; 
    
    printf("Destination account: "); scanf("%u", &destAccount);
    fseek(fPtr, (destAccount - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&destClient, sizeof(struct clientData), 1, fPtr);
    if (destClient.acctNum == 0) { printf(RED "Destination empty.\n" RESET); return; }
    
    printf("Amount to transfer (Rs): "); scanf("%lf", &amount);
    if (srcClient.balance - amount < ((srcClient.accountType == 'C') ? -10000.00 : 0.00)) {
        printf(RED ">>> DENIED: Insufficient Funds. <<<" RESET "\n"); return;
    }

    // PHASE 2: MUTEX LOCK INITIATED
    WaitForSingleObject(dbMutex, INFINITE);
    srcClient.balance -= amount; destClient.balance += amount;
    fseek(fPtr, (srcAccount - 1) * sizeof(struct clientData), SEEK_SET); fwrite(&srcClient, sizeof(struct clientData), 1, fPtr);
    fseek(fPtr, (destAccount - 1) * sizeof(struct clientData), SEEK_SET); fwrite(&destClient, sizeof(struct clientData), 1, fPtr);
    ReleaseMutex(dbMutex);

    printf(GREEN ">>> Transfer successful! <<<" RESET "\n");
    FILE *logPtr = fopen("transactions_log.txt", "a");
    if (logPtr) {
        fprintf(logPtr, "Acct: %u | Transfer TO %u: -%.2f | Balance: %.2f\n", srcAccount, destAccount, amount, srcClient.balance);
        fprintf(logPtr, "Acct: %u | Transfer FROM %u: +%.2f | Balance: %.2f\n", destAccount, srcAccount, amount, destClient.balance);
        fclose(logPtr);
    }
}

// ======================= STANDARD SYSTEM FUNCTIONS =======================
void miniStatement(FILE *fPtr) {
    unsigned int account; struct clientData client = {0}; char searchStr[30], line[150]; 
    printf("\nEnter account number: "); scanf("%u", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0 || !verify2FA(&client)) return; 

    FILE *logPtr = fopen("transactions_log.txt", "r");
    if (!logPtr) return;
    struct TransactionNode *head = NULL, *current = NULL, *newNode = NULL;
    sprintf(searchStr, "Acct: %u |", account);
    while (fgets(line, sizeof(line), logPtr)) {
        if (strstr(line, searchStr)) {
            newNode = (struct TransactionNode*)malloc(sizeof(struct TransactionNode));
            strcpy(newNode->logData, line); newNode->next = NULL;
            if (head == NULL) { head = newNode; current = newNode; } 
            else { current->next = newNode; current = newNode; }
        }
    }
    fclose(logPtr);
    printf(CYAN "\n======= MINI STATEMENT FOR ACCT %u =======" RESET "\n", account);
    if (head == NULL) printf("No recent transaction history.\n");
    else { current = head; while (current != NULL) { printf("%s", current->logData); current = current->next; } }
    printf(CYAN "==========================================" RESET "\n");
    current = head; while (current != NULL) { struct TransactionNode *temp = current; current = current->next; free(temp); }
}

void exportToExcel(FILE *readPtr) {
    FILE *writePtr; struct clientData client = {0};
    if ((writePtr = fopen("bank_database.csv", "w")) == NULL) return;
    rewind(readPtr); fprintf(writePtr, "Account Number,Account Type,Last Name,First Name,Balance (Rs)\n");
    while (fread(&client, sizeof(struct clientData), 1, readPtr)) {
        if (client.acctNum != 0) fprintf(writePtr, "%u,%c,%s,%s,%.2f\n", client.acctNum, client.accountType, client.lastName, client.firstName, client.balance);
    }
    fclose(writePtr); printf(GREEN ">>> SUCCESS: Database exported to bank_database.csv! <<<" RESET "\n");
}

void requestService(FILE *fPtr) {
    unsigned int account, serviceType; struct clientData client = {0};
    printf("\nEnter your account number: "); scanf("%u", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0 || !verify2FA(&client)) return; 

    printf(CYAN "\n--- WHAT DO YOU NEED HELP WITH TODAY? ---" RESET "\n");
    printf(" 1. Request New Checkbook\n 2. Report Stolen Card\n 3. Auto-Loan Application\nChoice: ");
    if(scanf("%u", &serviceType) != 1) { clearInputBuffer(); return; }

    if (serviceType == 3) {
        printf(YELLOW "\n>>> AI ANALYZING FINANCIAL HISTORY..." RESET "\n");
        int creditScore = 500; creditScore += (int)(client.balance / 10000) * 10; 
        if (client.balance < 0) creditScore -= 100;        
        if (client.accountType == 'S') creditScore += 25;  
        if (creditScore > 900) creditScore = 900;
        printf(">>> CREDIT SCORE: %d / 900\n", creditScore);
        if (creditScore >= 600) {
            printf(GREEN ">>> APPROVED! Rs. 50,000 deposited. 5%% Fee deducted." RESET "\n");
            
            WaitForSingleObject(dbMutex, INFINITE); // MUTEX LOCK
            client.balance += (50000.00 - 2500.00); 
            fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
            ReleaseMutex(dbMutex);
            
        } else printf(RED ">>> DENIED: Credit score is too low." RESET "\n");
        return;
    }
    FILE *reqPtr = fopen("service_requests.txt", "a");
    if (!reqPtr) return;
    if (serviceType == 1) { fprintf(reqPtr, "[TICKET] Acct: %u | Type: CHECKBOOK\n", client.acctNum); printf(GREEN ">>> LOGGED: Checkbook mailed. <<<" RESET "\n"); } 
    else if (serviceType == 2) { fprintf(reqPtr, "[URGENT] Acct: %u | Type: STOLEN CARD\n", client.acctNum); printf(RED ">>> ALARM: Card locked! <<<" RESET "\n"); }
    fclose(reqPtr);
}

void newRecord(FILE *fPtr) {
    struct clientData client = {0}; struct biometricData bio = {0, "", 0, "EMPTY"};
    unsigned int account, inputPin; char newHash[65];
    printf("\nAssign Account Number (1-100): "); scanf("%u", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum != 0) { printf(RED "Account already exists.\n" RESET); return; }
    printf("Select Account Type ('S' or 'C'): "); scanf(" %c", &client.accountType); client.accountType = toupper(client.accountType); 
    printf("Set a 4-digit PIN: "); scanf("%u", &inputPin);
    printf("Enter Last Name, First Name, Initial Balance: "); scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);
    printf("Scan Fingerprint (Enter 64-char Hash): "); scanf("%64s", newHash);

    client.pin = secureHashPIN(inputPin); // HASH THE PIN
    client.acctNum = account; strcpy(client.thumbprint, "LEGACY");

    WaitForSingleObject(dbMutex, INFINITE); // MUTEX LOCK
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fwrite(&client, sizeof(struct clientData), 1, fPtr);
    ReleaseMutex(dbMutex);

    FILE *bioPtr = fopen("biometrics.dat", "rb+");
    if (bioPtr) {
        bio.acctNum = account; strcpy(bio.secureHash, newHash); bio.minutiaePoints = 88; strcpy(bio.status, "ACTIVE");
        fseek(bioPtr, (account - 1) * sizeof(struct biometricData), SEEK_SET); fwrite(&bio, sizeof(struct biometricData), 1, bioPtr); fclose(bioPtr);
    }
    printf(GREEN ">>> Account %u created and Secured! <<<" RESET "\n", account);
} 

void changePin(FILE *fPtr) {
    unsigned int account, newPin; struct clientData client = {0};
    printf("\nEnter account number: "); scanf("%u", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0 || !verify2FA(&client)) return; 
    printf("Enter NEW 4-digit PIN: "); scanf("%u", &newPin);
    client.pin = secureHashPIN(newPin); // HASH THE PIN
    
    WaitForSingleObject(dbMutex, INFINITE);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fwrite(&client, sizeof(struct clientData), 1, fPtr);
    ReleaseMutex(dbMutex);
    printf(GREEN ">>> PIN changed successfully. <<<" RESET "\n");
}

void bankAnalytics(FILE *fPtr) {
    struct clientData client = {0}; int totalAccounts = 0; double totalLiquidity = 0.0, totalDebt = 0.0;
    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr)) {
        if (client.acctNum != 0) { totalAccounts++; if (client.balance >= 0) totalLiquidity += client.balance; else totalDebt += (client.balance * -1); }
    }
    printf(CYAN "\n--- BANK MACRO ANALYTICS ---" RESET "\nTotal Accounts: %d\nLiquidity: Rs. %.2f\nDebt: Rs. %.2f\n", totalAccounts, totalLiquidity, totalDebt);
}

void displayRecords(FILE *readPtr) {
    struct clientData client = {0}; rewind(readPtr); 
    printf(CYAN "\n%-6s%-6s%-16s%-11s%12s\n" RESET, "Acct", "Type", "Last Name", "First Name", "Balance(Rs)");
    while (fread(&client, sizeof(struct clientData), 1, readPtr)) {
        if (client.acctNum != 0) printf("%-6u%-6c%-16s%-11s%12.2f\n", client.acctNum, client.accountType, client.lastName, client.firstName, client.balance);
    }
}

void searchAccount(FILE *fPtr) {
    unsigned int account; struct clientData client = {0};
    printf("\nEnter account number: "); scanf("%u", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0 || !verify2FA(&client)) return; 
    printf(CYAN "\nAcct %u | %s %s | Rs. %.2f\n" RESET, client.acctNum, client.firstName, client.lastName, client.balance);
}

void applyInterest(FILE *fPtr) {
    struct clientData client = {0}; rewind(fPtr); 
    WaitForSingleObject(dbMutex, INFINITE);
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0 && client.accountType == 'S' && client.balance > 0) {
            client.balance += (client.balance * 0.04);
            fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR); fwrite(&client, sizeof(struct clientData), 1, fPtr); fseek(fPtr, 0, SEEK_CUR); 
        }
    }
    ReleaseMutex(dbMutex);
    printf(GREEN "\n>>> 4%% Interest Batch Process Complete! <<<" RESET "\n");
}

void payBill(FILE *fPtr) {
    unsigned int account; double amount; struct clientData client = {0};
    printf("\nAccount number: "); scanf("%u", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0 || !verify2FA(&client)) return; 
    printf("Bill Amount: Rs. "); scanf("%lf", &amount);
    double total = amount + 25.00; 
    if (client.balance - total < ((client.accountType == 'C') ? -10000.00 : 0.00)) printf(RED ">>> DENIED <<<" RESET "\n");
    else {
        WaitForSingleObject(dbMutex, INFINITE);
        client.balance -= total; fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fwrite(&client, sizeof(struct clientData), 1, fPtr);
        ReleaseMutex(dbMutex);
        printf(GREEN ">>> Paid successfully! (Inc. Rs 25 Fee) <<<" RESET "\n");
    }
}

void viewRequests(void) {
    char line[150]; FILE *reqPtr = fopen("service_requests.txt", "r");
    if (!reqPtr) return;
    printf(CYAN "\n======= PENDING SUPPORT TICKETS =======" RESET "\n");
    while (fgets(line, sizeof(line), reqPtr)) printf("%s", line); fclose(reqPtr);
}

void deleteRecord(FILE *fPtr) {
    struct clientData client; struct clientData blankClient = {0}; 
    struct biometricData blankBio = {0, "", 0, "EMPTY"}; unsigned int account; 
    printf("\nEnter account to close: "); scanf("%u", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0 || !verify2FA(&client)) return; 
    
    WaitForSingleObject(dbMutex, INFINITE);
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    ReleaseMutex(dbMutex);

    FILE *bioPtr = fopen("biometrics.dat", "rb+");
    if(bioPtr) { fseek(bioPtr, (account - 1) * sizeof(struct biometricData), SEEK_SET); fwrite(&blankBio, sizeof(struct biometricData), 1, bioPtr); fclose(bioPtr); }
    printf(GREEN ">>> Account & Biometric Profile closed. Remaining Rs. %.2f dispensed. <<<" RESET "\n", client.balance);
}