#include <stdio.h>

#define MAX 100

int stack[MAX];
int top = -1;

int total_cost = 0;
int op_count = 0;

void push(int val) {
    if (top == MAX - 1) {
        printf("Stack Overflow\n");
        return;
    }
    top++;
    stack[top] = val;
    
    int cost = 1; // 1 push operation = cost 1
    total_cost += cost;
    op_count++;
    
    printf("Op #%d: PUSH(%d)       -> Actual Cost: %d | Total Cost: %d\n", 
           op_count, val, cost, total_cost);
}

void pop() {
    if (top == -1) {
        printf("Stack Underflow\n");
        return;
    }
    int popped = stack[top--];
    int cost = 1; // 1 pop operation = cost 1
    total_cost += cost;
    op_count++;

    printf("Op #%d: POP()          -> Actual Cost: %d | Total Cost: %d (Popped: %d)\n", 
           op_count, cost, total_cost, popped);
}

void multipop(int k) {
    int cost = 0;
    while (top != -1 && cost < k) {
        top--;
        cost++; // Cost increases by 1 for each element removed
    }
    total_cost += cost;
    op_count++;

    printf("Op #%d: MULTIPOP(%d)   -> Actual Cost: %d | Total Cost: %d (Removed: %d items)\n", 
           op_count, k, cost, total_cost, cost);
}

int main() {
    printf("===============================================================\n");
    printf("  EXPERIMENT 1: AGGREGATE ANALYSIS ON STACK WITH MULTIPOP     \n");
    printf("===============================================================\n\n");

    // Sequence of operations
    push(10);
    push(20);
    push(30);
    push(40);
    push(50);
    pop();
    multipop(3);
    push(60);
    multipop(5); // Stack has only 2 items (10, 60), will pop only 2

    // Aggregate Calculation
    printf("\n---------------------------------------------------------------\n");
    printf("Total Operations (n)       : %d\n", op_count);
    printf("Total Actual Cost (T(n))   : %d\n", total_cost);
    printf("Amortized Cost per Op      : %.2f (Total Cost / n)\n", (float)total_cost / op_count);
    printf("Conclusion                 : Amortized cost is bounded by O(1)\n");
    printf("---------------------------------------------------------------\n");

    return 0;
}