#pragma once

#include <vector>

using namespace std;

//virtual class; not to be used directly
class Node {
	protected:
		double value;
		double derivative;
		vector<Node*> parents;
		vector<Node*> children;
		bool evaluated;
		bool differentiatedParents;
		
		void evaluate();
		void differentiate();
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
		vector<Node*> getDescendantNodes();
		vector<Node*> findTerminalNodes();
		void setParent(Node& node);
	
	public:
		Node();
		double getValue();
		double getDerivative();
		
		friend class Input;
		friend class AddConstant;
		friend class AddNodes;
		friend class SubtractNodes;
		friend class MultiplyNodes;
		friend class MultiplyByConstant;
		friend class DivideNodes;
		friend class Function;
};

class Input: public Node {
	private:
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
};

class AddConstant: public Node {
	private:
		double constant;
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		AddConstant(Node& node, double constant_);
};

class AddNodes: public Node {
	private:
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		AddNodes(Node& node1, Node& node2);
};

//subtracts the second arg from the first
class SubtractNodes: public Node {
	private:
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		SubtractNodes(Node& node1, Node& node2);
};

class MultiplyNodes: public Node {
	private:
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		MultiplyNodes(Node& node1, Node& node2);
};

class MultiplyByConstant: public Node {
	private:
		double constant;
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		MultiplyByConstant(Node& node, double constant_);
};

//divides the second arg by the first
class DivideNodes: public Node {
	private:
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		DivideNodes(Node& node1, Node& node2);
};

//constructor requires that the function's graph is completely built when constructed
//alternatively, could allow use to build function further, and then "compile" it (which checks for errors, etc)
class Function {
	private:
		vector<Node*> nodes;
		vector<Input*> inputNodes;
		Node* outputNode;

	public:
		Function(vector<Input*> inputNodes_);
		double evaluate(vector<double> args);
		vector<double> differentiate(vector<double> args);
};
