#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// The maximum number of equipment items
#define MAX 100

// Function prototypes
void printReceipt(char student[], char item[], char date[], char time[]);
void logBorrow(char student[], char item[], char date[], char time[]);
void logReturn(char student[], char item[]);
int searchEquipment(char name[]);
int isValidEquipment(char name[]);
int isAllUppercase(char str[]);
void insert(char student[], char item[], int priority, char date[], char time[]);

// Structure for equipment
struct equipment {
    char name[50];
    char category[50];
    int quantity;
};

// Array for storing all equipment records
struct equipment eq[MAX];
int count = 0; // Tracks total number of equipment

struct node {
    char student[50];
    char equipment[50];
    char date[20];
    char time[20];
    int priority;
    struct node *next;
};

// Front pointer of the priority queue
struct node *front = NULL;

// Helper: Checks if string is all uppercase
int isAllUppercase(char str[]) {
	int i;
    for ( i = 0; str[i] != '\0'; i++) {
        if (islower(str[i])) return 0;
    }
    return 1;
}

// Adds new equipment or updates existing quantity
void addEquipment() {
    char name[50], category[50];
    int qty;

    printf("Enter equipment name (MUST BE CAPS): ");
    scanf(" %[^\n]", name);

    if (!isAllUppercase(name)) {
        printf("Error: Equipment name must be in all CAPS (e.g., HDMI).\n");
        return;
    }

    if (!isValidEquipment(name)) {
        printf("Invalid equipment! Only HDMI, REMOTE, PROJECTOR are allowed.\n");
        return;
    }

    int index = searchEquipment(name);

    if (index != -1) {
        printf("Equipment already exists!\n");
        printf("\nEnter quantity to add: ");
        scanf("%d", &qty);
        eq[index].quantity += qty;
        printf("Quantity updated! New quantity: %d pcs\n", eq[index].quantity);
    } else {
        printf("\nEnter category (Audio Visual /Cable /Accessory): ");
        scanf(" %[^\n]", category);
        strcpy(eq[count].name, name);
        strcpy(eq[count].category, category);
        printf("\nEnter quantity: ");
        scanf("%d", &eq[count].quantity);
        count++;
        printf("\nEquipment added!\n");
    }
}

void displayEquipment() {
    if (count == 0) {
        printf("No equipment available.\n");
        return;
    }
    printf("\n-----------------------------------------------\n");
    printf("| %-3s | %-15s | %-12s | %-3s |\n", "No", "Name", "Category", "Qty");
    printf("-----------------------------------------------\n");
    int i;
    for ( i = 0; i < count; i++) {
        printf("| %-3d | %-15s | %-12s | %-3d |\n", i + 1, eq[i].name, eq[i].category, eq[i].quantity);
    }
}

int searchEquipment(char name[]) {
	int i;
    for ( i = 0; i < count; i++) {
        if (strcmp(eq[i].name, name) == 0) return i;
    }
    return -1;
}

void sortEquipment() {
    struct equipment temp;
    int i;
    for ( i = 0; i < count - 1; i++) {
    	int j;
        for ( j = 0; j < count - i - 1; j++) {
            if (strcmp(eq[j].name, eq[j + 1].name) > 0) {
                temp = eq[j];
                eq[j] = eq[j + 1];
                eq[j + 1] = temp;
            }
        }
    }
    printf("Sorted successfully!\n");
}

void insert(char student[], char item[], int priority, char date[], char time[]) {
    struct node *new_Node = (struct node*)malloc(sizeof(struct node));
    strcpy(new_Node->student, student);
    strcpy(new_Node->equipment, item);
    new_Node->priority = priority;
    strcpy(new_Node->date, date);
    strcpy(new_Node->time, time);
    new_Node->next = NULL;

    if (front == NULL || priority < front->priority) {
        new_Node->next = front;
        front = new_Node;
    } else {
        struct node *temp = front;
        while (temp->next != NULL && temp->next->priority <= priority) {
            temp = temp->next;
        }
        new_Node->next = temp->next;
        temp->next = new_Node;
    }
}

