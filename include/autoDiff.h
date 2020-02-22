#pragma once

#include "operations.h"

using namespace std;

struct Node {
	Operation* operation;
	double value;
	double derivative;
	vector<Node*> parents;
	vector<Node*> children;
	bool evaluated;
	bool differentiatedParents;
	
	void evaluate();
	void differentiate();
	void fillMyValue();
	void updateParentDerivatives();
	vector<Node*> getDescendantNodes();
	vector<Node*> findTerminalNodes();
	void setParent(Node& node);
	Node();
	Node(Node& parent); //copy constructor
	Node(Node& parent, Operation* operation);
	Node(Node& parent1, Node& parent2);
	Node(Node& parent1, Node& parent2, Operation* operation);
	Node(vector<Node*>& parents);
	Node(vector<Node*>& parents, Operation* operation);
	
	double getValue();
	double getDerivative();
};

Node& add(Node& parent1, Node& parent2);
Node& add(Node& parent, double x);
Node& add(double x, Node& parent);
Node& operator+(Node& parent1, Node& parent2);
Node& operator+(Node& parent, double x);
Node& operator+(double x, Node& parent);

Node& subtract(Node& parent1, Node& parent2);
Node& subtract(Node& parent, double x);
Node& subtract(double x, Node& parent);
Node& operator-(Node& parent1, Node& parent2);
Node& operator-(Node& parent, double x);
Node& operator-(double x, Node& parent);

Node& multiply(Node& parent1, Node& parent2);
Node& multiply(Node& parent, double x);
Node& multiply(double x, Node& parent);
Node& operator*(Node& parent1, Node& parent2);
Node& operator*(Node& parent, double x);
Node& operator*(double x, Node& parent);


//constructor requires that the function's graph is completely built when constructed
//alternatively, could allow use to build function further, and then "compile" it (which checks for errors, etc)
class Function {
	private:
		vector<Node*> nodes;
		vector<Node*> inputNodes;
		Node* outputNode;

	public:
		Function(vector<Node*>& inputNodes_);
		double evaluate(vector<double> args);
		vector<double> differentiate(vector<double> args);
		int nodeCount() {
			return nodes.size();
		}
};
