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
	Input x1;
	Input x2;
	Input x3;
	
	//create downstream nodes via computations
	MultiplyNodes mult1(&x1, &x2);
	MultiplyNodes mult2(&x1, &x3);
	MultiplyNodes mult3(&mult1, &mult2);
	MultiplyNodes mult4(&mult2, &mult3);
	SubtractNodes diff(&mult1, &mult3);
	AddConstant sum(&diff, 10);
	DivideNodes mult6(&mult4, &sum);
	
	//this computational graph computes the following expression
	//(x1*x3)*(x1*x2)*(x1*x3) / (((x1*x2)-(x1*x2)*(x1*x3))+10)
	
	try {
		vector<Input*> inputs;
		inputs.push_back(&x1);
		inputs.push_back(&x2);
		inputs.push_back(&x3);
		
		Function func(inputs);
		
		//evaluate at x1=1, x2=2, x3=3
		double output = func.evaluate(vector<double>{1,2,3});
		cout << "output: " << output << "\n";
		
		//differentiate with respect to (x1,x2,x3) at x1=1, x2=2, x3=3
		vector<double> gradient = func.differentiate(vector<double>{1,2,3});
		cout << "gradient: ";
		printVector(gradient);
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
	}
}