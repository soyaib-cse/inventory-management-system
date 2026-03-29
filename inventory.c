#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

// ===================== PRODUCT STRUCT =====================
typedef struct Product {
    int id;
    char name[50];
    int quantity;
    float price;
} Product;

// ===================== STACK =====================
typedef struct Stack {
    Product data[100];
    int top;
} Stack;

void initStack(Stack *s) { s->top = -1; }
int isEmpty(Stack *s) { return s->top == -1; }
void push(Stack *s, Product p) { if (s->top < 99) s->data[++s->top] = p; }
Product pop(Stack *s) { return s->data[s->top--]; }

// ===================== HASH TABLE =====================
typedef struct HashNode {
    Product product;
    struct HashNode* next;
} HashNode;

HashNode* hashTable[TABLE_SIZE];

int hashFunction(int id) { return id % TABLE_SIZE; }

void insertHash(Product p) {
    int index = hashFunction(p.id);
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->product = p;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

void deleteHash(int id) {
    int index = hashFunction(id);
    HashNode *temp = hashTable[index], *prev = NULL;
    while (temp) {
        if (temp->product.id == id) {
            if (prev) prev->next = temp->next;
            else hashTable[index] = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

Product* searchHash(int id) {
    int index = hashFunction(id);
    HashNode* temp = hashTable[index];
    while (temp) {
        if (temp->product.id == id)
            return &temp->product;
        temp = temp->next;
    }
    return NULL;
}

// ===================== BST =====================
typedef struct BSTNode {
    Product product;
    struct BSTNode *left, *right;
} BSTNode;

BSTNode* createBSTNode(Product p) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    node->product = p;
    node->left = node->right = NULL;
    return node;
}

BSTNode* insertBST(BSTNode* root, Product p) {
    if (!root) return createBSTNode(p);
    if (p.id < root->product.id)
        root->left = insertBST(root->left, p);
    else
        root->right = insertBST(root->right, p);
    return root;
}

BSTNode* findMin(BSTNode* root) {
    while (root->left) root = root->left;
    return root;
}

BSTNode* deleteBST(BSTNode* root, int id) {
    if (!root) return NULL;

    if (id < root->product.id)
        root->left = deleteBST(root->left, id);
    else if (id > root->product.id)
        root->right = deleteBST(root->right, id);
    else {
        if (!root->left) {
            BSTNode* temp = root->right;
            free(root);
            return temp;
        }
        else if (!root->right) {
            BSTNode* temp = root->left;
            free(root);
            return temp;
        }
        BSTNode* temp = findMin(root->right);
        root->product = temp->product;
        root->right = deleteBST(root->right, temp->product.id);
    }
    return root;
}

void inorder(BSTNode* root) {
    if (!root) return;
    inorder(root->left);
    printf("\nID:%d Name:%s Qty:%d Price:%.2f\n", root->product.id, root->product.name, root->product.quantity, root->product.price);
    inorder(root->right);
}

// ===================== HEAP =====================
Product heap[100];
int heapSize = 0;

void swap(Product *a, Product *b) {
    Product temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(int i) {
    while (i > 0 && heap[(i - 1)/2].quantity < heap[i].quantity) {
        swap(&heap[i], &heap[(i - 1)/2]);
        i = (i - 1)/2;
    }
}

void heapifyDown(int i) {
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if (left < heapSize && heap[left].quantity > heap[largest].quantity)
        largest = left;
    if (right < heapSize && heap[right].quantity > heap[largest].quantity)
        largest = right;

    if (largest != i) {
        swap(&heap[i], &heap[largest]);
        heapifyDown(largest);
    }
}

void insertHeap(Product p) {
    heap[heapSize] = p;
    heapifyUp(heapSize);
    heapSize++;
}

void deleteHeap(int id) {
    for (int i = 0; i < heapSize; i++) {
        if (heap[i].id == id) {
            heap[i] = heap[--heapSize];
            heapifyDown(i);
            return;
        }
    }
}

void displayHeap() {
    for (int i = 0; i < heapSize; i++)
        printf("\nID:%d Name:%s Qty:%d\n", heap[i].id, heap[i].name, heap[i].quantity);
}

// ===================== MAIN =====================
void menu() {
    printf("\n===== Inventory Management System =====\n");
    printf("1. Add Product\n");
    printf("2. Search Product\n");
    printf("3. Display Products\n");
    printf("4. Show Top Stock\n");
    printf("5. Undo Last Add\n");
    printf("0. Exit\n");
}

int main() {
    BSTNode* root = NULL;
    Stack undoStack;
    initStack(&undoStack);

    int choice;

    while (1) {
        menu();
        printf ("\nEnter your choice:");
        scanf("%d", &choice);

        if (choice == 0) break;

        Product p;
        int id;

        switch (choice) {
            case 1:
                printf("\nEnter Product ID: ");
                scanf("%d", &p.id);

                printf("\nEnter Product Name: ");
                scanf("%s", p.name);

                printf("\nEnter Quantity: ");
                scanf("%d", &p.quantity);

                printf("\nEnter Price: ");
                scanf("%f", &p.price);

                insertHash(p);
                root = insertBST(root, p);
                insertHeap(p);
                push(&undoStack, p);

                printf("\nAdded!\n");
                break;

            case 2:
                printf("\nEnter ID: ");
                scanf("%d", &id);
                Product* found = searchHash(id);
                if (found)
                    printf("\nFound: %s %d %.2f\n", found->name, found->quantity, found->price);
                else
                    printf("\nNot Found\n");
                break;

            case 3:
                inorder(root);
                break;

            case 4:
                displayHeap();
                break;

            case 5:
                if (!isEmpty(&undoStack)) {
                    Product last = pop(&undoStack);

                    deleteHash(last.id);
                    root = deleteBST(root, last.id);
                    deleteHeap(last.id);

                    printf("\nUndo Successful (ID:%d removed)\n", last.id);
                } else {
                    printf("\nNothing to undo\n");
                }
                break;
        }
    }

    return 0;
}
