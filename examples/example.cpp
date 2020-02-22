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
	
	//create input nodes
	Node x1;
	Node x2;
	Node x3;
	
	//create downstream nodes via computations
	Node n1 = (4 + 2*x1 + 3*x2 - 5*x3)*2;
	Node output = n1 * n1 * n1;
			
	try {
		vector<Node*> inputs = {&x1,&x2,&x3};
		Function func(inputs);

		cout << "func node count: " << func.nodeCount() << "\n";

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
};