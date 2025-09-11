/* Define the classes for tree elements, and prototypes of functions
    * Class for tree element
    * Class for tree
    * Function pointer for creating a tree
    * Function pointer for adding a node
    * Function pointer for deleting a node
    * Array of function pointers for traversing the tree in different orders
    * Function pointer for searching a node
    * Function pointer for deleting the entire tree
    * Function pointer for printing the tree
*/

typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

typedef struct Tree {
    TreeNode* root;
} Tree;

typedef Tree* (*CreateTreeFunc)();
typedef void (*AddNodeFunc)(Tree* tree, int data);
typedef void (*DeleteNodeFunc)(Tree* tree, int data);
typedef void (*TraverseFunc)(TreeNode* node);
typedef TraverseFunc TraverseFuncs[3]; // e.g., Preorder, Inorder, Postorder
typedef TreeNode* (*SearchNodeFunc)(Tree* tree, int data);
typedef void (*DeleteTreeFunc)(Tree* tree);
typedef void (*PrintTreeFunc)(Tree* tree);