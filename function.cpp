#include "autoDiff.h"

using namespace std;

Function::Function(): outputNode(nullptr) {
}

Function::Function(vector<Input*> inputNodes_): inputNodes(inputNodes_), outputNode(nullptr) {
	int nInputs = inputNodes.size();
	if(nInputs == 0) {
		throw "No inputs to function";
	}
	
	//check for singular terminal Nodes
	for(int i=0; i<nInputs; i++) {
		vector<Node*> terminalOps = inputNodes[i]->findTerminalNodes();
		int nTerminalOps = terminalOps.size();
		if(nTerminalOps == 0) {
			throw "At least one input lacks a terminal node. Circularity?";
		} else if(nTerminalOps > 1) {
			throw "More than one terminal node. There must be only one.";
		} else {
			if(outputNode == nullptr) {
				outputNode = terminalOps[0];
			} else {
				if(outputNode != terminalOps[0]) {
					throw "More than one terminal node. There must be only one.";
				}
			}
		}
	}
	if(outputNode == nullptr) {
		throw "No terminal node. Circular graph?";
	}
	
	//should probably also check for circularity directly, by seeing if any nodes have themselves as a parent?
	//should be impossible if used correctly?
}

double Function::evaluate(vector<double> args) {
	
	//set values of input nodes based on arg
	int nArgs = args.size();
	int nInputs = inputNodes.size();
	if(nArgs != nInputs) {
		throw "Number of args does not equal required number of inputs";
	}
	for(int i=0; i<nInputs; i++) {
		inputNodes[i]->setMySubNetworkEvaluatedFalse();
		inputNodes[i]->value = args[i];
	}
	
	for(int i=0; i<nInputs; i++) {
		inputNodes[i]->evaluate();	//recursive evaluation starts here
	}

	return outputNode->value;
}

vector<double> Function::differentiate(vector<double> args) {
	
	evaluate(args);
	
	int nInputs = inputNodes.size();
	for(int i=0; i<nInputs; i++) {
		inputNodes[i]->setMySubNetworkDifferentiatedFalse();
	}
	
	outputNode->derivative = 1.0;
	outputNode->differentiate(); //recursive differentiation starts here
	
	vector<double> derivatives(nInputs);
	for(int i=0; i<nInputs; i++) {
		derivatives[i] = inputNodes[i]->derivative;
 	}
	
	return derivatives;
}