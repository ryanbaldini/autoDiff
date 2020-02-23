#include "autoDiff.h"
#include <iostream>

using namespace std;

void printVector(vector<double>& vec) {
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
		ad::Node outputNode = ad::log(n1 * n1 * n2 * n2);

		//put in function
		vector<ad::Node*> inputNodes = {&x1,&x2,&x3};
		ad::Function func(inputNodes);
		
		//evaluate, differentiate
		vector<double> input{1,2,3};
		double output = func.evaluate(input);
		vector<double> gradient = func.differentiate(input);
		cout << "output: " << output << "\n";
		cout << "gradient: ";
		printVector(gradient);
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
 	}
	
	//invalid circular graph
	try {
		cout << "\nExample 2\n";
		ad::Node x1;
		ad::Node x2;
		
		ad::Node n1 = x1 * x2;
		x1 = 2*n1;
		
		vector<ad::Node*> inputNodes = {&x1,&x2};
		ad::Function func(inputNodes);
		
		vector<double> input{1,2};
		double output = func.evaluate(input);
		vector<double> gradient = func.differentiate(input);
		cout << "output: " << output << "\n";
		cout << "gradient: ";
		printVector(gradient);
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
 	}
};