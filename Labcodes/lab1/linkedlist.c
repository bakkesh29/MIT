#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* next;
};

struct Node* top = NULL;

int isEmpty() {
    return top == NULL;
}

void push(int val) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("[Overflow] Memory allocation failed!\n");
        return;
    }
    newNode->data = val;
    newNode->next = top;
    top = newNode;
    printf("Pushed: %d\n", val);
}

void pop() {
    if (isEmpty()) {
        printf("[Underflow] Stack is empty! Cannot pop.\n");
        return;
    }
    struct Node* temp = top;
    printf("Popped: %d\n", temp->data);
    top = top->next;
    free(temp);
}

void peek() {
    if (isEmpty()) {
        printf("Stack is empty!\n");
        return;
    }
    printf("Top element: %d\n", top->data);
}

void multipop(int k) {
    if (isEmpty()) {
        printf("[Underflow] Stack is empty! Cannot multipop.\n");
        return;
    }
    printf("Multipop: ");
    int count = 0;
    while (!isEmpty() && count < k) {
        struct Node* temp = top;
        printf("%d ", temp->data);
        top = top->next;
        free(temp);
        count++;
    }
    printf("\nTotal popped: %d\n", count);
}

void display() {
    if (isEmpty()) {
        printf("Stack is empty.\n");
        return;
    }
    struct Node* temp = top;
    printf("Stack (Top to Bottom): ");
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}

int main() {
    int choice, val, k;

    while (1) {
        printf("\n--- LINKED LIST STACK MENU ---\n");
        printf("1. Push\n");
        printf("2. Pop\n");
        printf("3. Peek (Top)\n");
        printf("4. Multipop\n");
        printf("5. Display\n");
        printf("6. Check if Empty\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter value to push: ");
                scanf("%d", &val);
                push(val);
                break;
            case 2:
                pop();
                break;
            case 3:
                peek();
                break;
            case 4:
                printf("Enter number of elements to pop: ");
                scanf("%d", &k);
                multipop(k);
                break;
            case 5:
                display();
                break;
            case 6:
                if (isEmpty()) printf("Stack is EMPTY.\n");
                else printf("Stack is NOT empty.\n");
                break;
            case 7:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
}