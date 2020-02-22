#pragma once

#include <vector>

using namespace std;

struct Operation {
	virtual double evaluate(vector<double>&) { return 0.0; }
	virtual vector<double> differentiate(vector<double>&) {return vector<double>(0); }
};

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

struct Inherit: Operation {
	virtual double evaluate(vector<double>& x) {
		if(x.size() != 1) {
 			throw "Input to Inherit Operation must have exactly one argument";
 		}
		return x[0];
	}
	virtual vector<double> differentiate(vector<double>& x) {
		return vector<double>{1.0};
	}
};

struct Add: Operation {
	double constant;
	virtual double evaluate(vector<double>& x) {
		double sum(constant);
		int n = x.size();
		for(int i=0; i<n; i++) {
			sum += x[i];
		}
		return sum;
	}
	virtual vector<double> differentiate(vector<double>& x) {
		return vector<double>(x.size(), 1.0);
	}

	Add(double constant_): constant(constant_){};
};
Node& add(Node& parent1, Node& parent2);
Node& add(Node& parent, double x);
Node& add(double x, Node& parent);
Node& operator+(Node& parent1, Node& parent2);
Node& operator+(Node& parent, double x);
Node& operator+(double x, Node& parent);

struct Subtract: Operation {
	double constant;
	bool useConstant;
	bool constantFirst;
	virtual double evaluate(vector<double>& x) {
		if(useConstant){
			if(x.size() != 1) {
				throw "Input to Subtract Operation must have exactly one argument when using constant";
			}
			if(constantFirst) {
				return constant - x[0];
			}
			return x[0] - constant;
		}
		if(x.size() != 2) {
			throw "Input to Subtract Operation must have exactly two arguments";
		}
		return x[0] - x[1];		
	}
	virtual vector<double> differentiate(vector<double>& x) {
		if(useConstant){ 
			if(constantFirst){
				return vector<double>{-1.0};
			}
			return vector<double>{1.0};
		}
		return vector<double>{1.0,-1.0};
	}
	
	Subtract(): constant(0.0), useConstant(false), constantFirst(false) {}
	Subtract(double constant_, bool constantFirst_): constant(constant_), useConstant(true), constantFirst(constantFirst_) {}
};
Node& subtract(Node& parent1, Node& parent2);
Node& subtract(Node& parent, double x);
Node& subtract(double x, Node& parent);
Node& operator-(Node& parent1, Node& parent2);
Node& operator-(Node& parent, double x);
Node& operator-(double x, Node& parent);

// Node& subtract(Node& parent1, Node& parent2);
// Node& operator-(Node& parent1, Node& parent2);

//
// struct SubtractConst: Operation {
// 	double constant;
// 	virtual double evaluate(vector<double>& x) {
// 		if(x.size() != 1) {
// 			throw "Input to SubtractConst Operation must have exactly one argument";
// 		}
// 		return x[0] - constant;
// 	}
// 	virtual vector<double> differentiate(vector<double>& x) {
// 		return vector<double>{1.0};
// 	}
//
// 	SubtractConst(double constant_): constant(constant_){};
// };
//
// struct SubtractFromConst: Operation {
// 	double constant;
// 	virtual double evaluate(vector<double>& x) {
// 		if(x.size() != 1) {
// 			throw "Input to SubtractFromConst Operation must have exactly one argument";
// 		}
// 		return constant - x[0];
// 	}
// 	virtual vector<double> differentiate(vector<double>& x) {
// 		return vector<double>{-1.0};
// 	}
//
// 	SubtractFromConst(double constant_): constant(constant_){};
// };


// class Input: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// };
//
// class Add: public Node {
// 	private:
// 		double constant;
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Add(Node& parent, double constant_);
// 		Add(Node& parent1, Node& parent2);
// 		Add(vector<Node*>& parents);
// };
//
// //subtracts the second arg from the first
// class Subtract: public Node {
// 	private:
// 		double constant;
// 		bool parentFirst;
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Subtract(Node& parent1, Node& parent2);
// 		Subtract(double constant_, Node& parent);
// 		Subtract(Node& parent, double constant_);
// };
//
// class Multiply: public Node {
// 	private:
// 		double constant;
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Multiply(Node& parent, double constant_);
// 		Multiply(Node& parent1, Node& parent2);
// 		Multiply(vector<Node*>& parents);
// };
//
// //divides the second arg by the first
// class Divide: public Node {
// 	private:
// 		double constant;
// 		bool parentFirst;
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Divide(Node& parent1, Node& parent2);
// 		Divide(double constant_, Node& parent);
// 		Divide(Node& parent, double constant_);
// };
//
// class NaturalLog: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		NaturalLog(Node& parent);
//
// };
//
// class Exponentiate: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Exponentiate(Node& parent);
// };
//
// class Square: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Square(Node& parent);
// };
//
// class SquareRoot: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		SquareRoot(Node& parent);
// };
//
// class Cube: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Cube(Node& parent);
// };
//
// class CubeRoot: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		CubeRoot(Node& parent);
// };
//
// //raises first input to power of second
// class RaiseToPower: public Node {
// 	private:
// 		bool parentIsBase;	//else parent is exponent
// 		double constant;
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		RaiseToPower(double base, Node& parent);
// 		RaiseToPower(Node& parent, double exponent);
// 		RaiseToPower(Node& parent1, Node& parent2);
// };
//
// class Sine: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Sine(Node& parent);
// };
//
// class Cosine: public Node {
// 	private:
// 		virtual void fillMyValue();
// 		virtual void updateParentDerivatives();
// 	public:
// 		Cosine(Node& parent);
// };

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
