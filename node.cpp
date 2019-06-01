#include "autoDiff.h"
#include <algorithm>

using namespace std;

Node::Node(): value(0), derivative(0), evaluated(false), differentiatedParents(false) {
}

void Node::evaluate() {
	
	if(evaluated) {
		return;
	}
	
	//don't go if not all parents evaluated
	int nParents = parents.size();
	for(int i=0; i<nParents; i++) {
		if(!parents[i]->evaluated) {
			return;
		}
	}
	
	fillMyValue();
	evaluated = true;
	
	int nChildren = children.size();
	for(int i=0; i<nChildren; i++) {
		children[i]->evaluate();
	}
	
	return;
}

void Node::differentiate() {
	
	if(differentiatedParents) {
		return;
	}
	
	//don't go if not all children differentiated their parents
	int nChildren = children.size();
	for(int i=0; i<nChildren; i++) {
		if(!children[i]->differentiatedParents) {
			return;
		}
	}
	
	updateParentDerivatives();
	differentiatedParents = true;
	
	int nParents = parents.size();
	for(int i=0; i<nParents; i++) {
		parents[i]->differentiate();
	}
	
	return;
}

//base class virtual function; unused
void Node::fillMyValue() {
	return;
}

//base class virtual function; unused
void Node::updateParentDerivatives() {
	return;
}

vector<Node*> Node::findTerminalNodes() {
	vector<Node*> terminalOps;
	int nChildren = children.size();
	if(nChildren == 0) {
		terminalOps.push_back(this);
		return terminalOps;
	} else {
		for(int i=0; i<nChildren; i++) {
			vector<Node*> childTerminalOps = children[i]->findTerminalNodes(); //recursive
			int nChildTerminalOps = childTerminalOps.size();
			//put in terminalOps if not in there yet
			for(int j=0; j<nChildTerminalOps; j++) {
				if(find(terminalOps.begin(), terminalOps.end(), childTerminalOps[j]) == terminalOps.end()) {
					terminalOps.push_back(childTerminalOps[j]);
				}
			}
		}
	}
	return terminalOps;
}

void Node::setMySubNetworkEvaluatedFalse() {
	evaluated = false;
	int nChildren = children.size();
	for(int i=0; i<nChildren; i++) {
		children[i]->setMySubNetworkEvaluatedFalse();
	}
	return;
}

void Node::setMySubNetworkDifferentiatedFalse() {
	differentiatedParents = false;
	int nChildren = children.size();
	for(int i=0; i<nChildren; i++) {
		children[i]->derivative = 0;
		children[i]->setMySubNetworkDifferentiatedFalse();
	}
	return;
}

void Node::setParent(Node* node) {
	parents.push_back(node);
	node->children.push_back(this);
}

void Input::fillMyValue() {
	return;
}

void Input::updateParentDerivatives() {
	return;
}

AddConstant::AddConstant(Node* node, double constant_): constant(constant_) {
	setParent(node);
}

void AddConstant::fillMyValue() {
	value = (parents[0]->value) + constant;
}

void AddConstant::updateParentDerivatives() {
	parents[0]->derivative += derivative;
}

AddNodes::AddNodes(Node* node1, Node* node2) {
	setParent(node1);
	setParent(node2);
}

void AddNodes::fillMyValue() {
	value = (parents[0]->value) + (parents[1]->value);
}

void AddNodes::updateParentDerivatives() {
	parents[0]->derivative += derivative;
	parents[1]->derivative += derivative;
}

SubtractNodes::SubtractNodes(Node* node1, Node* node2) {
	setParent(node1);
	setParent(node2);
}

void SubtractNodes::fillMyValue() {
	value = (parents[0]->value) - (parents[1]->value);
}

void SubtractNodes::updateParentDerivatives() {
	parents[0]->derivative += derivative;
	parents[1]->derivative -= derivative;
}

MultiplyNodes::MultiplyNodes(Node* node1, Node* node2) {
	setParent(node1);
	setParent(node2);
}

void MultiplyNodes::fillMyValue() {
	value = (parents[0]->value) * (parents[1]->value);
	evaluated = true;
}

void MultiplyNodes::updateParentDerivatives() {
	parents[0]->derivative += derivative * parents[1]->value;
	parents[1]->derivative += derivative * parents[0]->value;
}

DivideNodes::DivideNodes(Node* node1, Node* node2) {
	setParent(node1);
	setParent(node2);
}

void DivideNodes::fillMyValue() {
	if(parents[1]->value == 0.0) {
		throw "DivideNodes node attempted to divide by 0.0";
	}
	value = (parents[0]->value) / (parents[1]->value);
}

void DivideNodes::updateParentDerivatives() {
	parents[0]->derivative += derivative / (parents[1]->value);
	double p1Squared = (parents[1]->value) * (parents[1]->value);
	parents[1]->derivative -= derivative * (parents[0]->value) / p1Squared;
}
