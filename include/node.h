#pragma once

#include <algorithm>

namespace ad {
	class Node {
	public:
		Node();
		Node(Node& parent);
		~Node();
		
		Node& operator= (Node& node);
	
		double getValue();
		double getDerivative();
		
		friend Node& operator+(Node& parent1, Node& parent2);
		friend Node& operator+(Node& parent, double x);
		friend Node& operator-(Node& parent1, Node& parent2);
		friend Node& operator-(Node& parent, double x);
		friend Node& operator-(double x, Node& parent);
		friend Node& operator*(Node& parent1, Node& parent2);
		friend Node& operator*(Node& parent, double x);
		friend Node& operator/(Node& parent1, Node& parent2);
		friend Node& operator/(Node& parent, double x);
		friend Node& operator/(double x, Node& parent);
		friend Node& log(Node& parent, double base);
		friend Node& exp(Node& parent);
		
		void operator+=(Node& node);
		void operator+=(double x);
		void operator-=(Node& node);
		void operator-=(double x);
		void operator*=(Node& node);
		void operator*=(double x);
		void operator/=(Node& node);
		void operator/=(double x);
		
		friend class Function;
	
	private:
		Operation* operation;
		double value;
		double derivative;
		std::vector<Node*> parents;
		std::vector<Node*> children;
		bool evaluated;
		bool differentiatedParents;
		bool dynamicallyAllocated;
		
		Node(Node& parent, Operation* operation);
		Node(Node& parent1, Node& parent2, Operation* operation);
		Node(std::vector<Node*>& parents, Operation* operation);
		
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
		void replaceNodeWithSelf(Node& node);
	};
	
	void Node::replaceNodeWithSelf(Node& node) {
		if(this == &node) {
			return;
		}
		if(!node.dynamicallyAllocated) {
			throw "can't replace a non-dynamicallyAllocated node with self";
		}
		//then copy over info
		operation = node.operation;
		node.operation = nullptr; //so it's not deleted when node is deleted
		parents = node.parents;
		for(Node* parent : parents) {
			int nParentsChildren = parent->children.size();
			for(int i=0; i<nParentsChildren; i++) {
				if(parent->children[i] == &node) {
					parent->children[i] = this;
				}
			}
		}
		children = node.children;
		for(Node* child : children) {
			int nChildrensParents = child->parents.size();
			for(int i=0; i<nChildrensParents; i++) {
				if(child->parents[i] == &node) {
					child->parents[i] = this;
				}
			}
		}
		node.unlink(); //so it doesn't delete any dynamic ancestors
		delete &node;
	}
	
	//assignment operator
	Node& Node::operator= (Node& node) {
		if(this == &node) {
			return *this;
		}
		
		//if the assignment operator is being called, then this node probably already exists
		//if it has any connections at all to the system now, then make a copy of it on the heap
		if(parents.size() > 0 || children.size() > 0) {
			replaceWithDynamicCopy();
		}
				
		if(node.dynamicallyAllocated) {
			replaceNodeWithSelf(node);
		} else {
			//just be a descendant of the node that is passed in
			if(operation != nullptr) {
				delete operation;
			}
			operation = new Inherit;
			parents.resize(0);
			children.resize(0);
			setParent(node);
		}
		
		if(nodeIsAncestor(this)) {
			throw "invalid graph: node is an ancestor of itself";
		}
		
		return *this;
	}

	//base constructor used for input nodes
	Node::Node(): operation(nullptr), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {}

	//this is the copy constructor. 
	//if the node passed in is dynamicallyAllocated (not in scope - only possible when creating nodes with operators), replace that node with self
	//else, inherit it as a parent
	Node::Node(Node& node): value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		if(node.dynamicallyAllocated) {
			replaceNodeWithSelf(node);
		} else {
			operation = new Inherit;
			setParent(node);
		}
	}

	Node::Node(Node& parent, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		setParent(parent);
	}

	Node::Node(Node& parent1, Node& parent2, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		setParent(parent1);
		setParent(parent2);
	}

	Node::Node(std::vector<Node*>& parents, Operation* operation_): operation(operation_), value(0), derivative(0), evaluated(false), differentiatedParents(false), dynamicallyAllocated(false) {
		int nParents = parents.size();
		for(int i=0; i<nParents; i++) {
			setParent(*parents[i]);
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
		parents.resize(0);
		
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
		children.resize(0);
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
			int nChildParents = child->parents.size();
			for(int i=0; i<nChildParents; i++) {
				if(child->parents[i] == this) {
					child->parents[i] = node;
				}
			}
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

	Node& operator+(Node& parent1, Node& parent2) {
		Operation* op = new Add();
		Node* node = new Node(parent1, parent2, op);
		node->dynamicallyAllocated = true;
		return *node;
	};

	Node& operator+(Node& parent, double x) {
		Operation* op = new Add(x);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	};

	Node& operator+(double x, Node& parent) {
		return parent+x;
	};
	
	void Node::operator+=(Node& node) {
		*this = *this + node;
	}
	
	void Node::operator+=(double x) {
		*this = *this + x;
	}

	Node& operator-(Node& parent1, Node& parent2) {
		Operation* op = new Subtract();
		Node* node = new Node(parent1, parent2, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& operator-(Node& parent, double x) {
		Operation* op = new Subtract(x, false);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}
	
	Node& operator-(double x, Node& parent) {
		Operation* op = new Subtract(x, true);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}
	
	void Node::operator-=(Node& node) {
		*this = *this - node;
	}
	
	void Node::operator-=(double x) {
		*this = *this - x;
	}

	Node& operator*(Node& parent1, Node& parent2) {
		Operation* op = new Multiply();
		Node* node = new Node(parent1, parent2, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& operator*(Node& parent, double x) {
		Operation* op = new Multiply(x);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& operator*(double x, Node& parent) {
		return parent * x;
	}
	
	void Node::operator*=(Node& node) {
		*this = (*this) * node;
	}
	
	void Node::operator*=(double x) {
		*this = (*this) * x;
	}
	
	Node& operator/(Node& parent1, Node& parent2) {
		Operation* op = new Divide();
		Node* node = new Node(parent1, parent2, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& operator/(Node& parent, double x) {
		Operation* op = new Divide(x, false);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}

	Node& operator/(double x, Node& parent) {
		Operation* op = new Divide(x, true);
		Node* node = new Node(parent, op);
		node->dynamicallyAllocated = true;
		return *node;
	}
	
	void Node::operator/=(Node& node) {
		*this = (*this) / node;
	}
	
	void Node::operator/=(double x) {
		*this = (*this) / x;
	}

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