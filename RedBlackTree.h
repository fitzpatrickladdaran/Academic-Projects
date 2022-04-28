/*
RedBlackTree.h

Red-Black Tree template class that implements an Ordered Map

Authour: Fitz Laddaran
Contact: fitzladdaran@gmail.com
Last Updated: 27/08/2020
*/

#pragma once
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

template <class T, class U>
class NodeT {
	
public:

	T key; // unique value, occurs only once in tree
	U value; // associated value with key, non-unique
	NodeT<T, U>* left; // left child pointer
	NodeT<T, U>* right; // right child pointer
	NodeT<T, U>* parent; // parent pointer
	bool isBlack; // checks the colour of a node

	// constructor
	NodeT(T data1, U data2) { 
		key = data1;
		value = data2;
		left = nullptr;
		right = nullptr;
		parent = nullptr;
		isBlack = false;
	}

};

template <class T, class U>
class RedBlackTree {

public: 

	RedBlackTree(); // constructor
	RedBlackTree(const RedBlackTree& rbTree); // copy constructor
	RedBlackTree& operator=(const RedBlackTree& rbTree); // overloaded assignment operator
	~RedBlackTree(); // destructor

	// inserts a node, if key is not present in R-B Tree
	bool insert(const T keyP, const U valueP);

	// removes a node, if key is present in R-B Tree
	bool remove(const T keyP);

	// search R-B Tree to see if key matches any of the nodes
	bool search(const T keyP) const; 

	// returns all values whose keys are between keyP1 - keyP2
	// based on ascending order of keys
	vector<U> search(const T keyP1, const T keyP2) const;

	// returns all values in the tree
	// based on ascending order of keys
	vector<U> values() const;

	// returns all keys in the tree
	vector<T> keys() const;

	// returns number of items stored in the tree
	int size();

	// returns a pointer to tree's root node
	NodeT<T, U>* getRoot() const;

	// in-order print
	void inOrderPrint();

private:

	// attributes
	NodeT<T, U>* root; // pointer to tree's root
	int currSize; // size of tree

	// helper functions 
	NodeT<T, U>* copy(NodeT<T, U>* nd, NodeT<T, U>* & newParent); // deep copy every node in the tree
	void clear(); // deallocates memory and sets root to NULL
	void clear(NodeT<T, U>* nd); // deallocates dynamic memory
	NodeT<T, U>* BSTinsert(const T keyP, const U valueP); // BST Insert method
	NodeT<T, U>* predecessor(NodeT<T, U>* nd) const; // finds the predecessor 
	void rbFix(NodeT<T, U>* nd, bool isLeafCheck); // RB Tree Fix algorithm
	int size(NodeT<T, U>* nd) const; // counts the number of nodes in the tree
	bool empty() const; // checks if the tree is empty or not
	void leftRotate(NodeT<T, U>* newNode); // left rotation on newNode
	void rightRotate(NodeT<T, U>* newNode); // right rotation on newNode
	void inOrderSearch(NodeT<T, U>* nd, vector<U>& myVect, const T keyP1, const T keyP2) const; // in-order traversal for search()
	void inOrderValues(NodeT<T, U>* nd, vector<U>& myVect) const; // in-order traversal for values()
	void inOrderKeys(NodeT<T, U>* nd, vector<T>& myVect) const; // in-order traversal for keys()
	void inOrderP(NodeT<T, U>* nd); // in-order print

};

// constructor
template <class T, class U>
RedBlackTree<T, U>::RedBlackTree() {

	root = nullptr;
	currSize = 0;

}

// copy constructor
template <class T, class U>
RedBlackTree<T, U>::RedBlackTree(const RedBlackTree& rbTree) {

	NodeT<T, U>* newParent = nullptr;
	root = copy(rbTree.root, newParent);

}

