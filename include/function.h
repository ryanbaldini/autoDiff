#pragma once 

namespace ad {
	//constructor requires that the function's graph is completely built when constructed
	//alternatively, could allow use to build function further, and then "compile" it (which checks for errors, etc)
	class Function {
		private:
			std::vector<Node*> nodes;
			std::vector<Node*> inputNodes;
			Node* outputNode;

		public:
			Function(std::vector<Node*>& inputNodes_);
			double evaluate(std::vector<double> args);
			std::vector<double> differentiate(std::vector<double> args);
			int nodeCount() {
				return nodes.size();
			}
	};

	Function::Function(std::vector<Node*>& inputNodes_): inputNodes(inputNodes_), outputNode(nullptr) {
		int nInputs = inputNodes.size();
		if(nInputs == 0) {
			throw "No inputs to function";
		}
	
		//check that we have exactly one terminal Node
		for(int i=0; i<nInputs; i++) {
			std::vector<Node*> terminalOps = inputNodes[i]->findTerminalNodes();
			int nTerminalOps = terminalOps.size();
			if(nTerminalOps == 0) {
				throw "At least one input lacks a terminal node. Circularity?";
			} else if(nTerminalOps > 1) {
				throw "More than one terminal node. There must be only one.";
			} else {
				if(outputNode == nullptr) {
					outputNode = terminalOps[0];
				} else {
					if(outputNode != terminalOps[0]) {
						throw "More than one terminal node. There must be only one.";
					}
				}
			}
		}
		if(outputNode == nullptr) {
			throw "No terminal node. Circular graph?";
		}
	
		//collect all nodes into std::vector
		for(int i=0; i<nInputs; i++) {
			nodes.push_back(inputNodes[i]);
			std::vector<Node*> descendantNodes = inputNodes[i]->getDescendantNodes();
			int nDescendantNodes = descendantNodes.size();
			for(int j=0; j<nDescendantNodes; j++) {
				if(find(nodes.begin(), nodes.end(), descendantNodes[j]) == nodes.end()) {
					nodes.push_back(descendantNodes[j]);
				}
			}
		}
	
		//should probably also check for circularity directly, by seeing if any nodes have themselves as a parent?
		//should be impossible if used correctly?
	}

	double Function::evaluate(std::vector<double> args) {
		int nArgs = args.size();
		int nInputs = inputNodes.size();
		if(nArgs != nInputs) {
			throw "Number of args does not equal required number of inputs";
		}
	
		//set all nodes to unevaluated
		int nNodes = nodes.size();
		for(int i=0; i<nNodes; i++) {
			nodes[i]->evaluated = false;
		}
	
		for(int i=0; i<nInputs; i++) {
			inputNodes[i]->value = args[i];
			inputNodes[i]->evaluate();	//recursive evaluation starts here
		}

		return outputNode->value;
	}

	std::vector<double> Function::differentiate(std::vector<double> args) {
		evaluate(args);
	
		//set all nodes to undifferentiated
		int nNodes = nodes.size();
		for(int i=0; i<nNodes; i++) {
			nodes[i]->differentiatedParents = false;
			nodes[i]->derivative = 0.0;
		}
	
		outputNode->derivative = 1.0; //derivative of output with respect to itself is 1
		outputNode->differentiate(); //recursive differentiation starts here
	
		int nInputs = inputNodes.size();
		std::vector<double> derivatives(nInputs);
		for(int i=0; i<nInputs; i++) {
			derivatives[i] = inputNodes[i]->derivative;
	 	}
	
		return derivatives;
	}
};