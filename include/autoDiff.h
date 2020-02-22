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
		Node();
		Node(Node& parent);
		Node(Node& parent1, Node& parent2);
		Node(vector<Node*>& parents);
		
	public:
		double getValue();
		double getDerivative();
		
		friend class Input;
		friend class Add;
		friend class Subtract;
		friend class Multiply;
		friend class Divide;
		friend class NaturalLog;
		friend class Exponentiate;
		friend class Square;
		friend class SquareRoot;
		friend class Cube;
		friend class CubeRoot;
		friend class RaiseToPower;
		friend class Function;
		friend class Sine;
		friend class Cosine;
};

class Input: public Node {
	private:
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
};

class Add: public Node {
	private:
		double constant;
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Add(Node& parent, double constant_);
		Add(Node& parent1, Node& parent2);
		Add(vector<Node*>& parents);
};

//subtracts the second arg from the first
class Subtract: public Node {
	private:
		double constant;
		bool parentFirst;
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Subtract(Node& parent1, Node& parent2);
		Subtract(double constant_, Node& parent);
		Subtract(Node& parent, double constant_);
};

class Multiply: public Node {
	private:
		double constant;
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Multiply(Node& parent, double constant_);
		Multiply(Node& parent1, Node& parent2);
		Multiply(vector<Node*>& parents);
};

//divides the second arg by the first
class Divide: public Node {
	private:
		double constant;
		bool parentFirst;
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Divide(Node& parent1, Node& parent2);
		Divide(double constant_, Node& parent);
		Divide(Node& parent, double constant_);
};

class NaturalLog: public Node {
	private: 
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		NaturalLog(Node& parent);
		
};

class Exponentiate: public Node {
	private: 
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Exponentiate(Node& parent);
};

class Square: public Node {
	private: 
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Square(Node& parent);
};

class SquareRoot: public Node {
	private: 
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		SquareRoot(Node& parent);
};

class Cube: public Node {
	private: 
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Cube(Node& parent);
}; 

class CubeRoot: public Node {
	private: 
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		CubeRoot(Node& parent);
}; 

//raises first input to power of second
class RaiseToPower: public Node {
	private: 
		bool parentIsBase;	//else parent is exponent
		double constant;
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		RaiseToPower(double base, Node& parent);
		RaiseToPower(Node& parent, double exponent);
		RaiseToPower(Node& parent1, Node& parent2);
}; 

class Sine: public Node {
	private: 
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Sine(Node& parent);
}; 

class Cosine: public Node {
	private: 
		virtual void fillMyValue();
		virtual void updateParentDerivatives();
	public:
		Cosine(Node& parent);
}; 

//constructor requires that the function's graph is completely built when constructed
//alternatively, could allow use to build function further, and then "compile" it (which checks for errors, etc)
class Function {
	private:
		vector<Node*> nodes;
		vector<Input*> inputNodes;
		Node* outputNode;

	public:
		Function(vector<Input*>& inputNodes_);
		double evaluate(vector<double> args);
		vector<double> differentiate(vector<double> args);
};

Add& operator+(Node& parent1, Node& parent2);
