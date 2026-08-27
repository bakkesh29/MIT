#include <stdio.h>//including the libraries 
#include <stdlib.h>
#define MAX 5
//declaring the variables 
int stack[MAX];
int top = -1;

//declaring the methods 
int isFull() {
    return top == MAX - 1;
}
int isEmpty() {
    return top == -1;
}
void push(int val) {
    if (isFull()) {
        printf("[Overflow] Stack is full! Cannot push %d\n", val);
        return;
    }
    top++;
    stack[top] = val;
    printf("Pushed: %d\n", val);
}
void pop() {
    if (isEmpty()) {
        printf("[Underflow] Stack is empty! Cannot pop.\n");
        return;
    }
    printf("Popped: %d\n", stack[top]);
    top--;
}

void peek() {
    if (isEmpty()) {
        printf("Stack is empty!\n");
        return;
    }
    printf("Top element: %d\n", stack[top]);
}
void multipop(int k) {
    if (isEmpty()) {
        printf("[Underflow] Stack is empty! Cannot multipop.\n");
        return;
    }
    printf("Multipop: ");
    int count = 0;
    while (!isEmpty() && count < k) {
        printf("%d ", stack[top]);
        top--;
        count++;
    }
    printf("\nTotal popped: %d\n", count);
}
void display() {
    if (isEmpty()) {
        printf("Stack is empty.\n");
        return;
    }
    printf("Stack (Top to Bottom): ");
    for (int i = top; i >= 0; i--) {
        printf("%d ", stack[i]);
    }
    printf("\n");
}

//executing starts here main method
int main() {
    int choice, val, k;
    //while loop
    while (1) {
        //if it is false
        printf("\n--- ARRAY STACK MENU ---\n");
        printf("1. Push\n");
        printf("2. Pop\n");
        printf("3. Peek (Top)\n");
        printf("4. Multipop\n");
        printf("5. Display\n");
        printf("6. Check if Empty\n");
        printf("7. Check if Full\n");
        printf("8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        //switch cases
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
                if (isFull()) printf("Stack is FULL.\n");
                else printf("Stack is NOT full.\n");
                break;
            case 8:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
}