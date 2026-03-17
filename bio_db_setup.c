#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct biometricData {
    unsigned int acctNum;      
    char secureHash[65];       
    int minutiaePoints;        
    char status[10];           
};

int main() {
    FILE *bioPtr;
    
    if ((bioPtr = fopen("biometrics.dat", "wb")) == NULL) {
        printf("Error: Could not create Biometric Vault.\n");
        return 1;
    }

    struct biometricData blankBio = {0, "", 0, "EMPTY"};
    for (int i = 0; i < 100; ++i) {
        fwrite(&blankBio, sizeof(struct biometricData), 1, bioPtr);
    }
    printf(">>> Biometric Vault initialized with 100 secure slots.\n");

    struct biometricData testBio[3] = {
        {1, "A8F5F167F44F4964E6C998DEE827110C_THUMB", 84, "ACTIVE"}, 
        {2, "C4CA4238A0B923820DCC509A6F75849B_INDEX", 72, "ACTIVE"}, 
        {3, "ECCBC87E4B5CE2FE28308FD9F2A7BAF3_THUMB", 91, "ACTIVE"}  
    };

    for (int i = 0; i < 3; i++) {
        fseek(bioPtr, (testBio[i].acctNum - 1) * sizeof(struct biometricData), SEEK_SET);
        fwrite(&testBio[i], sizeof(struct biometricData), 1, bioPtr);
        printf(">>> Registered Biometric Data for Account #%u | Points: %d\n", 
               testBio[i].acctNum, testBio[i].minutiaePoints);
    }

    fclose(bioPtr);
    printf("\nSUCCESS: Relational Biometric Database (biometrics.dat) is online and seeded!\n");
    return 0;
}