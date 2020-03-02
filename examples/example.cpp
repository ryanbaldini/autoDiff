#include "autoDiff.h"
#include <iostream>

using namespace std;

void printVector(vector<arma::mat>& vec) {
	int vecSize = vec.size();
	for(int i=0; i<vecSize; i++) {
		cout << vec[i] << ",";
	}
	cout << "\n";
}

int main() {
	//a good one
	try {
		cout << "Example 1\n";
		//create input nodes
		ad::Node x1;
		ad::Node x2;
		ad::Node x3;

		//create downstream nodes via computations
		ad::Node n1 = x1 + x2 + x3;
		ad::Node output = sumElements(n1);
		// ad::Node n2 = exp(x1/x2);
		// n2 = n1 * n2; //reassigning is ok. the original n2 node is copied to heap.
		// ad::Node outputNode = ad::log(n1 * n1 * n2 * n2);

		//put in function
		vector<ad::Node*> inputNodes = {&x1,&x2,&x3};
		ad::Function func(inputNodes);
		
		//evaluate, differentiate
		arma::mat input1val(2, 2, arma::fill::ones);
		arma::mat input2val(2, 2, arma::fill::ones);
		arma::mat input3val(2, 2, arma::fill::ones);
		vector<arma::mat> input{input1val, input2val, input3val};
		arma::mat outputVals = func.evaluate(input);
		cout << "output:\n" << outputVals << "\n";
		vector<arma::mat> gradient = func.differentiate(input);
		cout << "gradient: ";
		printVector(gradient);
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
 	}
	
	// //invalid graph: input node becomes a calculated node
// 	try {
// 		cout << "\nExample 2\n";
// 		ad::Node x1;
// 		ad::Node x2;
//
// 		ad::Node n1 = x1 * x1;
// 		ad::Node n2 = exp(x2);
// 		x1 = n2*2; //bad!
//
// 		vector<ad::Node*> inputNodes = {&x1,&x2};
// 		ad::Function func(inputNodes);
//
// 		vector<double> input{1,2};
// 		double output = func.evaluate(input);
// 		vector<double> gradient = func.differentiate(input);
// 		cout << "output: " << output << "\n";
// 		cout << "gradient: ";
// 		printVector(gradient);
// 	}
// 	catch(const char* err) {
// 		cout << "Error: " << err << "\n";
//  	}
//
// 	//invalid graph: multiple terminal nodes
// 	try {
// 		cout << "\nExample 3\n";
// 		ad::Node x1;
// 		ad::Node x2;
//
// 		ad::Node n1 = x1 * x2;
// 		ad::Node n2 = exp(x2);
// 		//two terminal nodes: n1 and n2
//
// 		vector<ad::Node*> inputNodes = {&x1,&x2};
// 		ad::Function func(inputNodes);
//
// 		vector<double> input{1,2};
// 		double output = func.evaluate(input);
// 		vector<double> gradient = func.differentiate(input);
// 		cout << "output: " << output << "\n";
// 		cout << "gradient: ";
// 		printVector(gradient);
// 	}
// 	catch(const char* err) {
// 		cout << "Error: " << err << "\n";
//  	}
//
// 	//invalid graph: not all input nodes are provided to the function
// 	try {
// 		cout << "\nExample 4\n";
// 		ad::Node x1;
// 		ad::Node x2;
// 		ad::Node x3;
//
// 		ad::Node n1 = x1 * x2 * x3;
// 		ad::Node n2 = exp(x1 + x2 + x3);
// 		ad::Node nOut = n1*n2;
// 		n1 = nOut*nOut;
//
// 		vector<ad::Node*> inputNodes = {&x1,&x2,&x2}; //oops! passed in x2 but not x3!
// 		ad::Function func(inputNodes);
//
// 		vector<double> input{1,2,3};
// 		double output = func.evaluate(input);
// 		vector<double> gradient = func.differentiate(input);
// 		cout << "output: " << output << "\n";
// 		cout << "gradient: ";
// 		printVector(gradient);
// 	}
// 	catch(const char* err) {
// 		cout << "Error: " << err << "\n";
//}
};