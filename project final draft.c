#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_TRANS "transactions.txt"
#define FILE_LIMIT "limit.txt"

typedef struct {
    int id;
    char type[10];      // "Income" or "Expense"
    float amount;
    char category[30];
    int day, month, year;
} Transaction;

// Function prototypes
void addIncome();
void addExpense();
void checkLimitAlert(int month, int year);
void monthlyReport();
void yearlyReport();
void viewSavings();
int  getNextID();
void saveTransaction(Transaction t);
void loadTransactions(Transaction arr[], int *n);
float getSpendingLimit();
void setLimit();
void resetData();          // NEW: reset all data
void pressEnter();

// =================== MAIN MENU ===================

int main() {
    int choice;

    while (1) {
        printf("\n============ BUDGET TRACKER ============\n");
        printf("1. Add Income\n");
        printf("2. Add Expense\n");
        printf("3. Monthly Report\n");
        printf("4. Yearly Report\n");
        printf("5. View Total Savings\n");
        printf("6. Set Monthly Limit\n");
        printf("7. Reset All Data\n");   // NEW OPTION
        printf("8. Exit\n");
        printf("----------------------------------------\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addIncome();      break;
            case 2: addExpense();     break;
            case 3: monthlyReport();  break;
            case 4: yearlyReport();   break;
            case 5: viewSavings();    break;
            case 6: setLimit();       break;
            case 7: resetData();      break;
            case 8: 
                printf("Exiting program...\n"); 
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}



// =================== ADD INCOME ===================

void addIncome() {
    Transaction t;
    t.id = getNextID();
    strcpy(t.type, "Income");

    printf("\n--- Add Income ---\n");
    printf("Enter amount: ");
    scanf("%f", &t.amount);

    printf("Enter category (Salary/Gift/etc): ");
    fflush(stdin);
    fgets(t.category, sizeof(t.category), stdin);
    t.category[strcspn(t.category, "\n")] = '\0';   // remove newline

    printf("Enter date (dd mm yyyy): ");
    scanf("%d %d %d", &t.day, &t.month, &t.year);

    saveTransaction(t);
    printf("Income added successfully!\n");

    pressEnter();
}



// =================== ADD EXPENSE ===================

void addExpense() {
    Transaction t;
    t.id = getNextID();
    strcpy(t.type, "Expense");

    printf("\n--- Add Expense ---\n");
    printf("Enter amount: ");
    scanf("%f", &t.amount);

    printf("Enter category (Food/Transport/etc): ");
    fflush(stdin);
    fgets(t.category, sizeof(t.category), stdin);
    t.category[strcspn(t.category, "\n")] = '\0';

    printf("Enter date (dd mm yyyy): ");
    scanf("%d %d %d", &t.day, &t.month, &t.year);

    saveTransaction(t);

    // Check monthly limit after adding expense
    checkLimitAlert(t.month, t.year);

    printf("Expense added successfully!\n");

    pressEnter();
}



// ===== ALERT: MONTHLY EXPENSE CROSSES LIMIT =====

void checkLimitAlert(int month, int year) {
    float limit = getSpendingLimit();
    if (limit <= 0) return;   // if no limit set, do nothing

    Transaction arr[1000];
    int n = 0, i;
    loadTransactions(arr, &n);

    float totalExpense = 0;

    for (i = 0; i < n; i++) {
        if (strcmp(arr[i].type, "Expense") == 0 &&
            arr[i].month == month &&
            arr[i].year == year) 
        {
            totalExpense += arr[i].amount;
        }
    }

    if (totalExpense > limit) {
        printf("\n**************************************\n");
        printf("    ALERT! You exceeded your limit!\n");
        printf("    Month: %d-%d\n", month, year);
        printf("    Limit: %.2f | Spent: %.2f\n", limit, totalExpense);
        printf("**************************************\n");
    }
}



// =================== MONTHLY REPORT ===================

void monthlyReport() {
    int month, year;
    Transaction arr[1000];
    int n = 0, i;

    printf("\n--- Monthly Report ---\n");
    printf("Enter month & year (mm yyyy): ");
    scanf("%d %d", &month, &year);

    loadTransactions(arr, &n);

    float income = 0, expense = 0;

    printf("\nID | Type    | Amount   | Category\n");
    printf("-----------------------------------------\n");

    for (i = 0; i < n; i++) {
        if (arr[i].month == month && arr[i].year == year) {
            printf("%d | %-7s | %8.2f | %s\n",
                   arr[i].id, arr[i].type, arr[i].amount, arr[i].category);

            if (strcmp(arr[i].type, "Income") == 0)
                income += arr[i].amount;
            else
                expense += arr[i].amount;
        }
    }

    printf("-----------------------------------------\n");
    printf("Total Income : %.2f\n", income);
    printf("Total Expense: %.2f\n", expense);
    printf("Savings      : %.2f\n", income - expense);

    pressEnter();
}



// =================== YEARLY REPORT ===================

void yearlyReport() {
    int year, i;
    Transaction arr[1000];
    int n = 0;

    printf("\n--- Yearly Report ---\n");
    printf("Enter year: ");
    scanf("%d", &year);

    loadTransactions(arr, &n);

    float totalIncome = 0, totalExpense = 0;

    for (i = 0; i < n; i++) {
        if (arr[i].year == year) {
            if (strcmp(arr[i].type, "Income") == 0)
                totalIncome += arr[i].amount;
            else
                totalExpense += arr[i].amount;
        }
    }

    printf("\nTotal Income  : %.2f\n", totalIncome);
    printf("Total Expense : %.2f\n", totalExpense);
    printf("Total Savings : %.2f\n", totalIncome - totalExpense);

    pressEnter();
}



// ========== VIEW TOTAL SAVINGS (ALL TIME) ==========

void viewSavings() {
    Transaction arr[1000];
    int n = 0, i;
    float income = 0, expense = 0;

    loadTransactions(arr, &n);

    for (i = 0; i < n; i++) {
        if (strcmp(arr[i].type, "Income") == 0)
            income += arr[i].amount;
        else
            expense += arr[i].amount;
    }

    printf("\n--- Overall Savings ---\n");
    printf("Total Income : %.2f\n", income);
    printf("Total Expense: %.2f\n", expense);
    printf("Savings      : %.2f\n", income - expense);

    pressEnter();
}



// ========== GET NEXT TRANSACTION ID ==========

int getNextID() {
    FILE *fp = fopen(FILE_TRANS, "r");
    if (!fp) return 1;   // if file doesn't exist, first ID is 1

    int maxID = 0;
    Transaction t;

    while (fscanf(fp, "%d %s %f %s %d %d %d",
                  &t.id, t.type, &t.amount, t.category,
                  &t.day, &t.month, &t.year) == 7) 
    {
        if (t.id > maxID) maxID = t.id;
    }

    fclose(fp);
    return maxID + 1;
}



// ========== SAVE A TRANSACTION ==========

void saveTransaction(Transaction t) {
    FILE *fp = fopen(FILE_TRANS, "a");
    if (!fp) {
        printf("File error! Could not open transactions file.\n");
        return;
    }

    fprintf(fp, "%d %s %.2f %s %d %d %d\n",
            t.id, t.type, t.amount, t.category,
            t.day, t.month, t.year);

    fclose(fp);
}



// ========== LOAD ALL TRANSACTIONS ==========

void loadTransactions(Transaction arr[], int *n) {
    FILE *fp = fopen(FILE_TRANS, "r");
    if (!fp) {
        *n = 0;
        return;
    }

    *n = 0;

    while (fscanf(fp, "%d %s %f %s %d %d %d",
                  &arr[*n].id, arr[*n].type, &arr[*n].amount,
                  arr[*n].category, &arr[*n].day,
                  &arr[*n].month, &arr[*n].year) == 7)
    {
        (*n)++;
        if (*n >= 1000) break;  // safety limit
    }

    fclose(fp);
}



// ========== GET MONTHLY SPENDING LIMIT ==========

float getSpendingLimit() {
    FILE *fp = fopen(FILE_LIMIT, "r");
    if (!fp) return 0;   // no limit set yet

    float limit;
    fscanf(fp, "%f", &limit);
    fclose(fp);
    return limit;
}



// ========== SET MONTHLY SPENDING LIMIT ==========

void setLimit() {
    float limit;
    printf("\nEnter new monthly spending limit: ");
    scanf("%f", &limit);

    FILE *fp = fopen(FILE_LIMIT, "w");
    if (!fp) {
        printf("File error! Could not open limit file.\n");
        pressEnter();
        return;
    }

    fprintf(fp, "%.2f", limit);
    fclose(fp);

    printf("Limit updated successfully!\n");

    pressEnter();
}



// ========== RESET ALL DATA (NEW FEATURE) ==========

void resetData() {
    char confirm;
    printf("\nAre you sure you want to delete ALL data? (y/n): ");
    fflush(stdin);
    scanf(" %c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
        // Clear transactions file
        FILE *fp = fopen(FILE_TRANS, "w");
        if (fp) fclose(fp);

        // Clear limit file
        fp = fopen(FILE_LIMIT, "w");
        if (fp) fclose(fp);

        printf("\nAll data has been reset successfully!\n");
    } else {
        printf("\nReset cancelled.\n");
    }

    pressEnter();
}



// ========== PRESS ENTER TO CONTINUE ==========

void pressEnter() {
    printf("\nPress ENTER to continue...");
    fflush(stdin);
    getchar();
}




