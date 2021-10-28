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
    const char* firstName;
    const char* lastName;

    unsigned int monthlyIncome;

    struct Date* dateOfBirth;
};

struct BankAccount {
    unsigned int id;
    double balance;

    struct Customer *owner;
};

struct Bank {
    const char* name;

    unsigned int capacity;
    unsigned int registeredAccounts;
    unsigned int registerBonus;

    struct BankAccount** accounts;
};

/**
 * @return current Date as Date struct
 */
struct Date* getCurrentDate() {
    struct Date* currentDate = malloc(sizeof(struct Date));
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
struct Date* createDate(unsigned int day, unsigned int month, unsigned int year) {
    struct Date* dateOfBirth = malloc(sizeof(struct Date));

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
int idIsUnique(struct Bank* bank, unsigned int id) {
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
unsigned int createId(struct Bank* bank) {
    unsigned int id = (rand() % bank->capacity);

    while (idIsUnique(bank, id) == 0) {
        id = (rand() % bank->capacity);
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
struct Customer* createCustomer(const char* firstName, const char* lastName, struct Date* dateOfBirth, unsigned int income) {
    struct Customer* customer = malloc(sizeof(struct Customer));

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
struct Bank* createBank(const char* name, unsigned int capacity, unsigned int registerBonus) {
    struct Bank* bank = malloc(sizeof(struct Bank));
    bank->name = name;
    bank->capacity = capacity;
    bank->registeredAccounts = 0;
    bank->accounts = malloc(sizeof(struct BankAccount) * capacity);
    bank->registerBonus = registerBonus;
    return bank;
}

/**
 * @param bank The bank to check for the account
 * @param customer The customer that wants to open an account
 * @return 1 if customer already has account, 0 if customer has no account
 */
int customerHasAccount(struct Bank* bank, struct Customer* customer) {
    for (int i = 0; i < bank->registeredAccounts; i++) {
        struct Date* dateOfBirth = customer->dateOfBirth;
        struct Customer* owner = bank->accounts[i]->owner;

        int dateOfBirthMatches = dateOfBirth->day == owner->dateOfBirth->day
                && dateOfBirth->month == owner->dateOfBirth->month
                && dateOfBirth->year == owner->dateOfBirth->year;

        if (owner->firstName == customer->firstName && owner->lastName == customer->lastName && (dateOfBirthMatches == 1)) {
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
int createBankAccount(struct Bank* bank, struct Customer* customer) {
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
    struct BankAccount* account = malloc(sizeof(struct BankAccount));
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

char* getBankDetails(struct Bank* bank) {
    char* detailsString = malloc(15);
    sprintf(detailsString, "Name=%s\n", bank->name);

    unsigned int allocate = 15;

    char* capacity = malloc(allocate);
    sprintf(capacity, "Capacity=%i\n", bank->capacity);
    detailsString = realloc(detailsString, strlen(detailsString) + allocate);
    strcat(detailsString, capacity);

    allocate = 40;
    char* registeredAccounts = malloc(allocate);
    sprintf(registeredAccounts, "Registered Accounts=%i\n", bank->registeredAccounts);
    detailsString = realloc(detailsString, strlen(detailsString) + allocate);
    strcat(detailsString, registeredAccounts);

    char* registerBonus = malloc(allocate);
    sprintf(registerBonus, "Register Bonus=%i\n", bank->registerBonus);
    detailsString = realloc(detailsString, strlen(detailsString) + allocate);
    strcat(detailsString, registerBonus);

    strcat(detailsString, "[");
    for(int i = 0; i < bank->registeredAccounts; i++) {
        strcat(detailsString, "{");
        struct BankAccount* account = bank->accounts[i];
        struct Customer* owner = account->owner;

        allocate = 200;
        char* accountStr = malloc(allocate);
        sprintf(accountStr, "ID=%i;Balance=%f;Owner={First Name=%s;Last Name=%s;Birthday={Day=%i;Month=%i;Year=%i}}",
                account->id,
                account->balance,
                owner->firstName,
                owner->lastName,
                owner->dateOfBirth->day,
                owner->dateOfBirth->month,
                owner->dateOfBirth->year);
        strcat(detailsString, accountStr);

        if(i+1 == bank->registeredAccounts) {
            strcat(detailsString, "}");
        } else {
            strcat(detailsString, "},\n");
        }
    }
    strcat(detailsString, "]");

    return detailsString;
}

void saveBankToFile(struct Bank* bank) {
    FILE* out = fopen(bank->name, "w");
    fputs(getBankDetails(bank), out);
    fclose(out);
}

char* readFromFile(const char* bankName) {
    char* buf = malloc(1024);
    size_t nread;
    FILE* file = fopen(bankName, "r");

    if(buf == NULL) {
        return "Buffer is null, idk why. Fix your shit.";
    }

    while((nread = fread(buf, 1, 1024, file)) > 0);
    fclose(file);

    return buf;
}

void getValueFromBankString(const char* bankName, const char* valueName) {
    char* bankFile = readFromFile(bankName);
}

int main(void) {
    struct Bank* bank = createBank("Sparkasse", 3, 100);
    struct Customer* marcel = createCustomer("Marcel", "K", createDate(1, 7, 2002), 450);
    struct Customer* anna = createCustomer("Penis", "Kopf", createDate(28, 3, 1990), 0);

    createBankAccount(bank, marcel);
    createBankAccount(bank, anna);

    updateBalance(bank->accounts[0], 10);

    printf("%s\n", getBankDetails(bank));

    saveBankToFile(bank);

    free(anna);
    free(marcel);
    free(bank);
    return 0;
}