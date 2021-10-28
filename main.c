#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <string.h>

struct Date {
    unsigned int day;
    unsigned int month;
    unsigned int year;
};

struct Customer {
    const char *firstName;
    const char *lastName;

    unsigned int monthlyIncome;

    struct Date *dateOfBirth;
};

struct BankAccount {
    unsigned int id;
    double balance;

    struct Customer *owner;
};

struct Bank {
    const char *name;

    unsigned int capacity;
    unsigned int registeredAccounts;
    unsigned int registerBonus;

    struct BankAccount **accounts;
};

/**
 * @return current Date as Date struct
 */
struct Date *getCurrentDate() {
    struct Date *currentDate = malloc(sizeof(struct Date *));
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    currentDate->day = tm.tm_mday;
    currentDate->month = tm.tm_mon + 1;
    currentDate->year = tm.tm_year + 1900;

    return currentDate;
}

/**
 *
 * @param day
 * @param month
 * @param year
 * @return Date struct
 */
struct Date *createDate(unsigned int day, unsigned int month, unsigned int year) {
    struct Date *dateOfBirth = malloc(sizeof(struct Date *));

    // get current year
    unsigned int currentYear = getCurrentDate()->year;

    dateOfBirth->day = day;
    dateOfBirth->month = month;
    dateOfBirth->year = year;

    // error cases
    // day is below the first of a month or beyond the 31st
    if (day < 1 || day > 31) {
        dateOfBirth->day = 1;
    }

    // month is not in between january and december
    if (month < 1 || month > 12) {
        dateOfBirth->month = 1;
    }

    // year is not in the range of currentYear - 100 and the currentYear
    if (year < currentYear - 100 || year > currentYear) {
        dateOfBirth->year = currentYear - 5;
    }

    // special case for february because it usually only has 28 days
    if (month == 2 && day > 28) {
        dateOfBirth->day = 28;
        dateOfBirth->month = 2;
    }

    return dateOfBirth;
}

/**
 * @param year the person was born in
 * @return age of person
 */
unsigned int getAge(unsigned int year) {
    unsigned int age = getCurrentDate()->year - year;
    return age;
}

/**
 *
 * @param dateOfBirth of the person you want to know the age of
 * @return 1 if person is of legal age, 0 if person isn't of legal age
 */
unsigned int isOfLegalAge(unsigned int year) {
    return getAge(year) >= 18;
}

/**
 * @param bank
 * @param id the id to check if it's unique
 * @return 1 if is unique, 0 if is not unique
 */
int idIsUnique(struct Bank *bank, unsigned int id) {
    for (int i = 0; i < bank->registeredAccounts; i++) {
        if (bank->accounts[i]->id == id) {
            return 0;
        }
    }
    return 1;
}

/**
 * @param bank the bank to check whether an id is already given away or not
 * @param lower lower end of the randomizer
 * @param upper
 * @return unique id
 */
unsigned int createId(struct Bank *bank) {
    unsigned int id = (rand() % (bank->capacity - 0)) + 0;

    while (idIsUnique(bank, id) == 0) {
        id = (rand() % (bank->capacity - 0)) + 0;
    }

    return id;
}

/**
 *
 * @param firstName
 * @param lastName
 * @param dateOfBirth
 * @return pointer to Customer struct
 */
struct Customer* createCustomer(const char *firstName, const char *lastName, struct Date *dateOfBirth, unsigned int income) {
    struct Customer *customer = malloc(sizeof(struct Customer*));

    if (strcmp(firstName, "") == 0) {
        printf("Please input a valid first name!\n");
        return NULL;
    }

    if (strcmp(lastName, "") == 0) {
        printf("Plase input a valid last name!\n");
        return NULL;
    }

    if (dateOfBirth == NULL) {
        printf("Please insert a valid date of birth!\n");
        return NULL;
    }

    customer->firstName = firstName;
    customer->lastName = lastName;
    customer->dateOfBirth = dateOfBirth;
    customer->monthlyIncome = income;

    return customer;
}

/**
 * @param capacity max clients a bank can have
 * @param registerBonus the bonus every customer gets when he registeres
 * @return pointer to the created bank
 */
struct Bank *createBank(const char *name, unsigned int capacity, unsigned int registerBonus) {
    struct Bank *bank = malloc(sizeof(struct Bank *));
    bank->name = name;
    bank->capacity = capacity;
    bank->registeredAccounts = 0;
    bank->accounts = malloc(sizeof(struct BankAccount*) * capacity);
    bank->registerBonus = registerBonus;
    return bank;
}

