/*
Deque.h

Deque template class using a Doubly Linked List

Authour: Fitz Laddaran
Contact: fitzladdaran@gmail.com
Last Updated: 27/08/2020
*/

#pragma once
#include <iostream>
#include <stdexcept>
using std::runtime_error;
using std::cout;
using std::endl;

template <class T>
class Node 
{
public:

	T data;
	Node<T>* next;
	Node<T>* prev;

	Node(T value) {
		this->data = value;
		this->next = nullptr;
		this->prev = nullptr;
	}
	
};

template <class T>
class Deque
{
public:
	Deque(); // constructor
	Deque(const Deque& myDeq); // copy constructor
	~Deque(); // destructor
	Deque& operator=(const Deque& myDeq); // overloaded assignment operator
	void insertFront(T value); // insert parameter at the front of Deque
	void insertBack(T value); // insert parameter at the back of Deque
	T removeFront(); // removes and returns the value at the front of Deque
	T removeBack(); // removes and returns the value at the back of Deque
	T peekFront() const; // returns the value at the front of Deque without removing it
	T peekBack() const; // returns the value at the back of Deque without removing it
	bool empty() const; // returns true if Deque is empty
	int size() const; // returns number of items stored in Deque
	void display() const; // prints Deque contents

private:
	Node<T>* front;
	Node<T>* back;
};


// constructor
template <class T>
Deque<T>::Deque() {

	// initialized to null since it's an empty deque
	front = nullptr;
	back = nullptr;

}

// copy constructor
template <class T>
Deque<T>::Deque(const Deque& myDeq) {

	// initialize front and back to NULL in case myDeq is empty
	front = nullptr;
	back = nullptr;

	// iterator starting from beginning of myDeq
	Node<T>* nodeptr = myDeq.front;

	// if myDeq contains at least one node
	if (nodeptr != nullptr) {

		// copy "front" node of deque
		front = new Node<T>(nodeptr->data);

		// if there's only one node in myDeq
		if (nodeptr->next == nullptr) {
			back = front; // back and front point to the same node
		}

		nodeptr = nodeptr->next; // iterator points to next node
		Node<T>* copy = front; // iterator for our copied deque

		// copy the rest of myDeq 
		while (nodeptr != nullptr) {

			// iterating 
			Node<T>* newNode = new Node<T>(nodeptr->data);
			copy->next = newNode;
			newNode->prev = copy;
			copy = copy->next;
			

			// point "back" to last node
			if (nodeptr->next == nullptr) {
				back = copy;
			}

			nodeptr = nodeptr->next; // iterating

		}
	}
}

// destructor
template <class T>
Deque<T>::~Deque() {

	back = nullptr; // setting back to nullptr

	// iterating through calling object's nodes
	while (front != nullptr) {

		Node<T>* temp = front; // temp points to "front"
		front = front->next; // iterate "front"
		delete temp; // delete temp 
		temp = nullptr; // point temp to NULL

	}
}

// overloaded assignment operator
template <class T>
Deque<T>& Deque<T>::operator=(const Deque& myDeq) {

	/*
	If calling object is the same as parameter,
	then the assignment operator does nothing
	*/

	// if calling object is not the same as parameter 
	if (this != &myDeq) {

		// de-allocating dynamic memory associated 
		// with original contents of calling object
		Node<T>* thisptr = this->front; // ptr for calling obj's "front"

		// iterating through calling object's nodes
		while (thisptr != nullptr) {

			this->front = this->front->next; // "front" iterates
			delete thisptr; // delete current node
			thisptr = this->front; // iterator points to next node

		}

		// initialize front and back to NULL in case myDeq is empty
		front = nullptr;
		back = nullptr;

		// iterator starting from beginning of myDeq
		Node<T>* nodeptr = myDeq.front;

		// if myDeq contains at least one node
		if (nodeptr != nullptr) {

			// copy "front" node of deque
			front = new Node<T>(nodeptr->data);

			// if there's only one node in myDeq
			if (nodeptr->next == nullptr) {
				back = front; // back and front point to the same node
			}

			nodeptr = nodeptr->next; // iterator points to next node
			Node<T>* copy = front; // iterator for our copied deque

			// copy the rest of myDeq 
			while (nodeptr != nullptr) {

				// iterating 
				Node<T>* newNode = new Node<T>(nodeptr->data);
				copy->next = newNode;
				newNode->prev = copy;
				copy = copy->next;

				// point "back" to last node
				if (nodeptr->next == nullptr) {
					back = copy;
				}

				nodeptr = nodeptr->next; // iterating
			}

		}

		return *this; // return a reference to calling object

	}
}

