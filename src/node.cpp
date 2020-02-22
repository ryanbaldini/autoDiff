#include "autoDiff.h"
#include <algorithm>
#include <cmath>

using namespace std;

Node::Node(): operation(nullptr), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
}

Node::Node(Node& parent): operation(new Inherit), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
	setParent(parent);
}

Node::Node(Node& parent1, Node& parent2): operation(nullptr), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
	setParent(parent1);
	setParent(parent2);
}

Node::Node(vector<Node*>& parents): operation(nullptr), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
	int nParents = parents.size();
	if(nParents <= 1) {
		throw "MultiplyNodes called on less than 2 nodes. Need at least 2.";
	}
	for(int i=0; i<nParents; i++) {
		setParent(*parents[i]);
	}
}


double Node::getValue() {
	return value;
}

double Node::getDerivative() {
	return derivative;
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

void Node::fillMyValue() {
	if(operation == nullptr) {
		return;
	}
	int nParents = parents.size();
	vector<double> inputValues(nParents);
	for(int i=0; i<nParents; i++) {
		inputValues[i] = parents[i]->value;
	}
	value = operation->evaluate(inputValues);
}

void Node::updateParentDerivatives() {
	if(operation == nullptr) {
		return;
	}
	int nParents = parents.size();
	vector<double> inputValues(nParents);
	for(int i=0; i<nParents; i++) {
		inputValues[i] = parents[i]->value;
	}
	vector<double> derivatives = operation->differentiate(inputValues);
	for(int i=0; i<nParents; i++) {
		parents[i]->derivative += derivatives[i] * derivative;
	}
}

vector<Node*> Node::getDescendantNodes() {
	vector<Node*> descendantNodes;
	int nChildren = children.size();
	if(nChildren == 0) {
		return descendantNodes;
	}
	for(int i=0; i<nChildren; i++) {
		Node* child = children[i];
		descendantNodes.push_back(child);
		vector<Node*> childDescendants = child->getDescendantNodes(); //recursive
		int nChildDescendants = childDescendants.size();
		//put in descendantNodes if not in there yet
		for(int j=0; j<nChildDescendants; j++) {
			if(find(descendantNodes.begin(), descendantNodes.end(), childDescendants[j]) == descendantNodes.end()) {
				descendantNodes.push_back(childDescendants[j]);
			}
		}
	}
	return descendantNodes;
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

void Node::setParent(Node& node) {
	parents.push_back(&node);
	node.children.push_back(this);
}

// Node operator+(Node& parent1, Node& parent2) {
// 	Node node(parent1, parent2);
// 	node.operation = new Add(0.0);
// 	return node;
// };

Node& add(Node& parent1, Node& parent2) {
	Node* node = new Node(parent1, parent2);
	node->operation = new Add(0.0);
	return *node;
}

Node& operator+(Node& parent1, Node& parent2) {
	return add(parent1, parent2);
};

// void Input::fillMyValue() {
// 	return;
// }
//
// void Input::updateParentDerivatives() {
// 	return;
// }

// Add::Add(Node& parent, double constant_): Node(parent), constant(constant_) {
// }
//
// Add::Add(Node& parent1, Node& parent2): Node(parent1, parent2) {
// }
//
// Add::Add(vector<Node*>& parents): Node(parents) {
// }
//
// void Add::fillMyValue() {
// 	int nParents = parents.size();
// 	if(nParents == 1) { //then add constant
// 		value = (parents[0]->value) + constant;
// 	} else { //then add parents
// 		value = 0.0;
// 		for(int i=0; i<nParents; i++) {
// 			value += parents[i]->value;
// 		}
// 	}
// }
//
// void Add::updateParentDerivatives() {
// 	int nParents = parents.size();
// 	for(int i=0; i<nParents; i++) {
// 		parents[i]->derivative += derivative;
// 	}
// }
//
//
// Subtract::Subtract(Node& parent1, Node& parent2): Node(parent1, parent2) {
// }
//
// Subtract::Subtract(Node& parent, double constant_): Node(parent), constant(constant_), parentFirst(true) {
// }
//
// Subtract::Subtract(double constant_, Node& parent): Node(parent), constant(constant_), parentFirst(false) {
// }
//
// void Subtract::fillMyValue() {
// 	int nParents = parents.size();
// 	if(nParents == 1) {
// 		if(parentFirst) {
// 			value = (parents[0]->value) - constant;
// 		} else {
// 			value = constant - parents[0]->value;
// 		}
// 	} else {
// 		value = (parents[0]->value) - (parents[1]->value);
// 	}
// }
//
// void Subtract::updateParentDerivatives() {
// 	int nParents = parents.size();
// 	if(nParents == 1) {
// 		if(parentFirst) {
// 			parents[0]->derivative += derivative;
// 		} else {
// 			parents[0]->derivative -= derivative;
// 		}
// 	} else {
// 		parents[0]->derivative += derivative;
// 		parents[1]->derivative -= derivative;
// 	}
// }
//
//
// Multiply::Multiply(Node& parent, double constant_): Node(parent), constant(constant_) {
// }
//
// Multiply::Multiply(Node& parent1, Node& parent2): Node(parent1, parent2) {
// }
//
// Multiply::Multiply(vector<Node*>& parents): Node(parents) {
// }
//
// void Multiply::fillMyValue() {
// 	int nParents = parents.size();
// 	if(nParents == 1) {
// 		value = parents[0]->value * constant;
// 	} else {
// 	 	value = 1.0;
// 		for(int i=0; i<nParents; i++) {
// 			value *= parents[i]->value;
// 		}
// 	}
// }
//
// void Multiply::updateParentDerivatives() {
// 	int nParents = parents.size();
// 	if(nParents == 1) {
// 		parents[0]->derivative += derivative * constant;
// 	} else {
// 		for(int i=0; i<nParents; i++) {
// 			double prod = 1.0;
// 			for(int j=0; j<nParents; j++) {
// 				if(j != i) {
// 					prod *= parents[j]->value;
// 				}
// 			}
// 			parents[i]->derivative += derivative * prod;
// 		}
// 	}
// }
//
//
// Divide::Divide(Node& parent1, Node& parent2): Node(parent1, parent2) {
// }
//
// Divide::Divide(Node& parent, double constant_): Node(parent), constant(constant_), parentFirst(true) {
// 	if(constant_ == 0) {
// 		throw "Tried to construct Divide node with constant denominator of 0";
// 	}
// }
//
// Divide::Divide(double constant_, Node& parent): Node(parent), constant(constant_), parentFirst(false) {
// }
//
// void Divide::fillMyValue() {
// 	int nParents = parents.size();
// 	if(nParents == 1) {
// 		if(parentFirst) {
// 			value = (parents[0]->value) / constant; //already checked 0 on construction
// 		} else {
// 			if(parents[0]->value == 0.0) {
// 				throw "Divide node attempted to divide by 0.0";
// 			}
// 			value = constant / (parents[0]->value);
// 		}
// 	} else {
// 		if(parents[1]->value == 0.0) {
// 			throw "Divide node attempted to divide by 0.0";
// 		}
// 		value = (parents[0]->value) / (parents[1]->value);
// 	}
// }
//
//
// void Divide::updateParentDerivatives() {
// 	int nParents = parents.size();
// 	if(nParents == 1) {
// 		Node* parent = parents[0];
// 		if(parentFirst) {
// 			parent->derivative += derivative / constant;
// 		} else {
// 			if(parent->value == 0.0) {
// 				throw "Divide node attempted to divide by 0.0 in gradient calculation";
// 			}
// 			double pSquared = (parent->value) * (parent->value);
// 			parent->derivative -= derivative * constant / pSquared;
// 		}
// 	} else {
// 		if(parents[1]->value == 0.0) {
// 			throw "DivideNodes node attempted to divide by 0.0 in gradient calculation";
// 		}
// 		parents[0]->derivative += derivative / (parents[1]->value);
// 		double p1Squared = (parents[1]->value) * (parents[1]->value);
// 		parents[1]->derivative -= derivative * (parents[0]->value) / p1Squared;
// 	}
// }
//
// NaturalLog::NaturalLog(Node& parent): Node(parent) {
// }
//
// void NaturalLog::fillMyValue() {
// 	Node* parent = parents[0];
// 	if(parent->value <= 0) {
// 		throw "NaturalLog node tried to take log of non-positive number";
// 	}
// 	value = log(parent->value);
// }
//
// void NaturalLog::updateParentDerivatives() {
// 	Node* parent = parents[0];
// 	if(parent->value == 0) {
// 		throw "NaturalLog node tried to divide by 0.0 in gradient calculation";
// 	}
// 	parent->derivative += derivative / parent->value;
// }
//
// Exponentiate::Exponentiate(Node& parent): Node(parent) {
// }
//
// void Exponentiate::fillMyValue() {
// 	value = exp(parents[0]->value);
// }
//
// void Exponentiate::updateParentDerivatives() {
// 	parents[0]->derivative += derivative * exp(parents[0]->value);
// }
//
// Square::Square(Node& parent): Node(parent) {
// }
//
// void Square::fillMyValue() {
// 	double parentValue = parents[0]->value;
// 	value = parentValue * parentValue;
// }
//
// void Square::updateParentDerivatives() {
// 	parents[0]->derivative += derivative * 2*parents[0]->value;
// }
//
// SquareRoot::SquareRoot(Node& parent): Node(parent) {
// }
//
// void SquareRoot::fillMyValue() {
// 	double parentValue = parents[0]->value;
// 	if(parentValue < 0) {
// 		throw "SquareRoot tried to take square root of negative number";
// 	}
// 	value = sqrt(parentValue);
// }
//
// void SquareRoot::updateParentDerivatives() {
// 	parents[0]->derivative += derivative * 0.5 * pow(parents[0]->value, -0.5);
// }
//
// Cube::Cube(Node& parent): Node(parent) {
// }
//
// void Cube::fillMyValue() {
// 	double parentValue = parents[0]->value;
// 	value = parentValue * parentValue * parentValue;
// }
//
// void Cube::updateParentDerivatives() {
// 	double parentValue = parents[0]->value;
// 	parents[0]->derivative += derivative * 3 * parentValue * parentValue;
// }
//
//
// CubeRoot::CubeRoot(Node& parent): Node(parent) {
// }
//
// void CubeRoot::fillMyValue() {
// 	double parentValue = parents[0]->value;
// 	value = pow(parentValue, 1.0/3.0);
// }
//
// void CubeRoot::updateParentDerivatives() {
// 	parents[0]->derivative += derivative * 1.0/3.0 * pow(parents[0]->value, -2.0/3.0);
// }
//
//
// RaiseToPower::RaiseToPower(double base, Node& parent): Node(parent), parentIsBase(false), constant(base) {
// 	if(base <= 0) {
// 		throw "Can't make constant base <=0 in RaiseToPower node: computing derivative involves logging the base";
// 	}
// }
//
// RaiseToPower::RaiseToPower(Node& parent, double exponent): Node(parent), parentIsBase(true), constant(exponent) {
// }
//
// RaiseToPower::RaiseToPower(Node& parent1, Node& parent2): Node(parent1, parent2) {
// }
//
// void RaiseToPower::fillMyValue() {
// 	int nParents = parents.size();
// 	if(nParents == 1) {
// 		if(parentIsBase) {
// 			value = pow(parents[0]->value, constant);
// 		} else {
// 			value = pow(constant, parents[0]->value);
// 		}
// 	} else {
// 		value = pow(parents[0]->value, parents[1]->value);
// 	}
// }
//
// void RaiseToPower::updateParentDerivatives() {
// 	int nParents = parents.size();
// 	if(nParents == 1) {
// 		if(parentIsBase) {
// 			parents[0]->derivative += derivative * constant * pow(parents[0]->value, constant-1);
// 		} else {
// 			//<= 0 already checked for base
// 			parents[0]->derivative += derivative * log(constant) * pow(constant, parents[0]->value);
// 		}
// 	} else {
// 		if(parents[0]->value <= 0) {
// 			throw "Failed differentiatio of RaiseToPower: can't take log of non-positive number";
// 		}
// 		parents[0]->derivative += derivative * parents[1]->value * pow(parents[0]->value, parents[1]->value-1);
// 		parents[1]->derivative += derivative * log(parents[0]->value) * pow(parents[0]->value, parents[1]->value);
// 	}
// }
//
//
// Sine::Sine(Node& parent): Node(parent) {
// }
//
// void Sine::fillMyValue() {
// 	value = sin(parents[0]->value);
// }
//
// void Sine::updateParentDerivatives() {
// 	parents[0]->derivative += derivative * cos(parents[0]->value);
// }
//
//
// Cosine::Cosine(Node& parent): Node(parent) {
// }
//
// void Cosine::fillMyValue() {
// 	value = cos(parents[0]->value);
// }
//
// void Cosine::updateParentDerivatives() {
// 	parents[0]->derivative += derivative * -sin(parents[0]->value);
// }