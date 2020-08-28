/*
statistics.cpp

The program stores doubles and calculates some basic statistics

Authour: Fitz Laddaran
Contact: fitzladdaran@gmail.com
Last Updated: 27/08/2020
*/

#include "statistics.h"
#include <algorithm>
#include <cmath>
#include <iostream>;
using std::sort;
using std::pow;
using std::sqrt;
using std::cout;
using std::endl;

// default constructor: creates array of size 2 in dynamic memory
Sequence::Sequence() {

	maxSize = 2; // setting maximum size to 2
	currSize = 0; // setting current size to 0
	arr = new double[maxSize]; // creating array of size 2 in dynamic memory

}

// copy constructor
Sequence::Sequence(const Sequence& mySeq) {
	
	// copying sizes of mySeq
	maxSize = mySeq.maxSize; // copying maximum size of input parameter
	currSize = mySeq.currSize; // copying current size of input parameter
	arr = new double[maxSize]; // initializing array to be size maxSize

	// copying elements of mySeq
	for (int i = 0; i < currSize; ++i) {
		arr[i] = mySeq.arr[i];
	}
	
}

// destructor
Sequence::~Sequence() {

	delete[] arr; // deallocate memory
	arr = nullptr; // pointing arr to NULL

}

// overloaded assignment operator
Sequence& Sequence::operator=(const Sequence& mySeq) {

	Sequence tempSeq(mySeq); // copy constructor used to create a copy called tempSeq
	delete[] this->arr; // deallocating memory allocated for arr of calling object (left-side of operator)

	this->currSize = tempSeq.currSize; // setting calling object's currSize to tempSeq's currSize
	this->maxSize = tempSeq.maxSize; // setting calling object's maxSize to tempSeq's maxSize
	this->arr = new double[this->maxSize]; // allocating memory for calling object's arr

	// copying elements of tempSeq
	for (int i = 0; i < this->currSize; ++i) {
		this->arr[i] = tempSeq.arr[i];
	}
	
	return *this; // return a reference to calling object

}

// inserts value at next available index 
void Sequence::insert(double value) {

	// if array is full
	if (currSize == maxSize) {

		maxSize *= 2; // double maximum size
		double* newArr = new double[maxSize]; // creating an array of new maxSize

		// copying contents of old array to new array
		for (int i = 0; i < currSize; ++i) {
			newArr[i] = arr[i]; 
		}

		delete[] arr; // freeing memory associated with old array
		arr = newArr; // assigning new array's address to object's array pointer

	}

	arr[currSize++] = value; // inserting parameter

}

// inserts value of someArr at the end of calling object's array
void Sequence::insert(double someArr[], int someArrSize) {

	// if array is full 
	if ((currSize + someArrSize) > maxSize) {

		maxSize = currSize + someArrSize; // maxSize is changed to the equivalent of both arrays' currSize
		double* newArr = new double[maxSize]; // creating an array of new maxSize

		// copying contents of calling object's array
		for (int i = 0; i < currSize; ++i) {
			newArr[i] = arr[i];
		}

		delete[] arr; // freeing memory associated with calling object's array
		arr = newArr; // assigning new array's address to object's array pointer
	}

	// copying contents of parameter array into calling object's array
	for (int i = 0; i < someArrSize; ++i) {
		arr[currSize++] = someArr[i];
	}

}

// returns an integer equal to number of elements whose value is equal to parameter
int Sequence::find(double value) const {
	
	int count = 0; // counter: number of elements in arr equal to value

	// go through array to count how many elements are equal to value
	for (int i = 0; i < currSize; ++i) {
		if (arr[i] == value) {
			count++; // increment count
		}
	}

	return count; 

}

// returns an integer equal to number of values in calling object
int Sequence::size() const {

	return currSize; // return current size of array

}

// return a double equal to sum of values in calling object
double Sequence::sum() const {
	
	double sum = 0; // counter: sum of values in array

	// go through array to sum the values of all elements
	for (int i = 0; i < currSize; ++i) {
		sum += arr[i];
	}

	return sum; // return a double

}

// return a double equal to average of values in calling object
double Sequence::mean() const {

	// if the sequence is empty, return 0 as instructed
	if (size() == 0) {
		return 0;
	}

	double mean = sum(); // get sum of all elements
	mean /= size(); // get average by dividing number of elements in arr
	return mean; // return a double

}

// return a double equal to median of values in calling object
double Sequence::median() const {
	
	// if the sequence is empty, return 0 as instructed
	if (size() == 0) {
		return 0;
	}

	int n = this->size(); // get number of elements in calling object's array
	sort(arr, arr + n); // sort elements in calling object's array in ascending order

	// if there is an odd number of elements in calling object's array
	if (n % 2 == 1) {
		return arr[n / 2]; // return middle element's value
	}

	// if there is an even number of elements in calling object's array
	else {
		return ((arr[(n - 1) / 2] + arr[n / 2]) / 2); // return average of the two most "inner" elements in calling object's array
	}

}

// return a double equal to standard deviation of values in calling object
double Sequence::stddev() const {

	double mean = this->mean(); // get mean of elements
	double standardDev = 0; // counter: standard deviation
	double summation = 0; // counter: summation in standard deviation calculation
	int n = this->size(); // number of elements

	// if the sequence is empty, return 0 as instructed
	if (n == 0) {
		return 0;
	}

	// calculating summation 
	for (int i = 0; i < n; ++i) {
		summation += pow((arr[i] - mean), 2);
	}

	// calculating standard deviation
	standardDev = sqrt(summation / n);

	return standardDev; // return standard deviation value

}

// returns a concatenated Sequence object
Sequence Sequence::concatenate(const Sequence& mySeq) {
	
	Sequence newSeq; // calling constructor to create newSeq
	int totalCurrSize = this->currSize + mySeq.currSize; // adding the currSize of calling object array and parameter array
	int result = 2; // initalizing result: result is size of newArr
	
	// if calling object's arr and parameter's arr combined size is greater than default 
	if (totalCurrSize > 2) {
		result = totalCurrSize; // set result to equal to combined size
	}

	double* newArr = new double[result]; // creating an array of new max size: result
	
	// copy calling object's elements into newArr
	for (int i = 0; i < this->currSize; ++i) {
		newArr[i] = this->arr[i];
	}

	// copy called object's elements into newArr
	for (int i = 0; i < mySeq.currSize; ++i) {
		newArr[i + this->currSize] = mySeq.arr[i];
	}

	delete[] newSeq.arr; // freeing memory associated with newSeq's arr
	newSeq.currSize = totalCurrSize; // set newSeq's currSize to totalCurrSize
	newSeq.maxSize = result; // set newSeq's maxSize to result
	newSeq.arr = newArr; // set newSeq's array to newArr

	return newSeq; // return a Sequence object

}

// prints Sequence
void Sequence::print() {

	for (int i = 0; i < maxSize; ++i) {
		cout << arr[i] << endl;
	}

}
