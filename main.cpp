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
	Input input1;
	Input input2;
	Input input3;
	
	MultiplyInputs mult1(&input1, &input2);
	MultiplyInputs mult2(&input1, &input3);
	MultiplyInputs mult3(&mult1, &mult2);
	MultiplyInputs mult4(&mult2, &mult3);
	SubtractInputs diff(&mult1, &mult3);
	AddConstant sum(&diff, 10);
	DivideInputs mult6(&mult4, &sum);
	
	Function func;
	
	try {
		vector<Input*> inputs;
		inputs.push_back(&input1);
		inputs.push_back(&input2);
		inputs.push_back(&input3);
		
		func = Function(inputs);
		
		double output = func.evaluate(vector<double>{1,2,3});
		cout << "output: " << output << "\n";
		
		vector<double> derivatives = func.differentiate(vector<double>{1,2,3});
		printVector(derivatives);
	}
	catch(const char* err) {
		cout << "Error: " << err << "\n";
	}
}