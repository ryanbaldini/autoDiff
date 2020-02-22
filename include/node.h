#pragma once

#include <algorithm>
#include <cmath>

namespace ad {
	struct Node {
		Operation* operation;
		double value;
		double derivative;
		std::vector<Node*> parents;
		std::vector<Node*> children;
		bool evaluated;
		bool differentiatedParents;
	
		void evaluate();
		void differentiate();
		void fillMyValue();
		void updateParentDerivatives();
		std::vector<Node*> getDescendantNodes();
		std::vector<Node*> findTerminalNodes();
		void setParent(Node& node);
	
		Node();
		Node(Node& parent); //copy constructor
		Node(Node& parent, Operation* operation);
		Node(Node& parent1, Node& parent2);
		Node(Node& parent1, Node& parent2, Operation* operation);
		Node(std::vector<Node*>& parents);
		Node(std::vector<Node*>& parents, Operation* operation);
	
		double getValue();
		double getDerivative();
	};

	Node::Node(): operation(nullptr), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
	}

	//this is the copy constructor. it will create a new node that simply inherits the value of the previous node.
	Node::Node(Node& parent): operation(new Inherit), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
		setParent(parent);
	}

	Node::Node(Node& parent, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
		setParent(parent);
	}


	Node::Node(Node& parent1, Node& parent2): operation(nullptr), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
		setParent(parent1);
		setParent(parent2);
	}

	Node::Node(Node& parent1, Node& parent2, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
		setParent(parent1);
		setParent(parent2);
	}

	Node::Node(std::vector<Node*>& parents): operation(nullptr), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
		int nParents = parents.size();
		for(int i=0; i<nParents; i++) {
			setParent(*parents[i]);
		}
	}

	Node::Node(std::vector<Node*>& parents, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false) {
		int nParents = parents.size();
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
		std::vector<double> inputValues(nParents);
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
		std::vector<double> inputValues(nParents);
		for(int i=0; i<nParents; i++) {
			inputValues[i] = parents[i]->value;
		}
		std::vector<double> derivatives = operation->differentiate(inputValues);
		for(int i=0; i<nParents; i++) {
			parents[i]->derivative += derivatives[i] * derivative;
		}
	}

	std::vector<Node*> Node::getDescendantNodes() {
		std::vector<Node*> descendantNodes;
		int nChildren = children.size();
		if(nChildren == 0) {
			return descendantNodes;
		}
		for(int i=0; i<nChildren; i++) {
			Node* child = children[i];
			descendantNodes.push_back(child);
			std::vector<Node*> childDescendants = child->getDescendantNodes(); //recursive
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

	std::vector<Node*> Node::findTerminalNodes() {
		std::vector<Node*> terminalOps;
		int nChildren = children.size();
		if(nChildren == 0) {
			terminalOps.push_back(this);
			return terminalOps;
		} else {
			for(int i=0; i<nChildren; i++) {
				std::vector<Node*> childTerminalOps = children[i]->findTerminalNodes(); //recursive
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

	Node& add(Node& parent1, Node& parent2) {
		Operation* addOp = new Add(0.0);
		Node* node = new Node(parent1, parent2, addOp);
		return *node;
	}

	Node& add(Node& parent, double x) {
		Operation* addOp = new Add(x);
		Node* node = new Node(parent, addOp);
		return *node;
	}

	Node& add(double x, Node& parent) {
		return add(parent, x);
	}

	Node& operator+(Node& parent1, Node& parent2) {
		return add(parent1, parent2);
	};

	Node& operator+(Node& parent, double x) {
		return add(parent, x);
	};

	Node& operator+(double x, Node& parent) {
		return add(parent, x);
	};

	Node& subtract(Node& parent1, Node& parent2) {
		Operation* subOp = new Subtract();
		Node* node = new Node(parent1, parent2, subOp);
		return *node;
	}

	Node& subtract(Node& parent, double x) {
		Operation* subOp = new Subtract(x, false);
		Node* node = new Node(parent, subOp);
		return *node;
	}

	Node& subtract(double x, Node& parent) {
		Operation* subOp = new Subtract(x, true);
		Node* node = new Node(parent, subOp);
		return *node;
	}

	Node& operator-(Node& parent1, Node& parent2) {
		return subtract(parent1, parent2);
	};

	Node& operator-(Node& parent, double x) {
		return subtract(parent, x);
	};

	Node& operator-(double x, Node& parent) {
		return subtract(x, parent);
	};

	Node& multiply(Node& parent1, Node& parent2) {
		Operation* multOp = new Multiply(1.0);
		Node* node = new Node(parent1, parent2, multOp);
		return *node;
	}

	Node& multiply(Node& parent, double x) {
		Operation* multOp = new Multiply(x);
		Node* node = new Node(parent, multOp);
		return *node;
	}

	Node& multiply(double x, Node& parent) {
		return multiply(parent, x);
	}

	Node& operator*(Node& parent1, Node& parent2) {
		return multiply(parent1, parent2);
	};

	Node& operator*(Node& parent, double x) {
		return multiply(parent, x);
	};

	Node& operator*(double x, Node& parent) {
		return multiply(parent, x);
	};


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
};