// insert at front of Deque
template <class T>
void Deque<T>::insertFront(T value) {

	// pointer for a new node
	Node<T>* newNode = new Node<T>(value);

	// if calling obj is empty
	if (front == nullptr) {

		newNode->next = nullptr;
		newNode->prev = nullptr;
		front = newNode;
		back = newNode;

	}

	// if calling obj already has at least one node
	else {

		front->prev = newNode;
		newNode->next = front;
		newNode->prev = nullptr;
		front = newNode;

	}
}

// insert at back of Deque
template <class T>
void Deque<T>::insertBack(T value) {

	// pointer for a new node
	Node<T>* newNode = new Node<T>(value);

	// if calling obj is empty
	if (front == nullptr) {

		newNode->next = nullptr;
		newNode->prev = nullptr;
		front = newNode;
		back = newNode;

	}

	// if calling obj already has at least one node
	else {

		back->next = newNode;
		newNode->prev = back;
		newNode->next = nullptr;
		back = newNode;

	}
}

// removes and returns the value of the first node
template <class T>
T Deque<T>::removeFront() {

	// if calling obj is not empty 
	if (!empty()) {

		T returnVal; // variable to be returned
		returnVal = front->data;  // get data from calling obj's front
		Node<T>* temp = front; // temp points to front
		front = front->next; // front points to next node

		// if there's another node in the calling obj
		if (front != nullptr) {
			front->prev = nullptr;
		}

		// if the calling obj only contains one node
		else {
			back = nullptr;
		}

		delete temp; // delete front
		return returnVal;

	}

	// if calling object is empty
	else {
		throw runtime_error("Nothing to remove because Deque is empty.");
	}

}

// removes and returns the value of the last node
template <class T>
T Deque<T>::removeBack() {

	// if calling obj is not empty 
	if (!empty()) {

		T returnVal; // variable to be returned
		returnVal = back->data; // get data from calling obj's back
		Node<T>* temp = back; // temp points to back
		back = back->prev; // back points to prev node

		// if there's another node in the calling obj
		if (back != nullptr) {
			back->next = nullptr;
		}

		// if the calling obj only contains one node
		else {
			front = nullptr;
		}

		delete temp; // delete back
		return returnVal;
	}

	// if calling object is empty
	else {
		throw runtime_error("Nothing to remove because Deque is empty.");
	}
}

// returns value at Deque's "front"
template <class T>
T Deque<T>::peekFront() const {

	// if calling object is not empty
	if (!empty()) {
		return front->data;
	}

	// if calling object is empty
	else {
		throw runtime_error("Nothing to return because Deque is empty.");
	}

}

// returns value at Deque's "back"
template <class T>
T Deque<T>::peekBack() const {

	// if calling obj is not empty
	if (!empty()) {
		return back->data;
	}

	// if calling obj is empty
	else {
		throw runtime_error("Nothing to return because Deque is empty.");
	}

}

// check if Deque is empty
template <class T>
bool Deque<T>::empty() const {

	// if "front" points to NULL then it must be empty;
	// can also change "front" to "back"
	if (front == nullptr) {
		return true;
	}

	return false;

}

// counts Deque's size
template <class T>
int Deque<T>::size() const {

	// if calling obj is not empty
	if (!empty()) {

		Node<T>* temp = front;
		int count = 0;

		// iterate through calling obj
		while (temp != nullptr) {

			temp = temp->next;
			count++;

		}

		return count;

	}

	// if calling obj is empty 
	return 0;

}

// prints Deque contents
template <class T>
void Deque<T>::display() const {

	if (!empty()) {

		Node* temp = front;

		while (temp != nullptr) {

			cout << temp->data << " ";
			temp = temp->next;

		}

		cout << endl;
	}

	else {
		cout << "Deque is empty!" << endl;
	}

}
