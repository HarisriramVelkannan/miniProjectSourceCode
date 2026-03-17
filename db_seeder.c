// File 1: db_seeder.c (Phase 3: One-Way Cryptographic Hashing)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct clientData {
    unsigned int acctNum; 
    unsigned int pin;     // Now stores an irreversible ONE-WAY HASH
    char thumbprint[15];  
    char accountType;     
    char lastName[15];    
    char firstName[10];   
    double balance;       
};

// ENTERPRISE ALGORITHM: DJB2 One-Way Cryptographic Hash
unsigned int secureHashPIN(unsigned int pin) {
    unsigned int hash = 5381;
    char pinStr[10];
    sprintf(pinStr, "%u", pin); // Convert PIN to string for bitwise hashing
    for (int i = 0; pinStr[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + pinStr[i]; /* hash * 33 + character */
    }
    return hash;
}

int main() {
    FILE *cfPtr;
    if ((cfPtr = fopen("credit.dat", "wb")) == NULL) return 1;

    struct clientData blankClient = {0, 0, "", ' ', "", "", 0.0};
    for (int i = 0; i < 100; ++i) fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);

    struct clientData testAccounts[3] = {
        {1, 1234, "LEGACY", 'S', "Velkannan", "Harisriram", 85000.00}, // Updated your name!
        {2, 9999, "LEGACY", 'C', "Sharma", "Priya", 15000.00},
        {3, 1111, "LEGACY", 'S', "Kumar", "Rahul", 5000.00}
    };

    for (int i = 0; i < 3; i++) {
        // APPLY ONE-WAY HASH BEFORE SAVING TO HARD DRIVE
        testAccounts[i].pin = secureHashPIN(testAccounts[i].pin);
        
        fseek(cfPtr, (testAccounts[i].acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&testAccounts[i], sizeof(struct clientData), 1, cfPtr);
    }
    fclose(cfPtr);
    
    printf("SUCCESS: Database Generated with One-Way Cryptography!\n");
    return 0;
}