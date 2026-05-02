#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

 // The maximum number of equipment items
#define MAX 100

// Function prototypes
// Lists all functions used in the program
void printReceipt(char student[], char item[], char date[], char time[]);
void logBorrow(char student[], char item[], char date[], char time[]);
void logReturn(char student[], char item[]);
int searchEquipment(char name[]);
int isValidEquipment(char name[]);
void toLowerCase(char str[]);
void insert(char student[], char item[], int priority, char date[], char time[]);

// Structure for equipment
// Stores name, category, and quantity of each item
struct equipment {
    char name[50];
    char category[50];
    int quantity;
};

//Array for storing all equipment records
struct equipment eq[MAX];
int count = 0;  // Tracks total number of equipment


struct node {
    char student[50];
    char equipment[50];
    char date[20];
    char time[20];
    int priority;
    struct node *next;
};
// Front pointer of the priority queue (linked list) 
struct node *front = NULL;

// Adds new equipment or updates existing quantity
void addEquipment() {
    char name[50], category[50];
    int qty;

    getchar(); // clear newline

    printf("Enter equipment name: ");
    scanf("%[^\n]", name);

// Validate input
if (!isValidEquipment(name)) {
    printf("Invalid equipment! Only HDMI, Remote, Projector are allowed.\n");
    return;
}

    int index = searchEquipment(name);

    if (index != -1) {
        // Equipment already exists -> update quantity
        printf("Equipment already exists!\n");
        printf("Enter quantity to add: ");
        scanf("%d", &qty);

        eq[index].quantity += qty;

        printf("Quantity updated! New quantity: %d pcs\n", eq[index].quantity);
    } else {
        // New equipment -> add to array
        getchar(); // clear newline

        strcpy(eq[count].name, name);

        printf("Enter category (Device/Cable/Accessory): ");
        scanf("%[^\n]", category);
        strcpy(eq[count].category, category);

        printf("Enter quantity: ");
        scanf("%d", &eq[count].quantity);

        count++;
        printf("\nEquipment added!\n");
    }
}

// Displays all available equipment in table format
void displayEquipment() {
    int i;
    if (count == 0) {
        printf("No equipment available.\n");
        return;
    }

    printf("\n---------------------------------------------\n");
    printf("\n|             EQUIPMENT LIST                 |\n");
    printf("-----------------------------------------------\n");
    printf("| %-3s | %-15s | %-12s | %-3s |\n",
       "No", "Name", "Category", "Qty");
    printf("-----------------------------------------------\n");
    for (i = 0; i < count; i++) {
        printf("| %-3d | %-15s | %-12s | %-3d |\n",
               i+1, eq[i].name, eq[i].category, eq[i].quantity);
      }
    }
    
// Linear Search
// Finds equipment by comparing names one by one
int searchEquipment(char name[]) {
    int i;
    for (i = 0; i < count; i++) {
        if (strcmp(eq[i].name, name) == 0)
            return i;
    }
    return -1;
}

 // Bubble sort
 // Arranges equipment alphabetically by name
void sortEquipment() {
    int i, j;
    struct equipment temp;

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (strcmp(eq[j].name, eq[j+1].name) > 0) {
                temp = eq[j];
                eq[j] = eq[j+1];
                eq[j+1] = temp;
            }
        }
    }
    printf("Sorted successfully!\n");
}
// Inserts new node into priority queue based on priority
 void insert(char student[], char item[], int priority, char date[], char time[]) {
    struct node *new_Node, *temp;


    // Allocating memory for new node
    new_Node = (struct node*)malloc(sizeof(struct node));
    strcpy(new_Node->student, student);
    strcpy(new_Node->equipment, item);
    new_Node->priority = priority;

    strcpy(new_Node->date, date);
    strcpy(new_Node->time, time);

    new_Node->next = NULL;
    // If the highest priority level
    if (front == NULL || priority < front->priority) {
        new_Node->next = front;
        front = new_Node;
    } else {
        temp = front;
        // Finding the correct position based on their priority level
        while (temp->next != NULL && temp->next->priority <= priority) {
            temp = temp->next;
        }

        new_Node->next = temp->next;
        temp->next = new_Node;
    }
}
// Removes and processes the highest priority borrower
void processQueue() {
    if (front == NULL) {
        printf("No queue to process.\n");
        return;
    }

    struct node *temp = front;
    front = front->next;

    printf("Processed: %s borrowed %s\n", temp->student, temp->equipment);
    free(temp);
}