// overloaded assignment operator
template <class T, class U>
RedBlackTree<T, U>& RedBlackTree<T, U>::operator=(const RedBlackTree& rbTree)
{

	/*
	If calling object is the same as parameter,
	then the assignment operator does nothing.
	*/

	// if it is not self-assignment
	if (this != &rbTree) {

		// deallocate memory associated with tree nodes
		this->clear();

		// deep copy
		NodeT<T, U>* newParent = nullptr;
		root = copy(rbTree.root, newParent);

	}

	// return reference to calling object
	return *this;

}

// destructor
template <class T, class U>
RedBlackTree<T, U>::~RedBlackTree() {

	clear();

}

// inserts node if key is not in R-B Tree and return true
// otherwise return false without insertion
template <class T, class U>
bool RedBlackTree<T, U>::insert(const T keyP, const U valueP) {

	// if keyP is found, then return false without insertion 
	if (search(keyP)) {
		return false;
	}

	// if keyP is not found, then return true with insertion
	else {

		NodeT<T, U>* newNode = BSTinsert(keyP, valueP); // pointer to inserted node

		// while newNode is not the root and its parents are red
		while (newNode != root && newNode->parent->isBlack == false) {

			// if newNode's parent is a left child
			if (newNode->parent == newNode->parent->parent->left) {

				// uncle of newNode, sibling of newNode's parents
				NodeT<T, U>* uncle = newNode->parent->parent->right;

				// if uncle is red (ensure it is not NULL to have a colour)
				if (uncle != nullptr && uncle->isBlack == false) {

					newNode->parent->isBlack = true;
					uncle->isBlack = true;
					newNode->parent->parent->isBlack = false;
					newNode = newNode->parent->parent;

				}

				// if uncle is NULL (hence it has a black colour)
				else {

					if (newNode == newNode->parent->right) {

						newNode = newNode->parent;
						leftRotate(newNode);

					}

					newNode->parent->isBlack = true;
					newNode->parent->parent->isBlack = false;
					rightRotate(newNode->parent->parent);

				}
			}

			// if newNode's parent is a right child
			else {

				// uncle of newNode, sibling of newNode's parents
				NodeT<T, U>* uncle = newNode->parent->parent->left;

				// if uncle is red (ensure it is not NULL to have a colour)
				if (uncle != nullptr && uncle->isBlack == false) {

					newNode->parent->isBlack = true;
					uncle->isBlack = true;
					newNode->parent->parent->isBlack = false;
					newNode = newNode->parent->parent;

				}

				// if uncle is NULL (hence it has a black colour)
				else {

					if (newNode == newNode->parent->left) {

						newNode = newNode->parent;
						rightRotate(newNode);

					}

					newNode->parent->isBlack = true;
					newNode->parent->parent->isBlack = false;
					leftRotate(newNode->parent->parent);

				}
			}
		}

		root->isBlack = true;
		return true;

	}
}

