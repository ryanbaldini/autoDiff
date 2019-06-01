#pragma once

#include <vector>

using namespace std;

class Node {
	public:
		double value;
		double derivative;
		vector<Node*> parents;
		vector<Node*> children;
		bool evaluated;
		bool differentiatedParents;
		
		Node();
		void evaluate();
		void differentiate();
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
		vector<Node*> findTerminalNodes();
		void setMySubNetworkEvaluatedFalse();
		void setMySubNetworkDifferentiatedFalse();
		void setParent(Node* node);
};

class Input: public Node {
	public:
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
};

class AddConstant: public Node {
	public:
		AddConstant(Node* node, double constant_);
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	private:
		double constant;
};

class AddInputs: public Node {
	public:
		AddInputs(Node* node1, Node* node2);
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
};

//subtracts the second arg from the first
class SubtractInputs: public Node {
	public:
		SubtractInputs(Node* node1, Node* node2);
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
};

class MultiplyInputs: public Node {
	public:
		MultiplyInputs(Node* node1, Node* node2);
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
};

//divides the second arg by the first
class DivideInputs: public Node {
	public:
		DivideInputs(Node* node1, Node* node2);
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
};

//second constructor requires that the function's graph is completely built when constructed
//alternatively, could allow use to build function further, and then "compile" it (which checks for errors, etc)
class Function {
	public:
		vector<Input*> inputNodes;
		Node* outputNode;

		Function();
		Function(vector<Input*> inputNodes_);
		double evaluate(vector<double> args);
		vector<double> differentiate(vector<double> args);
};
