#pragma once

#include <algorithm>

#define SELFANCESTOR "invalid graph: node is an ancestor of itself"

namespace ad {
	struct Node {
		Operation* operation;
		double value;
		double derivative;
		std::vector<Node*> parents;
		std::vector<Node*> children;
		bool evaluated;
		bool differentiatedParents;
		bool dynamicallyAllocated;
	
		void evaluate();
		void differentiate();
		void fillMyValue();
		void updateParentDerivatives();
		std::vector<Node*> getDescendantNodes();
		std::vector<Node*> findTerminalNodes();
		std::vector<Node*> findOriginNodes();
		void setParent(Node& node);
		bool nodeIsAncestor(Node* node);
		void unlink();
		void deleteDynamicallyAllocatedAncestors();
		void replaceWithDynamicCopy();
	
		Node();
		Node(Node& parent); //copy constructor just makes this node an inherit descendant of the arg node
		Node(Node& parent, Operation* operation);
		Node(Node& parent1, Node& parent2, Operation* operation);
		Node(std::vector<Node*>& parents, Operation* operation);
		~Node();
		
		Node& operator= (Node& node);
	
		double getValue();
		double getDerivative();
	};
	
	//assignment operator
	//directly inherit from node passed in
	//if this node is already in the graph, place that copy on the heap
	Node& Node::operator= (Node& parent)
	{
		if(this == &parent) {
			return *this;
		}
		
		//if the assignment operator is being called, then this node probably already exists
		//if it has any connections at all to the system now, then make a copy of it on the heap
		if(this->parents.size() > 0 || this->children.size() > 0) {
			this->replaceWithDynamicCopy();
		}
 
		//just be a descendant of the node that is passed in
		if(operation != nullptr) {
			delete operation;
		}
		operation = new Inherit;
		parents.resize(1);
		parents[0] = &parent;
		children.resize(0);
		parent.children.push_back(this);
		
		if(nodeIsAncestor(this)) {
			throw SELFANCESTOR;
		}
		
		// return the existing object so we can chain this operator
		return *this;
	}