/**
 *
 * @param bank The bank to check for the account
 * @param customer The customer that wants to open an account
 * @return 1 if customer already has account, 0 if customer has no account
 */
int customerHasAccount(struct Bank *bank, struct Customer *customer) {
    for (int i = 0; i < bank->registeredAccounts; i++) {
        struct Customer *owner = bank->accounts[i]->owner;
        if (owner->firstName == customer->firstName && owner->lastName == customer->lastName) {
            return 1;
        }
    }
    return 0;
}

/**
 * @param bank the bank the account should be created for
 * @param customer the customer the account belongs to
 * @return -1 if error occured, 0 if creation was successfull
 */
int createBankAccount(struct Bank *bank, struct Customer *customer) {
    if(customerHasAccount(bank, customer)) {
        printf("Customer already has account!\n");
        return -1;
    }

    if (!isOfLegalAge(customer->dateOfBirth->year)) {
        printf("Customer has to be at least 18 years old!\n");
        return -1;
    }

    if (bank->registeredAccounts >= bank->capacity) {
        printf("There is not enough space to create further bank accounts! Sorry!\n");
        return -1;
    }

    // allocates memory for BankAccount
    struct BankAccount *account = malloc(sizeof(struct BankAccount *));
    // set owner of account
    account->owner = customer;

    // sets the unique id for the bank account
    account->id = createId(bank);

    // set the accounts balance to the banks register bonus
    account->balance = bank->registerBonus;

    // if bank has 0 accounts, it should use 0 as index otherwise the count of the registered bank accounts
    if (bank->registeredAccounts == 0) {
        bank->accounts[0] = account;
    } else {
        bank->accounts[bank->registeredAccounts] = account;
    }
    // update count of bank accounts
    bank->registeredAccounts++;

    return 0;
}

/**
 * @param account
 * @param amount
 * @return 0 if update was a success, -1 if some error occured
 */
int updateBalance(struct BankAccount* account, int amount) {
    if(amount == 0) {
        printf("Please choose an amount greater than 0 to add money to your bank account "
               "or an amount smaller than 0 to remove money from your account!\n");
        return -1;
    }
    account->balance += amount;
    return 0;
}

/**
 * @param bank
 * @param firstName
 * @param lastName
 * @param dateOfBirth
 * @return BankAccount if something was found, otherwise NULL
 */
struct BankAccount* getAccountByOwnerDetails(struct Bank* bank, const char* firstName, const char* lastName, struct Date* dateOfBirth) {
    for(int i = 0; i < bank->registeredAccounts; i++) {
        struct BankAccount* account = bank->accounts[i];
        struct Customer* owner = account->owner;
        struct Date* ownerDate = owner->dateOfBirth;

        int dateMatches = ownerDate->day == dateOfBirth->day
                && ownerDate->month == dateOfBirth->month
                && ownerDate->year == dateOfBirth->year;

        if(strcmp(owner->firstName, firstName) == 0 && strcmp(owner->lastName, lastName) == 0 && dateMatches) {
            return account;
        }
    }

    return NULL;
}

/**
 *
 * @param bank
 * @param id ID of the account
 * @return BankAccount if something was found, otherwise NULL
 */
struct BankAccount* getAccountById(struct Bank* bank, unsigned int id) {
    for(int i = 0; i < bank->registeredAccounts; i++) {
        struct BankAccount* account = bank->accounts[i];
        if(account->id == id) {
            return account;
        }
    }
    return NULL;
}

/**
 * @param bank print the details of given Bank
 */