// removes a node if key is not in R-B Tree and return true
// otherwise return false without removal
template <class T, class U>
bool RedBlackTree<T, U>::remove(const T keyP) {

	NodeT<T, U>* current = root; // iterator

	// if tree is empty
	if (root == nullptr) {
		return false; 
	}

	// iterate through tree
	while (current->key != keyP) {

		// if keyP is not found, then return false without removal

		// descend left child
		if (keyP < current->key) {

			if (current->left == nullptr) {
				return false;
			}

			current = current->left;

		}

		// descend right child
		else {

			if (current->right == nullptr) {
				return false;
			}

			current = current->right;

		}
	}

	// keyP is found, therefore return true after removal 

	NodeT<T, U>* ndRemove = nullptr; // pointer to physical node to be removed
	NodeT<T, U>* ndRemoveChild = nullptr; // ndRemove's child (can be NULL)
	bool isLeaf = false; // checks if ndRemoveChild is a leaf
	bool leftCheck = false; // checks if ndRemoveChild is a left child
	bool rightCheck = false; // checks if ndRemoveChild is a right child

	// if current has one or no children
	if (current->left == nullptr || current->right == nullptr) {
		ndRemove = current; // current node is to be physically removed
	}

	// if current has two children
	else {
		ndRemove = predecessor(current); // predecessor node is to be physically removed
	}

	// if ndRemove doesn't have a child
	if (ndRemove->left == nullptr && ndRemove->right == nullptr) {

		ndRemoveChild = new NodeT<T, U>(current->key, current->value); // initialize object for NULL node
		ndRemoveChild->isBlack = true; // NULL has a black colour
		isLeaf = true; // ndRemove is a leaf

	}

	// if ndRemove's only child is a left child
	else if (ndRemove->left != nullptr) {
		ndRemoveChild = ndRemove->left;
	}

	// if ndRemove's only child is a right child
	else {
		ndRemoveChild = ndRemove->right;
	}

	// attaching ndRemovechild to ndRemove's parent
	ndRemoveChild->parent = ndRemove->parent;

	// if ndRemove is the root
	if (ndRemove->parent == nullptr) {
		root = ndRemoveChild;
	}

	// if ndRemove is not the root, attach ndRemove's parent to ndRemoveChild
	else {

		// ndRemove is a left child
		if (ndRemove == ndRemove->parent->left) {

			ndRemove->parent->left = ndRemoveChild;
			leftCheck = true;

		}

		// ndRemove is a right child
		else {

			ndRemove->parent->right = ndRemoveChild;
			rightCheck = true;

		}
	}

	// ndRemove has, conceptually, been moved up
	// color is not moved
	if (ndRemove != current) {

		current->key = ndRemove->key;
		current->value = ndRemove->value;

	}

	// need to fix RB Tree if ndRemove is black
	// need to preserve RB Tree "black height" property
	if (ndRemove->isBlack == true) {
		rbFix(ndRemoveChild, isLeaf); // ndRemoveChild can be NULL
	}

	// to detach ndRemoveChild since it's a NULL NODE
	if (isLeaf == true) {

		// if ndRemoveChild is a left NULL node
		if (leftCheck) {
			ndRemoveChild->parent->left = nullptr;
		}

		// if ndRemoveChild is a right NULL node
		else if (rightCheck) {
			ndRemoveChild->parent->right = nullptr;
		}

		// detach ndRemoveChild from parent,
		// therefore eliminating all relationships with the RB Tree
		ndRemoveChild->parent = nullptr;
		delete ndRemoveChild; // delete temporary node
	} 

	else {
		delete ndRemove; // delete physical node removed
	}

	return true;

}

// search R-B Tree to see if key matches any node's key
// return true if found, otherwise false
template <class T, class U>
bool RedBlackTree<T, U>::search(const T keyP) const {

	NodeT<T, U>* current = root; // iterator

	// loop until iterator is NULL
	while (current != nullptr) {

		// if key-parameter = current's key
		if (keyP == current->key) {
			return true;
		}

		// if key-parameter < current's key
		if (keyP < current->key) {
			current = current->left;
		}

		// if key-parameter > current's key
		else {
			current = current->right;
		}

	}

	// if key is not found
	return false;

}

// returns a vector containing all values whose keys are between 
// keyP1 - keyP2, based on ascending key order
template <class T, class U>
vector<U> RedBlackTree<T, U>::search(const T keyP1, const T keyP2) const {

	vector<U> myVect;
	inOrderSearch(root, myVect, keyP1, keyP2);
	return myVect;

}

// returns a vector containing all values in ascending key oder
// if tree is empty, vector is also empty
template <class T, class U>
vector<U> RedBlackTree<T, U>::values() const {

	vector<U> myVect;
	inOrderValues(root, myVect);
	return myVect;

}

// returns a vector containing all keys in ascending order
// if tree is empty, vector is also empty
template <class T, class U>
vector<T> RedBlackTree<T, U>::keys() const {

	vector<T> myVect;
	inOrderKeys(root, myVect);
	return myVect;

}