	//base constructor used for input nodes
	Node::Node(): operation(nullptr), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		if(nodeIsAncestor(this)) {
			throw SELFANCESTOR;
		}
	}

	//this is the copy constructor. it will create a new node that simply inherits the value of the previous node.
	Node::Node(Node& parent): operation(new Inherit), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		setParent(parent);
		if(nodeIsAncestor(this)) {
			throw SELFANCESTOR;
		}
	}

	Node::Node(Node& parent, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		setParent(parent);
		if(nodeIsAncestor(this)) {
			throw SELFANCESTOR;
		}
	}

	Node::Node(Node& parent1, Node& parent2, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		setParent(parent1);
		setParent(parent2);
		if(nodeIsAncestor(this)) {
			throw SELFANCESTOR;
		}
	}

	Node::Node(std::vector<Node*>& parents, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		int nParents = parents.size();
		for(int i=0; i<nParents; i++) {
			setParent(*parents[i]);
		}
		if(nodeIsAncestor(this)) {
			throw SELFANCESTOR;
		}
	}
	
	void Node::unlink() {
		//remove self from each parent's children vector
		for(Node* parent : parents) {
			std::vector<Node*> newChildVec;
			for(Node* child : parent->children) {
				if(child != this) {
					newChildVec.push_back(child);
				}
			}
			parent->children = newChildVec;
		}
		
		//remove self from each child's parent vector
		for(Node* child : children) {
			std::vector<Node*> newParentVec;
			for(Node* parent : child->parents) {
				if(parent != this) {
					newParentVec.push_back(parent);
				}
			}
			child->parents = newParentVec;
		}
	}
	
	void Node::deleteDynamicallyAllocatedAncestors() {
		//go through each parent and delete if dynamically allocated
		//since the parent vector will generally be resized when parent is deleted (due to unlink()), we need to iterate carefully through this!
		while(true) {
			bool hasDynamicallyAllocatedParent(false);
			for(Node* parent : parents) {
				if(parent->dynamicallyAllocated){
					hasDynamicallyAllocatedParent = true;
					delete parent; //calls destructor first, which will in turn delete any dynamically allocated ancestors of parent
					break;
				}
			}
			if(!hasDynamicallyAllocatedParent) {
				return;
			}
		}
	}
	
	//replace this node with a copy of it on the heap. 
	//the copy takes any connections that this one had, effectively disconnecting this node from the system
	void Node::replaceWithDynamicCopy() {
		Node* node = new Node;
		node->operation = this->operation;
		node->dynamicallyAllocated = true;
		for(Node* parent : this->parents) {
			node->setParent(*parent);	
		}
		node->children = this->children;
		for(Node* child : this->children) {
			child->parents.push_back(node);
		}
		
		this->operation = nullptr;
		this->unlink();
	}
	
	Node::~Node() {
		deleteDynamicallyAllocatedAncestors();
		if(operation != nullptr) {
			delete operation;
			operation = nullptr;
		}
		unlink();
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
		std::vector<Node*> terminalNodes;
		int nChildren = children.size();
		if(nChildren == 0) {
			terminalNodes.push_back(this);
			return terminalNodes;
		} else {
			for(int i=0; i<nChildren; i++) {
				std::vector<Node*> childterminalNodes = children[i]->findTerminalNodes(); //recursive
				int nChildterminalNodes = childterminalNodes.size();
				//put in terminalNodes if not in there yet
				for(int j=0; j<nChildterminalNodes; j++) {
					if(find(terminalNodes.begin(), terminalNodes.end(), childterminalNodes[j]) == terminalNodes.end()) {
						terminalNodes.push_back(childterminalNodes[j]);
					}
				}
			}
		}
		return terminalNodes;
	}

	std::vector<Node*> Node::findOriginNodes() {
		std::vector<Node*> originNodes;
		int nParents = parents.size();
		if(nParents == 0) {
			originNodes.push_back(this);
			return originNodes;
		} else {
			for(int i=0; i<nParents; i++) {
				std::vector<Node*> parentOriginNodes = parents[i]->findOriginNodes(); //recursive
				int nParentOriginNodes = parentOriginNodes.size();
				//put in originNodes if not in there yet
				for(int j=0; j<nParentOriginNodes; j++) {
					if(find(originNodes.begin(), originNodes.end(), parentOriginNodes[j]) == originNodes.end()) {
						originNodes.push_back(parentOriginNodes[j]);
					}
				}
			}
		}
		return originNodes;
	}

	bool Node::nodeIsAncestor(Node* node) {
		int nParents = parents.size();
		if(nParents == 0) {
			return false;
		}
		for(int i=0; i<nParents; i++) {
			if(parents[i] == node) {
				return true;
			}
		}
		for(int i=0; i<nParents; i++) {
			if(parents[i]->nodeIsAncestor(node)) {
				return true;
			}
		}
		return false;
	}

	void Node::setParent(Node& node) {
		parents.push_back(&node);
		node.children.push_back(this);
	}

	Node& add(Node& parent1, Node& parent2) {
		Operation* op = new Add(0.0);
		Node* node = new Node(parent1, parent2, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& add(Node& parent, double x) {
		Operation* op = new Add(x);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
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
		Operation* op = new Subtract();
		Node* node = new Node(parent1, parent2, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& subtract(Node& parent, double x) {
		Operation* op = new Subtract(x, false);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& subtract(double x, Node& parent) {
		Operation* op = new Subtract(x, true);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
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
		Operation* op = new Multiply(1.0);
		Node* node = new Node(parent1, parent2, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& multiply(Node& parent, double x) {
		Operation* op = new Multiply(x);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
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

	Node& divide(Node& parent1, Node& parent2) {
		Operation* op = new Divide();
		Node* node = new Node(parent1, parent2, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& divide(Node& parent, double x) {
		Operation* op = new Divide(x, false);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& divide(double x, Node& parent) {
		Operation* op = new Divide(x, true);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& operator/(Node& parent1, Node& parent2) {
		return divide(parent1, parent2);
	};

	Node& operator/(Node& parent, double x) {
		return divide(parent, x);
	};

	Node& operator/(double x, Node& parent) {
		return divide(x, parent);
	};
	
	Node& log(Node& parent, double base = -1) {
		Operation* op;
		if(base == -1) {
			op = new Log();
		} else {
			op = new Log(base);
		}
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}
	
	Node& exp(Node& parent) {
		Operation* op = new Exp;
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

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