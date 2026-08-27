#include <stdio.h>
#include <stdlib.h>

#define T 3             // Minimum degree
#define MAX (2 * T - 1) // Maximum keys per node (5)

struct Node {
    int keys[MAX];
    struct Node *child[MAX + 1];
    int n;       // Number of keys
    int isLeaf;  // 1 if leaf, 0 if internal node
};

// Create a new node
struct Node* createNode(int isLeaf) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->n = 0;
    node->isLeaf = isLeaf;
    for (int i = 0; i <= MAX; i++) {
        node->child[i] = NULL;
    }
    return node;
}

// Traverse and print all keys in sorted order
void traverse(struct Node* root) {
    if (root == NULL) return;
    for (int i = 0; i < root->n; i++) {
        if (!root->isLeaf) traverse(root->child[i]);
        printf("%d ", root->keys[i]);
    }
    if (!root->isLeaf) traverse(root->child[root->n]);
}

// Search for a key
struct Node* search(struct Node* root, int k) {
    if (root == NULL) return NULL;
    int i = 0;
    while (i < root->n && k > root->keys[i]) i++;
    if (i < root->n && root->keys[i] == k) return root;
    if (root->isLeaf) return NULL;
    return search(root->child[i], k);
}

// Split a full child node (y) into two
void splitChild(struct Node* parent, int i, struct Node* y) {
    struct Node* z = createNode(y->isLeaf);
    z->n = T - 1; // z gets (T - 1) keys

    // Copy right half of keys from y to z
    for (int j = 0; j < T - 1; j++)
        z->keys[j] = y->keys[j + T];

    // Copy children pointers if not leaf
    if (!y->isLeaf) {
        for (int j = 0; j < T; j++)
            z->child[j] = y->child[j + T];
    }
    y->n = T - 1;

    // Shift parent's children to insert z
    for (int j = parent->n; j >= i + 1; j--)
        parent->child[j + 1] = parent->child[j];
    parent->child[i + 1] = z;

    // Shift parent's keys to move middle key of y up
    for (int j = parent->n - 1; j >= i; j--)
        parent->keys[j + 1] = parent->keys[j];
    parent->keys[i] = y->keys[T - 1];

    parent->n++;
}

// Insert into a non-full node
void insertNonFull(struct Node* node, int k) {
    int i = node->n - 1;

    if (node->isLeaf) {
        // Shift keys right to insert k in sorted place
        while (i >= 0 && node->keys[i] > k) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = k;
        node->n++;
    } else {
        // Find which child to descend into
        while (i >= 0 && node->keys[i] > k) i--;
        i++;

        // Split child if full before descending
        if (node->child[i]->n == MAX) {
            splitChild(node, i, node->child[i]);
            if (node->keys[i] < k) i++;
        }
        insertNonFull(node->child[i], k);
    }
}

// Main insert wrapper
void insert(struct Node** root, int k) {
    if (*root == NULL) {
        *root = createNode(1);
        (*root)->keys[0] = k;
        (*root)->n = 1;
        return;
    }

    // If root is full, make a new root and split old root
    if ((*root)->n == MAX) {
        struct Node* s = createNode(0);
        s->child[0] = *root;
        splitChild(s, 0, *root);

        int i = (s->keys[0] < k) ? 1 : 0;
        insertNonFull(s->child[i], k);
        *root = s;
    } else {
        insertNonFull(*root, k);
    }
}

// ==========================================
// MAIN DRIVER
// ==========================================
int main() {
    struct Node* root = NULL;

    // Sample inputs
    int keys[] = {10, 20, 5, 6, 12, 30, 7, 17};
    int n = sizeof(keys) / sizeof(keys[0]);

    printf("Inserting: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", keys[i]);
        insert(&root, keys[i]);
    }

    printf("\n\nB-Tree Traversal: ");
    traverse(root);
    printf("\n");

    // Search Demonstration
    int searchKey = 6;
    printf("\nSearch (%d): %s\n", searchKey, 
           search(root, searchKey) ? "Found in Tree" : "Not Found");

    return 0;
}