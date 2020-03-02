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
			arma::mat evaluate(std::vector<arma::mat> args);
			std::vector<arma::mat> differentiate(std::vector<arma::mat> args);
			int nodeCount() {
				return nodes.size();
			}
	};

	Function::Function(std::vector<Node*>& inputNodes_): inputNodes(inputNodes_), outputNode(nullptr) {
		int nInputs = inputNodes.size();
		if(nInputs == 0) {
			throw "No inputs to function";
		}
		for(Node* inputNode : inputNodes) {
			if(inputNode->operation != nullptr) {
				throw "Input nodes to a function must not have an operation (i.e., require operation = nullptr)";
			}
		}
	
		//check that we have exactly one terminal Node
		for(Node* inputNode : inputNodes) {
			std::vector<Node*> terminalNodes = inputNode->findTerminalNodes();
			int nTerminalNodes = terminalNodes.size();
			if(nTerminalNodes == 0) {
				throw "At least one input lacks a terminal node. Circularity?";
			} else if(nTerminalNodes > 1) {
				throw "More than one terminal node. There must be only one.";
			} else {
				if(outputNode == nullptr) {
					outputNode = terminalNodes[0];
				} else {
					if(outputNode != terminalNodes[0]) {
						throw "More than one terminal node. There must be only one.";
					}
				}
			}
		}
		if(outputNode == nullptr) {
			throw "No terminal node. Circular graph?";
		}
		
		//check that there are not any origin nodes of this system not represented by the inputs
		std::vector<Node*> originNodes = outputNode->findOriginNodes();
		if(originNodes.size() > inputNodes.size()) {
			throw "There are more origin nodes in this graph than have been provided as inputs.";
		} else if(originNodes.size() < inputNodes.size()) {
			throw "There are fewer origin nodes in this graph than have been provided as inputs.";
		} else {
			for(Node* originNode : originNodes) {
				bool found(false);
				for(Node* inputNode : inputNodes) {
					if(inputNode == originNode) {
						found = true;
						break;
					}
				}
				if(!found) {
					throw "An origin node of this graph is not represented among the inputs nodes provided.";
				}
			}
		}
		
		
		//collect all nodes into std::vector
		for(Node* inputNode : inputNodes) {
			nodes.push_back(inputNode);
			std::vector<Node*> descendantNodes = inputNode->getDescendantNodes();
			for(Node* descendantNode : descendantNodes) {
				if(find(nodes.begin(), nodes.end(), descendantNode) == nodes.end()) {
					nodes.push_back(descendantNode);
				}
			}
		}
	
		//should probably also check for circularity directly, by seeing if any nodes have themselves as a parent?
		//should be impossible if used correctly?
	}

	arma::mat Function::evaluate(std::vector<arma::mat> args) {
		int nArgs = args.size();
		int nInputs = inputNodes.size();
		if(nArgs != nInputs) {
			throw "Number of args does not equal required number of inputs";
		}
	
		//set all nodes to unevaluated
		for(Node* node : nodes) {
			node->evaluated = false;
		}
	
		for(int i=0; i<nInputs; i++) {
			inputNodes[i]->value = args[i];
			inputNodes[i]->evaluate();	//recursive evaluation starts here
		}

		return outputNode->value;
	}

	std::vector<arma::mat> Function::differentiate(std::vector<arma::mat> args) {
		evaluate(args);
	
		//set all nodes to undifferentiated
		for(Node* node : nodes) {
			node->differentiatedParents = false;
			node->derivative = node->value;
			node->derivative.fill(0);
		}
	
		outputNode->derivative = 1.0; //derivative of output with respect to itself is 1
		outputNode->differentiate(); //recursive differentiation starts here
	
		int nInputs = inputNodes.size();
		std::vector<arma::mat> derivatives(nInputs);
		for(int i=0; i<nInputs; i++) {
			derivatives[i] = inputNodes[i]->derivative;
	 	}
	
		return derivatives;
	}
};