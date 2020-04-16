#include "autoDiff.h"
#include <iostream>

using namespace std;

void printVector(const vector<double>& vec) {
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
		ad::Node n1 = (4 + 2*x1 + 3*x2 - 5*x3)/(x1+x3);
		ad::Node n2 = exp(x1/x2);
		n2 += n1 * n2; //reassigning is ok. the original n2 node is copied to heap and linked appropriately.
		ad::Node outputNode = log(n1 * n1 * n2 * n2);
		outputNode /= 2;

		//create function with input nodes
		ad::Function func({&x1,&x2,&x3});
		
		//evaluate, differentiate
		cout << "output: " << func.evaluate({12,3,7}) << "\n";
		cout << "gradient: ";
		printVector(func.differentiate({18,1,6}));
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
 	}
	
	//invalid graph: input node becomes a calculated node
	try {
		cout << "\nExample 2\n";
		ad::Node x1;
		ad::Node x2;
		
		ad::Node n1 = x1 * x1;
		ad::Node n2 = exp(x2);
		x1 = n2*2; //bad!
		
		ad::Function func({&x1,&x2});
		
		cout << "output: " << func.evaluate({1,2}) << "\n";
		cout << "gradient: ";
		printVector(func.differentiate({1,2}));
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
 	}
	
	//invalid graph: multiple terminal nodes
	try {
		cout << "\nExample 3\n";
		ad::Node x1;
		ad::Node x2;
		
		ad::Node n1 = x1 * x2;
		ad::Node n2 = exp(x2);
		//two terminal nodes: n1 and n2
		
		ad::Function func({&x1,&x2});
		
		cout << "output: " << func.evaluate({1,2}) << "\n";
		cout << "gradient: ";
		printVector(func.differentiate({1,2}));
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
 	}
	
	//invalid graph: not all origin nodes are provided to the function as input
	try {
		cout << "\nExample 4\n";
		ad::Node x1;
		ad::Node x2;
		ad::Node x3;
		
		ad::Node n1 = x1 * x2 * x3;
		ad::Node n2 = exp(x1 + x2 + x3);
		ad::Node nOut = n1*n2;
		n1 = nOut*nOut;
		
		ad::Function func({&x1,&x2,&x2}); //oops! passed in x2 twice, but not x3, which is an origin node to the graph
		
		cout << "output: " << func.evaluate({1,2,3}) << "\n";
		cout << "gradient: ";
		printVector(func.differentiate({1,2,3}));
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
 	}
};