// returns the number of items stored in the tree
template <class T, class U>
int RedBlackTree<T, U>::size() {

	NodeT<T, U>* current = root; // iterator

	if (current == nullptr) {
		currSize = 0;
	}

	else {
		currSize = size(current);
	}

	return currSize;
}

// returns a pointer to the root
template <class T, class U>
NodeT<T, U>* RedBlackTree<T, U>::getRoot() const {
	
	return root;

}

// HELPER FUNCTION: copies every node in tree (pre-order traversal)
// USED BY: copy constructor, overloaded assignment operator
template <class T, class U>
NodeT<T, U>* RedBlackTree<T, U>::copy(NodeT<T, U>* nd, NodeT<T, U>*& newParent) {

	// if node is not NULL
	if (nd != nullptr) {

		NodeT<T, U>* newNode = new NodeT<T, U>(nd->key, nd->value); // create new node for parameter node
		newNode->isBlack = nd->isBlack; // copy colour attribute
		newNode->parent = newParent; // copy parent attribute
		newNode->left = copy(nd->left, newNode); // recursively copy left descendants 
		newNode->right = copy(nd->right, newNode); // recursively copy right descendants 
		return newNode;

	}

	// if node is NULL
	return nullptr;
}

// HELPER FUNCTION: calls clear(NodeT* nd), and sets root = NULL
// USED BY: destructor, overloaded assignment operator 
template <class T, class U>
void RedBlackTree<T, U>::clear() {

	clear(root);
	root = nullptr;

}

// HELPER FUNCTION: removes all nodes and deallocates dynamic memory for every node
// USED BY: clear()
template <class T, class U>
void RedBlackTree<T, U>::clear(NodeT<T, U>* nd) {

	// if node is not NULL
	if (nd != nullptr) {

		clear(nd->left); // recursively check if there's a left descendant
		clear(nd->right); // recursively check if there's a right descendant
		delete nd; // if no descendants, delete current node

	}

}

// HELPER FUNCTION: inserts a node in the appropriate location in a tree
// USED BY: insert()
template <class T, class U>
NodeT<T, U>* RedBlackTree<T, U>::BSTinsert(const T keyP, const U valueP) {

	NodeT<T, U>* newNode = new NodeT<T, U>(keyP, valueP); // create new node
	NodeT<T, U>* parent = root; // keep track of parent of the node we want to insert
	NodeT<T, U>* next = root; // to determine what is ahead of current node

	// if tree is empty
	if (empty()) {
		root = newNode;
	}

	// if tree is not empty
	else {

		// finding parent of new node
		while (next != nullptr) {

			parent = next; // keep track of parent

			// descend left-subtree
			if (keyP < parent->key) {
				next = parent->left;
			}

			// descend right-subtree
			else {
				next = parent->right;
			}

		}

		// insert new node
		if (keyP < parent->key) {

			parent->left = newNode; // left-child
			newNode->parent = parent; // new node's parent is matched

		}

		else {

			parent->right = newNode; // right-child
			newNode->parent = parent; // new node's parent is matched

		}

	}

	return newNode;
}

// HELPER FUNCTION: finds the predecessor 
// USED BY: remove()
template <class T, class U>
NodeT<T, U>* RedBlackTree<T, U>::predecessor(NodeT<T, U>* nd) const {

	// enter left sub-tree
	nd = nd->left;

	// find right-most node (predecessor)
	while (nd->right != nullptr) {
		nd = nd->right;
	}

	return nd;

}