void borrowEquipment() {
    char student[50], item[50], date[20], timeStr[20];
    int priority, index;

    printf("Enter Full name: ");
    scanf(" %[^\n]", student);

    printf("\nEnter equipment (CAPS): ");
    scanf(" %[^\n]", item);

    if (!isValidEquipment(item)) {
        printf("Invalid equipment! Only HDMI, REMOTE, PROJECTOR are allowed.\n");
        return;
    }

    index = searchEquipment(item);
    if (index == -1) {
        printf("Equipment not found!\n");
        return;
    }
    if (eq[index].quantity <= 0) {
        printf("Out of stock!\n");
        return;
    }

    printf("\nEnter priority (1=Teacher, 2=Student): ");
    scanf("%d", &priority);
    printf("\nEnter date: ");
    scanf("%s", date);
    printf("\nEnter time: ");
    scanf("%s", timeStr);

    eq[index].quantity--;
    insert(student, item, priority, date, timeStr);
    printf("Borrow recorded successfully!\n");
    printReceipt(student, item, date, timeStr);
    logBorrow(student, item, date, timeStr);
}

void returnEquipment() {
    char student[50], item[50];
    printf("\nEnter name: ");
    scanf(" %[^\n]", student);
    printf("\nEnter equipment to return (CAPS): ");
    scanf(" %[^\n]", item);

    int index = searchEquipment(item);
    if (index == -1) {
        printf("Equipment not found!\n");
        return;
    }

    struct node *temp = front, *prev = NULL;
    while (temp != NULL) {
        if (strcmp(temp->student, student) == 0 && strcmp(temp->equipment, item) == 0) {
            if (prev == NULL) front = temp->next;
            else prev->next = temp->next;

            eq[index].quantity++;
            printf("\nReturned successfully!\n");
            logReturn(student, item);
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("No record found for this student and equipment!\n");
}

void searchByCategory() {
     char cat[50];
    int i, found = 0;

    printf("Enter category to search: ");
    scanf(" %[^\n]", cat);

    printf("\n-----------------------------------------------\n");
    printf("         SEARCH RESULT (Category: %s)\n", cat);
    printf("-----------------------------------------------\n");

    for (i = 0; i < count; i++) {
        if (strcmp(eq[i].category, cat) == 0) {
            printf("%-5d %-15s %-15s %-10d pcs\n",
                   i+1,
                   eq[i].name,
                   eq[i].category,
                   eq[i].quantity);
            found = 1;
        }
    }

    if (!found) {
        printf("No equipment found in this category.\n");
    }


}

void filterMenu() {
    int choice, i, found = 0;

   printf("\nFilter by Category:\n");
    printf("[1] Audio Visual\n");
    printf("[2] Cable\n");
    printf("[3] Accessory\n");
    printf("\nEnter choice: ");
    scanf("%d", &choice);

   const char *selected;

    if (choice == 1) selected = "Audio Visual";
    else if (choice == 2) selected = "Cable";
    else if (choice == 3) selected = "Accessory";
    else {
        printf("Invalid choice!\n");
        return;
    }

    printf("\n----------------------------------------------\n");
    printf("        FILTERED: %s\n", selected);
    printf("------------------------------------------------\n");

    for (i = 0; i < count; i++) {
        if (strcmp(eq[i].category, selected) == 0) {
            printf("%-5d %-15s %-15s %-10d pcs\n",
                   i+1,
                   eq[i].name,
                   eq[i].category,
                   eq[i].quantity);
            found = 1;
        }
    }

    if (!found) {
        printf("No equipment found.\n");
    }

    printf("------------------------------------------------\n");
}

void displayBorrowList() {
    struct node *temp = front;
    if (temp == NULL) { printf("No borrowing records.\n"); return; }
    printf("\n| %-15s | %-10s | %-12s | %-6s |\n", "Name", "Equipment", "Date", "Time");
    while (temp != NULL) {
        printf("| %-15s | %-10s | %-12s | %-6s |\n", temp->student, temp->equipment, temp->date, temp->time);
        temp = temp->next;
    }
}

void printReceipt(char student[], char item[], char date[], char time[]) {
    printf("\n==================== RECEIPT ===============\n");
    printf("             Name      : %s\n", student);
    printf("             Equipment : %s\n", item);
    printf("             Date      : %s\n", date);
    printf("             Time      : %s\n", time);
    printf("=============================================\n");
}

void logBorrow(char student[], char item[], char date[], char time[]) {
    FILE *fp = fopen("borrow_history.txt", "a");
    if (fp) { fprintf(fp, "%s borrowed %s on %s at %s\n", student, item, date, time); fclose(fp); }
}

void logReturn(char student[], char item[]) {
    FILE *fp = fopen("return_history.txt", "a");
    if (fp) { fprintf(fp, "%s returned %s\n", student, item); fclose(fp); }
}

void editEquipment() {
    char name[50], newName[50], newCategory[50];
    printf("Enter equipment name to edit: ");
    scanf(" %[^\n]", name);
    int index = searchEquipment(name);
    if (index == -1) { printf("Not found!\n"); return; }

    printf("\nEnter new name (CAPS): ");
    scanf(" %[^\n]", newName);
    if (!isAllUppercase(newName) || !isValidEquipment(newName)) {
        printf("Invalid name or not CAPS!\n");
        return;
    }
    printf("\nEnter new category: ");
    scanf(" %[^\n]", newCategory);
    printf("\nEnter new quantity: ");
    scanf("%d", &eq[index].quantity);
    strcpy(eq[index].name, newName);
    strcpy(eq[index].category, newCategory);
    printf("Updated!\n");
}

void deleteEquipment() {
    char name[50];
    printf("Enter equipment name to delete: ");
    scanf(" %[^\n]", name);
    int index = searchEquipment(name);
    if (index == -1) { printf("Not found!\n"); return; }
    int i;
    for ( i = index; i < count - 1; i++) eq[i] = eq[i + 1];
    count--;
    printf("Deleted!\n");
}

int isValidEquipment(char name[]) {
    if (strcmp(name, "HDMI") == 0 || strcmp(name, "REMOTE") == 0 || strcmp(name, "PROJECTOR") == 0) return 1;
    return 0;
}

int main() {
    int choice;
    while (1) {
        printf("\n---------------------------------------------\n");
        printf("             Hello CITZENS!\n                \n");
        printf("     SCHOOL EQUIPMENT BORROWING SYSTEM\n");
        printf("---------------------------------------------\n");
        printf("[1.] Add Equipment\n");
        printf("[2.] Display Equipment\n");
        printf("[3.] Borrow Equipment\n");
        printf("[4.] Return Equipment\n");
        printf("[5.] Search Equipment\n");
        printf("[6.] Sort Equipment\n");
        printf("[7.] Display Borrow List\n");
        printf("[8.] Search by Category\n");
        printf("[9.] Filter Equipment\n");
        printf("[10.] Edit Equipment\n");
        printf("[11.] Delete Equipment\n");
        printf("[12.] Exit\n");
        printf("---------------------------------------------\n");
        printf("\nEnter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
       printf("\n----------------------------------------------\n");

        switch (choice) {
            case 1: addEquipment(); break;
            case 2: displayEquipment(); break;
            case 3: borrowEquipment(); break;
            case 4: returnEquipment(); break;
            case 5: {
                char name[50];
                printf("Enter equipment name: ");
                scanf(" %[^\n]", name);
                int res = searchEquipment(name);
                if (res != -1)
                    printf("Found: %s (%d pcs)\n", eq[res].name, eq[res].quantity);
                else
                    printf("Not found!\n");
                break;
            }
            case 6: sortEquipment(); break;
            case 7: displayBorrowList(); break;
            case 8: searchByCategory(); break;
            case 9: filterMenu(); break;
            case 10: editEquipment(); break;
            case 11: deleteEquipment(); break;
            case 12: exit(0);
            default: printf("Invalid choice! Please select between 1 and 12.\n");
        }
    }
    return 0;
}
