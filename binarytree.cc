/*
 * A simple binary tree serialization/deserialization implementation
 * Author: Wytze Hazeberg
 */

#include <iostream>

using namespace std;

#include <queue>


//Node structure
struct Node {
	size_t value;
	Node *leftChild;
	Node *rightChild;

	//constructor
	Node(size_t value) {
		this->value = value;
		leftChild=NULL;
		rightChild=NULL;
	};
};

void save(Node *root);
Node* load();
void freeTreeMem(Node *node);
void print(Node *root);
Node* dummyTree();


/**
 * main
 */
int main() {
	Node *root = dummyTree(); //generate tree
	print(root); //print dummy tree
	save(root); //save tree to binary file
	freeTreeMem(root); //free resources used

	root = load(); //load tree from binary file
	print(root); //print tree
	freeTreeMem(root); //free resources used
}


/**
 * Generate a dummy tree for demonstration purposes.
 * Tree will look like:
 *          1
 *        /   \
 *      2       3
 *    /   \   /   \
 *   4     5 6     78
 *
 * @return root Node
 */
Node* dummyTree() {
	//root
	Node *root = new Node(1);
	
	//1st level
	Node *two = new Node(2);
	Node *three = new Node(3);
	root->leftChild = two;
	root->rightChild = three;

	//2nd level
	Node *four = new Node(4);
	//Node *five = new Node(5); //commented b/c to simulate NULL values (!= full binary tree)
	Node *six = new Node(6);
	Node *seven = new Node(78);
	two->leftChild = four;
	//two->rightChild = five;
	three->leftChild = six;
	three->rightChild = seven;

	return root;
}

/**
 * Store tree on disk. Breadth-first traversal.
 * @param root Node
 */
void save(Node *root) {
	FILE *out;
    out = fopen("./tree.bin", "wb");
	size_t padding = 0; //value used to fill up the sequence to form a full binary tree.

	queue<Node*> Q;
	Q.push(root);

	while(Q.size()) {
		if (Q.front() == NULL) {
			fwrite(&padding, sizeof(size_t), 1, out);
			Q.pop();
			continue;
		}

		fwrite(&Q.front()->value, sizeof(size_t), 1, out);

		//leaf node?
		if (Q.front()->leftChild == NULL && Q.front()->rightChild == NULL) {
			Q.pop();
			continue;
		}
	
		//push left and right child onto the queue, if present.
		Q.push(Q.front()->leftChild == NULL ? NULL : Q.front()->leftChild);
		Q.push(Q.front()->rightChild == NULL ? NULL : Q.front()->rightChild);
		Q.pop();

	}

	fclose(out);
}

/**
 * load binary tree from file and place it in memory.
 * Note: this function allocates Node objects in memory, be ware to free these through function freeTreeMem.
 * @return root Node of the constructed tree
 */
Node* load() {
	FILE *in;
    in = fopen("./tree.bin","rb");
	queue<Node*> Q;
	size_t padding = 0; //value used to fill up the sequence to form a full binary tree.

	//fetch first value from file, which is the root
	size_t val;
	fread(&val, sizeof(size_t), 1, in);
	Node *root = new Node(val);
	Q.push(root);

	size_t left, right;
	Node *currentNode;
	while(fread(&left, sizeof(size_t), 1, in)) {
		fread(&right, sizeof(size_t), 1, in);

		currentNode = Q.front();
		Q.pop();

		if (left != padding) {
			currentNode->leftChild = new Node(left);
			Q.push(currentNode->leftChild);
		}
		if (right != padding) {
			currentNode->rightChild = new Node(right);
			Q.push(currentNode->rightChild);
		}
	}
	//NOTE: the leaf nodes are still stored in Q because fread is exhausted.

    fclose(in);
	return root;
}

/**
 * Free internal memory that was used to store the tree using post-order traversal.
 * @param Node pointer to root node
 */
void freeTreeMem(Node *node) {
	if (node->leftChild != NULL) freeTreeMem(node->leftChild);
	if (node->rightChild != NULL) freeTreeMem(node->rightChild);
	delete(node);
}

/**
 * Print tree to std out, nothing fancy. Breadth-first traversal.
 * @param root Node
 */
void print(Node *root) {
	queue<Node*> Q;
    Q.push(root);

    while(Q.size()) {
        if (Q.front() == NULL) {
            cout << "NULL\n";
            Q.pop();
            continue;
        } else {
            cout << Q.front()->value << endl;
        }

        //leaf node?
        if (Q.front()->leftChild == NULL && Q.front()->rightChild == NULL) {
            Q.pop();
            continue;
        }

        Q.push(Q.front()->leftChild == NULL ? NULL : Q.front()->leftChild);
        Q.push(Q.front()->rightChild == NULL ? NULL : Q.front()->rightChild);

        Q.pop();
    }
}
