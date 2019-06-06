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
	Multiply mult(x1, x2);
	Add sum(x2, x3);
	NaturalLog ln(mult);
	Exponentiate ex(sum);
	RaiseToPower po(ex, ln);
	Sine out(po);
			
	try {
		vector<Input*> inputs = {&x1,&x2,&x3};
		
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