// HELPER FUNCTION: fixes the RB Tree
// USED BY: remove()
template <class T, class U>
void RedBlackTree<T, U>::rbFix(NodeT<T, U>* nd, bool isLeafCheck) {

	NodeT<T, U>* ndCopy = nd; // copy of ndRemoveChild, to be deleted if a NULL node

	// if ndRemoveChild is not the root and is black
	while (nd != root && nd->isBlack == true) {

		// ndRemoveChild is a left child
		if (nd == nd->parent->left) {

			NodeT<T, U>* sibling = nd->parent->right; // ndRemoveChild's sibling

			// if sibling is red
			if (sibling->isBlack == false) {

				/*
				black height property is maintained,
				but ndRemoveChild's sibling is now black
				*/

				sibling->isBlack = true;
				nd->parent->isBlack = false;
				leftRotate(nd->parent);
				sibling = nd->parent->right;

			}

			if ((sibling->left != nullptr && sibling->left->isBlack == false) ||
				(sibling->right != nullptr && sibling->right->isBlack == false)) {

				// if sibling's right child is black 
				if (sibling->right == nullptr || sibling->right->isBlack == true) {

					// makes ndRemoveChild's sibling's right child red
					sibling->left->isBlack = true;
					sibling->isBlack = false;
					rightRotate(sibling);
					sibling = nd->parent->right;

				}

				sibling->isBlack = sibling->parent->isBlack;
				nd->parent->isBlack = true;
				sibling->right->isBlack = true;
				leftRotate(sibling->parent);
				nd = root;

			}

			else {

				sibling->isBlack = false; // sibling is now red
				nd = nd->parent; // push the fix up the tree 

			}
		}

		// ndRemoveChild is a right child
		else {

			NodeT<T, U>* sibling = nd->parent->left; // ndRemoveChild's sibling

			// if sibling is red
			if (sibling->isBlack == false) {

				/*
				black height property is maintained,
				but ndRemoveChild's sibling is now black
				*/

				sibling->isBlack = true;
				nd->parent->isBlack = false;
				rightRotate(nd->parent);
				sibling = nd->parent->left;

			}

			if ((sibling->left != nullptr && sibling->left->isBlack == false) ||
				(sibling->right != nullptr && sibling->right->isBlack == false)) {

				// if sibling's left child is black 
				if (sibling->left == nullptr || sibling->left->isBlack == true) {

					// makes ndRemoveChild's sibling's left child red
					sibling->right->isBlack = true;
					sibling->isBlack = false;
					leftRotate(sibling);
					sibling = nd->parent->left;

				}

				sibling->isBlack = sibling->parent->isBlack;
				nd->parent->isBlack = true;
				sibling->left->isBlack = true;
				rightRotate(sibling->parent);
				nd = root;

			}

			else {

				sibling->isBlack = false; // sibling is now red
				nd = nd->parent; // push the fix up the tree 

			}
		}
	}

	// delete NULL nodes
	if (ndCopy->parent != nullptr && isLeafCheck == true) {
		
		if (ndCopy == ndCopy->parent->left) {
			ndCopy->parent->left = nullptr;
		}

		else {
			ndCopy->parent->right = nullptr;
		}
	} 
	
	// case: occurs when removing a node and it's the only one in the tree
	else if (ndCopy->parent == nullptr && isLeafCheck == true) {
		
		root = nullptr;
		
	}

	// change to black to satisfy "black height" property
	nd->isBlack = true;

}

// HELPER FUNCTION: counts the number of nodes stored in a tree
// USED BY: size()
template <class T, class U>
int RedBlackTree<T, U>::size(NodeT<T, U>* nd) const{

	int count = 1;

	if (nd->left != nullptr) {
		count += size(nd->left);
	}

	if (nd->right != nullptr) {
		count += size(nd->right);
	}

	return count;
}

// HELPER FUNCTION: checks if the tree is empty or not
// USED BY: BSTinsert()
template <class T, class U>
bool RedBlackTree<T, U>::empty() const{

	return root == nullptr;

}