void printBankDetails(struct Bank *bank) {
    /*
    char* bankDetails = malloc(100);

    strcat(bankDetails, "{\n");


    const int tabSize = 8;

    char* bankName = malloc(tabSize + 14 + sizeof(bank->name));
    sprintf(bankName, "\t\"Name\": \"%s\",\n", bank->name);
    printf("size: %i\n", sizeof(bankDetails) + sizeof(bankName));
    // realloc(bankDetails, sizeof(bankDetails) + sizeof(bankName));
    strcat(bankDetails, bankName);


    char* capacity = malloc(tabSize + 20 + sizeof(bank->capacity));
    sprintf(capacity, "\t\"Capacity\": \"%i\",\n", bank->capacity);
    realloc(bankDetails, sizeof(bankDetails) + sizeof(capacity));
    strcat(bankDetails, capacity);


    char* registerBonus = malloc(tabSize + 25 + sizeof(bank->registerBonus));
    sprintf(registerBonus, "\t\"Register Bonus\": \"%i\",\n", bank->registerBonus);
    realloc(bankDetails, sizeof(bankDetails) + sizeof(registerBonus));
    strcat(bankDetails, registerBonus);

    char* registeredAccounts = malloc(tabSize + 30 + sizeof(bank->registeredAccounts));
    sprintf(registeredAccounts, "\t\"Registered Accounts\": \"%i\",\n", bank->registeredAccounts);
    realloc(bankDetails, sizeof(bankDetails) + sizeof(registeredAccounts));
    strcat(bankDetails, registeredAccounts);

    char* accountStr = malloc(tabSize + 15);
    sprintf(accountStr, "\t\"Accounts\": [\n");
    realloc(bankDetails, sizeof(bankDetails) + sizeof(accountStr));
    strcat(bankDetails, accountStr);


    for(int i = 0; i < bank->registeredAccounts; i++) {
        struct BankAccount* account = bank->accounts[i];

        char* openAccountDetailsStr = malloc((tabSize*2) + 5);
        sprintf(openAccountDetailsStr, "\t\t{\n");
        realloc(bankDetails, sizeof(bankDetails) + sizeof(openAccountDetailsStr));
        strcat(bankDetails, openAccountDetailsStr);

        char* id = malloc((tabSize*3) + 15 + sizeof(account->id));
        sprintf(id, "\t\t\t\"ID\": \"%i\",\n", account->id);
        realloc(bankDetails, sizeof(bankDetails) + sizeof(id));
        strcat(bankDetails, id);

        char* ownerOpeningString = malloc((tabSize*3) + 15);
        sprintf(ownerOpeningString, "\t\t\t\"Owner\": {\n");
        realloc(bankDetails, sizeof(bankDetails) + sizeof(ownerOpeningString));
        strcat(bankDetails, ownerOpeningString);

        // OWNER
        struct Customer* owner = account->owner;

        char* ownerFirstName = malloc((tabSize*4) + 25 + sizeof(owner->firstName));
        sprintf(ownerFirstName, "\t\t\t\t\"First Name\": \"%s\",\n", owner->firstName);
        realloc(bankDetails, sizeof(bankDetails) + sizeof(ownerFirstName));
        strcat(bankDetails, ownerFirstName);


        char* ownerLastName = malloc((tabSize*4) + 25 + sizeof(owner->lastName));
        sprintf(ownerLastName, "\t\t\t\t\"Last Name\": \"%s\",\n", owner->lastName);
        realloc(bankDetails, sizeof(bankDetails) + sizeof(ownerLastName));
        strcat(bankDetails, ownerLastName);


        char* ageOpeningStr = malloc((tabSize*4) + 10);
        sprintf(ageOpeningStr, "\t\t\t\t\"Age\": {\n");
        realloc(bankDetails, sizeof(bankDetails) + sizeof(ageOpeningStr));
        strcat(bankDetails, ageOpeningStr);

        char* dateOfBirthDay = malloc((tabSize*5) + 15);
        sprintf(dateOfBirthDay, "\t\t\t\t\t\"Day\": \"%i\",\n", owner->dateOfBirth->day);
        realloc(bankDetails, sizeof(bankDetails) + sizeof(dateOfBirthDay));
        strcat(bankDetails, dateOfBirthDay);


        char* dateOfBirthMonth = malloc((tabSize*5) + 20);
        sprintf(dateOfBirthMonth, "\t\t\t\t\t\"Month\": \"%i\",\n", owner->dateOfBirth->month);
        realloc(bankDetails, sizeof(bankDetails) + sizeof(dateOfBirthMonth));
        strcat(bankDetails, dateOfBirthMonth);


        char* dateOfBirthYear = malloc((tabSize*5) + 20);
        sprintf(dateOfBirthYear, "\t\t\t\t\t\"Year\": \"%i\",\n", owner->dateOfBirth->year);
        strcat(bankDetails, dateOfBirthYear);

        char* currentAge = malloc((tabSize*5) + 25 + sizeof(getAge(owner->dateOfBirth->year)));
        sprintf(currentAge, "\t\t\t\t\t\"Current Age\": \"%i\"\n", getAge(owner->dateOfBirth->year));
        strcat(bankDetails, currentAge);

        char* ageCloseStr = malloc((tabSize*4) + 5);
        sprintf(ageCloseStr, "\t\t\t\t}\n");
        strcat(bankDetails, ageCloseStr);

        char* ownerCloseStr = malloc((tabSize*3) + 5);
        sprintf(ownerCloseStr, "\t\t\t},\n");
        strcat(bankDetails, ownerCloseStr);

        char* balance = malloc((tabSize*3) + 30 + sizeof(account->balance));
        sprintf(balance, "\t\t\t\"Balance\": \"%f\"\n", account->balance);
        strcat(bankDetails, balance);

        char* closeAccountDetailsStr = malloc((tabSize*2) + 10);
        if (i + 1 == bank->registeredAccounts) {
            sprintf(closeAccountDetailsStr, "\t\t}\n");
        } else {
            sprintf(closeAccountDetailsStr, "\t\t},\n");
        }
        strcat(bankDetails, closeAccountDetailsStr);

    }


    char* closeAccountsArray = malloc(tabSize + 5);
    sprintf(closeAccountsArray, "\t]\n");
    strcat(bankDetails, closeAccountsArray);


    char* closeFileStr = malloc(5);
    sprintf(closeFileStr, "}\n");
    strcat(bankDetails, closeFileStr);

    printf("%s\n", bankDetails);
    */
    /*
    printf("{\n");
        printf("\t\"Name\": \"%s\",\n", bank->name);
        printf("\t\"Capacity\": \"%i\",\n", bank->capacity);
        printf("\t\"Register Bonus\": \"%i\",\n", bank->registerBonus);

        // All about the accounts
        printf("\t\"Registered Accounts\": \"%i\",\n", bank->registeredAccounts);
        printf("\t\"Accounts\": [\n");
    for (int i = 0; i < bank->registeredAccounts; i++) {
        struct BankAccount *account = bank->accounts[i];
            printf("\t\t{\n");
                printf("\t\t\t\"ID\": \"%i\",\n", account->id);

                // ======= Details of Owner =======
                printf("\t\t\t\"Owner\": {\n");
                struct Customer *owner = account->owner;
                    printf("\t\t\t\t\"First Name\": \"%s\",\n", owner->firstName);
                    printf("\t\t\t\t\"Last Name\": \"%s\",\n", owner->lastName);

                    // age object
                    printf("\t\t\t\t\"Age\": {\n");
                        printf("\t\t\t\t\t\"Day\": \"%i\",\n", owner->dateOfBirth->day);
                        printf("\t\t\t\t\t\"Month\": \"%i\",\n", owner->dateOfBirth->month);
                        printf("\t\t\t\t\t\"Year\": \"%i\",\n", owner->dateOfBirth->year);
                        printf("\t\t\t\t\t\"Current Age\": \"%i\"\n", getAge(owner->dateOfBirth->year));
                    printf("\t\t\t\t}\n");

                printf("\t\t\t},\n");
                // ================================

                printf("\t\t\t\"Balance\": \"%f\"\n", account->balance);

        if (i + 1 == bank->registeredAccounts) {
            printf("\t\t}\n");
        } else {
            printf("\t\t},\n");
        }
    }
    printf("\t]\n");
    printf("}\n");
    */
}

