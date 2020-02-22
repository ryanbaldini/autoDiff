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
	// Node x3;
	
	//create downstream nodes via computations
	
	Node output = x1 + x1 + x2 + x2 + x2;
	cout << "got here\n";
	cout << x1.children[0] << "\n";
	cout << x2.children[0] << "\n";
	cout << x2.children[0]->operation << "\n";
	cout << &output << "\n";
	cout << output.operation << "\n";
	cout << output.parents.size() << "\n";
	cout << output.parents[0] << "\n";
	cout << output.parents[0]->parents.size() << "\n";
	cout << output.parents[0]->parents[0] << "\n";
	cout << output.parents[0]->parents[1] << "\n";
	// Multiply mult(x1, x2);
	// Add sum = x1 + x2 + x3;
	// NaturalLog ln(mult);
	// Exponentiate ex(sum);
	// RaiseToPower po(ex, ln);
	// Sine out(po);
			
	try {
		vector<Node*> inputs = {&x1,&x2};
		Function func(inputs);

		cout << "func node count: " << func.nodeCount() << "\n";

		double output = func.evaluate(vector<double>{5,2});
		cout << "output: " << output << "\n";
		output = func.evaluate(vector<double>{9,13});
		cout << "output: " << output << "\n";
		output = func.evaluate(vector<double>{0,1});
		cout << "output: " << output << "\n";

		vector<double> gradient = func.differentiate(vector<double>{5,2});
		cout << "gradient: ";
		printVector(gradient);
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
 	}
};