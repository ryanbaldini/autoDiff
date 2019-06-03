#include "autoDiff.h"
#include <algorithm> 

using namespace std;

Function::Function(vector<Input*> inputNodes_): inputNodes(inputNodes_), outputNode(nullptr) {
	int nInputs = inputNodes.size();
	if(nInputs == 0) {
		throw "No inputs to function";
	}
	
	//check that we only have one terminal Node
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
	
	//collect all nodes into vector
	for(int i=0; i<nInputs; i++) {
		nodes.push_back(inputNodes[i]);
		vector<Node*> descendantNodes = inputNodes[i]->getDescendantNodes();
		int nDescendantNodes = descendantNodes.size();
		for(int j=0; j<nDescendantNodes; j++) {
			if(find(nodes.begin(), nodes.end(), descendantNodes[j]) == nodes.end()) {
				nodes.push_back(descendantNodes[j]);
			}
		}
	}
	
	//should probably also check for circularity directly, by seeing if any nodes have themselves as a parent?
	//should be impossible if used correctly?
}

double Function::evaluate(vector<double> args) {
	
	int nArgs = args.size();
	int nInputs = inputNodes.size();
	if(nArgs != nInputs) {
		throw "Number of args does not equal required number of inputs";
	}
	
	//set all nodes to unevaluated
	int nNodes = nodes.size();
	for(int i=0; i<nNodes; i++) {
		nodes[i]->evaluated = false;
	}
	
	for(int i=0; i<nInputs; i++) {
		inputNodes[i]->value = args[i];
		inputNodes[i]->evaluate();	//recursive evaluation starts here
	}

	return outputNode->value;
}

vector<double> Function::differentiate(vector<double> args) {
	
	evaluate(args);
	
	//set all nodes to undifferentiated
	int nNodes = nodes.size();
	for(int i=0; i<nNodes; i++) {
		nodes[i]->differentiatedParents = false;
		nodes[i]->derivative = 0.0;
	}
	
	outputNode->derivative = 1.0; //derivative of output with respect to itself is 1
	outputNode->differentiate(); //recursive differentiation starts here
	
	int nInputs = inputNodes.size();
	vector<double> derivatives(nInputs);
	for(int i=0; i<nInputs; i++) {
		derivatives[i] = inputNodes[i]->derivative;
 	}
	
	return derivatives;
}