/**
 *
 * @param account The account you want to print the details of
 */
void printDetailsOfBankAccount(struct BankAccount* account) {
    if(account == NULL) {
        return;
    }
    printf("{\n");
    printf("\t\"ID\": \"%i\",\n", account->id);

    // ======= Details of Owner =======
    printf("\t\"Owner\": {\n");
    struct Customer *owner = account->owner;
    printf("\t\t\"First Name\": \"%s\",\n", owner->firstName);
    printf("\t\t\"Last Name\": \"%s\",\n", owner->lastName);

    // age object
    printf("\t\t\"Age\": {\n");
    printf("\t\t\t\"Day\": \"%i\",\n", owner->dateOfBirth->day);
    printf("\t\t\t\"Month\": \"%i\",\n", owner->dateOfBirth->month);
    printf("\t\t\t\"Year\": \"%i\",\n", owner->dateOfBirth->year);
    printf("\t\t\t\"Current Age\": \"%i\"\n", getAge(owner->dateOfBirth->year));
    printf("\t\t}\n");

    printf("\t},\n");
    // ================================

    printf("\t\"Balance\": \"%f\"\n", account->balance);
    printf("}\n");
}

void saveBankToFile(struct Bank* bank) {

}

int main() {
    struct Bank* bank = createBank("Sparkasse", 3, 100);
    // struct Customer* marcel = createCustomer("Marcel", "K", createDate(1, 7, 2002), 450);
    // struct Customer *anna = createCustomer("Anna", "B", createDate(27, 9, 2001), 300);

    // createBankAccount(bank, marcel);
    // createBankAccount(bank, anna);

    // updateBalance(getAccountById(bank, 1), -10);

    // printBankDetails(bank);

    // free(anna);
    // free(marcel);
    // free(bank);
    return 0;
}