// HELPER FUNCTION: left-rotate on newNode
// USED BY: insert(), remove()
template <class T, class U>
void RedBlackTree<T, U>::leftRotate(NodeT<T, U>* newNode) {

	NodeT<T, U>* newParent = newNode->right; // newNode's soon-to-be new parent
	newNode->right = newParent->left; // attach newParent's left child as newNode's right child

	// if newParent's left child is not NULL
	if (newParent->left != nullptr) {
		newParent->left->parent = newNode; // newParent's left child's parent is now newNode
	}

	// attach newParent's new parent
	newParent->parent = newNode->parent;

	// newNode is the root
	if (newNode->parent == nullptr) {
		root = newParent;
	}

	// newNode is a left child
	else if (newNode == newNode->parent->left) {
		newNode->parent->left = newParent;
	}

	// newNode is a right child
	else {
		newNode->parent->right = newParent;
	}

	newParent->left = newNode; // attach new node as newParent's left child
	newNode->parent = newParent; // attach newParent as newNode's new parent
}

// HELPER FUNCTION: right-rotate on newNode
// USED BY: insert(), remove()
template <class T, class U>
void RedBlackTree<T, U>::rightRotate(NodeT<T, U>* newNode) {

	NodeT<T, U>* newParent = newNode->left; // newNode's soon-to-be new parent
	newNode->left = newParent->right; // attach newParent's right child as newNode's left child

	// if newParent's right child is not NULL
	if (newParent->right != nullptr) {
		newParent->right->parent = newNode; // newParent's left child's parent is now newNode
	}

	// attach newParent's new parent
	newParent->parent = newNode->parent;

	// newNode is the root
	if (newNode->parent == nullptr) {
		root = newParent;
	}

	// newNode is a left child
	else if (newNode == newNode->parent->left) {
		newNode->parent->left = newParent;
	}

	// newNode is a right child
	else {
		newNode->parent->right = newParent;
	}

	newParent->right = newNode; // attach new node as newParent's right child
	newNode->parent = newParent; // attach newParent as newNode's new parent
}

// HELPER FUNCTION: in-order traversal for search(T keyP1, T keyP2)
// USED BY: search(T keyP1, T keyP2)
template <class T, class U>
void RedBlackTree<T, U>::inOrderSearch(NodeT<T, U>* nd, vector<U>& myVect, const T keyP1, const T keyP2) const {

	if (nd != nullptr) {

		inOrderSearch(nd->left, myVect, keyP1, keyP2);

		if ((nd->key >= keyP1 && nd->key <= keyP2) || 
			(nd->key >= keyP2 && nd->key <= keyP1)) {
			myVect.push_back(nd->value);
		}

		inOrderSearch(nd->right, myVect, keyP1, keyP2);

	}
}

// HELPER FUNCTION: in-order traversal for values()
// USED BY: values()
template <class T, class U>
void RedBlackTree<T, U>::inOrderValues(NodeT<T, U>* nd, vector<U>& myVect) const {

	if (nd != nullptr) {

		inOrderValues(nd->left, myVect);
		myVect.push_back(nd->value);
		inOrderValues(nd->right, myVect);

	}

}

// HELPER FUNCTION: in-order traversal for keys()
// USED BY: keys()
template <class T, class U>
void RedBlackTree<T, U>::inOrderKeys(NodeT<T, U>* nd, vector<T>& myVect) const {

	if (nd != nullptr) {

		inOrderKeys(nd->left, myVect);
		myVect.push_back(nd->key);
		inOrderKeys(nd->right, myVect);

	}

}

// HELPER FUNCTION: check if tree is properly in-order
// USED BY: inOrderPrint()
template <class T, class U>
void RedBlackTree<T, U>::inOrderP(NodeT<T, U>* nd) {

	if (nd != nullptr) {

		inOrderP(nd->left);
		cout << "Key: " << nd->key << endl;
		cout << "Value: " << nd->value << endl;
		cout << "isBlack: " << nd->isBlack << endl;
		cout << "Parent: " << nd->parent << endl;
		cout << "Left: " << nd->left << endl;
		cout << "Right: " << nd->right << endl;
		cout << "Address: " << nd << endl;
		cout << endl;
		inOrderP(nd->right);

	}

}

// HELPER FUNCTION: in-order print
// USED BY: main() 
template <class T, class U>
void RedBlackTree<T, U>::inOrderPrint() {

	inOrderP(root);

}