// Handles borrowing of equipment
// Checks availability and adds to queue
 void borrowEquipment() {
    char student[50], item[50];
    char date[20], time[20];
    int priority, index;

    printf("Enter name: ");
    scanf("%s", student);

    printf("Enter equipment: ");
    scanf("%s", item);

    // Validate input
if (!isValidEquipment(item)) {
    printf("Invalid equipment! Only HDMI, Remote, Projector are allowed.\n");
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

    printf("Enter priority (1=Teacher, 2=Student): ");
    scanf("%d", &priority);


    printf("Enter date: ");
    scanf("%s", date);

    printf("Enter time: ");
    scanf("%s", time);

    // Display the available items
    eq[index].quantity--;

    // For inserting to queue
    insert(student, item, priority, date, time);

    printf("Borrow recorded successfully!\n");

    printReceipt(student, item, date, time);
    logBorrow(student, item, date, time);
}

// Handles returning of equipment and updates quantity
void returnEquipment() {
    char student[50], item[50];
    int index;

    printf("\nEnter name: ");
    scanf("%s", student);

    printf("Enter equipment to return: ");
    scanf("%s", item);

    index = searchEquipment(item);

    if (index == -1) {
        printf("Equipment not found!\n");
        return;
    }

    struct node *temp = front;
    struct node *prev = NULL;

    while (temp != NULL) {
        if (strcmp(temp->student, student) == 0 &&
            strcmp(temp->equipment, item) == 0) {

            // Remove node from queue
            if (prev == NULL) {
                front = temp->next;
            } else {
                prev->next = temp->next;
            }

            // Show the increase stock when the item is return
            eq[index].quantity++;

            // Get date & time
            time_t t;
            struct tm *tm_info;
            char date[20], timeStr[20];

            time(&t);
            tm_info = localtime(&t);

            strftime(date, sizeof(date), "%Y-%m-%d", tm_info);
            strftime(timeStr, sizeof(timeStr), "%H:%M", tm_info);

            // Output

            printf("Name      : %s\n", student);
            printf("Equipment : %s\n", item);
            printf("Date      : %s\n", date);
            printf("Time      : %s\n", timeStr);
            printf("\nReturned successfully!\n");
            logReturn(student, item);

            free(temp);
            return; // To run the program
        }

        prev = temp;
        temp = temp->next;
    }

    printf("No record found for this student and equipment!\n");
}

// Displays equipment based on selected category
void searchByCategory() {
    char cat[50];
    int i, found = 0;

    printf("Enter category to search: ");
    scanf("%s", cat);

    printf("\n-------------------------------------------------\n");
    printf("         SEARCH RESULT (Category: %s)\n", cat);
    printf("--------------------------------------------------\n");

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

    printf("-------------------------------------------------\n");
}

// For filtering equipment based on category
void filterMenu() {
    int choice, i, found = 0;

    printf("\nFilter by Category:\n");
    printf("[1] Device\n");
    printf("[2] Cable\n");
    printf("[3] Accessory\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    const char *selected;

    if (choice == 1) selected = "Device";
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

// Displays all borrowing records from the queue
// Helps monitor who borrowed and track transactions
void displayQueue() {
    struct node *temp = front;

    if (temp == NULL) {
        printf("No borrowing records.\n");
        return;
    }

    printf("-----------------------------------------------------------------\n");
    printf("| %-8s | %-10s | %-12s | %-6s | %-8s |\n",
           "Name", "Equipment", "Date", "Time", "Priority");
    printf("-----------------------------------------------------------------\n");

    while (temp != NULL) {

        // Convert priority number to text
        char priorityStr[10];
        if (temp->priority == 1)
            strcpy(priorityStr, "Teacher");
        else
            strcpy(priorityStr, "Student");

        printf("| %-8s | %-10s | %-12s | %-6s | %-8s |\n",
               temp->student,
               temp->equipment,
               temp->date,
               temp->time,
               priorityStr);

        temp = temp->next;
    }

    printf("-----------------------------------------------------------------\n");
}

// Prints receipt after borrowing
void printReceipt(char student[], char item[], char date[], char time[]) {
    printf("\n========== RECEIPT ==========\n");
    printf("Name      : %s\n", student);
    printf("Equipment : %s\n", item);
    printf("Date      : %s\n", date);
    printf("Time      : %s\n", time);
    printf("=============================\n");
}

// Saves borrowing history to file
void logBorrow(char student[], char item[], char date[], char time[]) {
    FILE *fp = fopen("borrow_history.txt", "a");
    if (fp == NULL) return;

    fprintf(fp, "%s borrowed %s on %s at %s\n",
            student, item, date, time);

    fclose(fp);
}

// Saves return transaction with date and time
void logReturn(char student[], char item[]) {
    FILE *fp = fopen("return_history.txt", "a");
    if (fp == NULL) return;

    time_t t;
    struct tm *tm_info;
    char date[20], timeStr[20];

    time(&t);
    tm_info = localtime(&t);

    strftime(date, 20, "%Y-%m-%d", tm_info);
    strftime(timeStr, 20, "%H:%M:%S", tm_info);

    fprintf(fp, "%s returned %s on %s at %s\n",
            student, item, date, timeStr);

    fclose(fp);
}

// Edits existing equipment details (names, category, quantity)
void editEquipment() {
    char name[50];
    int index;

    printf("Enter equipment name to edit: ");
    scanf("%s", name);

    index = searchEquipment(name);

    if (index == -1) {
        printf("Equipment not found!\n");
        return;
    }

       char newName[50];
    char newCategory[50];

    printf("Enter new name: ");
    scanf("%s", newName);

    // ? VALIDATION ADDED HERE
    if (!isValidEquipment(newName)) {
        printf("Invalid equipment! Only HDMI, Remote, Projector are allowed.\n");
        return;
    }

    printf("Enter new category: ");
    scanf("%s", newCategory);

    printf("Enter new quantity: ");
    scanf("%d", &eq[index].quantity);

    strcpy(eq[index].name, newName);
    strcpy(eq[index].category, newCategory);

    printf("Equipment updated successfully!\n");
}

// Removes equipment from the list
void deleteEquipment() {
    char name[50];
    int index, i;

    printf("Enter equipment name to delete: ");
    scanf("%s", name);

    index = searchEquipment(name);

    if (index == -1) {
        printf("Equipment not found!\n");
        return;
    }

    for (i = index; i < count - 1; i++) {
        eq[i] = eq[i + 1];
    }

    count--;

    printf("Equipment deleted successfully!\n");
}

    // Converts string to lowercase for comparison
    void toLowerCase(char str[]) {
    int i;
    for (i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Checks if equipment name is valid
int isValidEquipment(char name[]) {
    char temp[50];
    strcpy(temp, name);
    toLowerCase(temp);

    if (strcmp(temp, "hdmi") == 0 ||
        strcmp(temp, "remote") == 0 ||
        strcmp(temp, "projector") == 0) {
        return 1;
    }
    return 0;
}

// Main function that controls program flow and menu
int main() {
    int choice;

    while (1) {
    	printf("\n---------------------------------------------\n");
    	printf("                 Hello CITZENS!                \n ");
        printf("   SCHOOL EQUIPMENT BORROWING SYSTEM\n");
        printf("----------------------------------------------\n");
        printf("[1.] Add Equipment\n");
        printf("[2.] Display Equipment\n");
        printf("[3.] Borrow Equipment\n");
        printf("[4.] Return Equipment\n");
        printf("[5.] Search Equipment\n");
        printf("[6.] Sort Equipment\n");
        printf("[7.] Process Borrow\n");
        printf("[8.] Display Borrow List\n");
        printf("[9.] Search by Category\n");
        printf("[10.] Filter Equipment\n");
        printf("[11.] Edit Equipment\n");
        printf("[12.] Delete Equipment\n");
        printf("[13.] Exit\n");
        printf("\n----------------------------------------------\n");
        printf("\nEnter choice: ");

if (scanf("%d", &choice) != 1) {
    printf("Invalid input! Please enter a number.\n");
    while (getchar() != '\n'); // clear buffer
    continue;
}

if (choice < 1 || choice > 13) {
    printf("Invalid choice! Please select between 1 and 13 only.\n");
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
                scanf("%s", name);

                int res = searchEquipment(name);
                if (res != -1)
                    printf("Found: %s (%d pcs)\n",
                           eq[res].name, eq[res].quantity);
                else
                    printf("Not found!\n");
                break;
            }

            case 6: sortEquipment(); break;
            case 7: processQueue(); break;
            case 8: displayQueue(); break;
            case 9: searchByCategory(); break;
            case 10: filterMenu(); break;
            case 11: editEquipment(); break;
            case 12: deleteEquipment(); break;
            case 13: exit(0);

            default: printf("Invalid choice!\n");
        }
    }
}
