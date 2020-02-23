#pragma once

#include <vector>
#include <cmath>

namespace ad {
	struct Operation {
		virtual ~Operation(){};
		virtual double evaluate(std::vector<double>&) { return 0.0; }
		virtual std::vector<double> differentiate(std::vector<double>&) {return std::vector<double>(0); }
	};

	struct Inherit: Operation {
		virtual double evaluate(std::vector<double>& x) {
			if(x.size() != 1) {
	 			throw "Input to Inherit Operation must have exactly one argument";
	 		}
			return x[0];
		}
		virtual std::vector<double> differentiate(std::vector<double>& x) {
			return std::vector<double>{1.0};
		}
	};

	struct Add: Operation {
		double constant;
		virtual double evaluate(std::vector<double>& x) {
			double sum(constant);
			int n = x.size();
			for(int i=0; i<n; i++) {
				sum += x[i];
			}
			return sum;
		}
		virtual std::vector<double> differentiate(std::vector<double>& x) {
			return std::vector<double>(x.size(), 1.0);
		}
		
		Add(double constant_): constant(constant_){};
	};

	struct Subtract: Operation {
		double constant;
		bool useConstant;
		bool constantFirst;
		virtual double evaluate(std::vector<double>& x) {
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
		virtual std::vector<double> differentiate(std::vector<double>& x) {
			if(useConstant){ 
				if(constantFirst){
					return std::vector<double>{-1.0};
				}
				return std::vector<double>{1.0};
			}
			return std::vector<double>{1.0,-1.0};
		}
	
		Subtract(): constant(0.0), useConstant(false), constantFirst(false) {}
		Subtract(double constant_, bool constantFirst_): constant(constant_), useConstant(true), constantFirst(constantFirst_) {}
	};

	struct Multiply: Operation {
		double constant;
		virtual double evaluate(std::vector<double>& x) {
			double prod(constant);
			int n = x.size();
			for(int i=0; i<n; i++) {
				prod *= x[i];
			}
			return prod;
		}
		virtual std::vector<double> differentiate(std::vector<double>& x) {
			int n = x.size();
			std::vector<double> output(n, constant);
			for(int i=0; i<n; i++) {
				for(int j=0; j<n; j++) {
					if(j==i) {
						continue;
					}
					output[i] *= x[j];
				}
			}
			return output;
		}

		Multiply(double constant_): constant(constant_){};
	};
	
	struct Divide: Operation {
		double constant;
		bool useConstant;
		bool constantFirst;
		virtual double evaluate(std::vector<double>& x) {
			if(useConstant){
				if(x.size() != 1) {
					throw "Input to Divide Operation must have exactly one argument when using constant";
				}
				if(constantFirst) {
					if(x[0] == 0) {
						throw "Divide Operation tried to divide by zero";
					}
					return constant/x[0];
				}
				return x[0]/constant;
			}
			if(x.size() != 2) {
				throw "Input to Divide Operation must have exactly two arguments";
			}
			if(x[1] == 0) {
				throw "Divide Operation tried to divide by zero";
			}
			return x[0]/x[1];		
		}
		virtual std::vector<double> differentiate(std::vector<double>& x) {
			if(useConstant){ 
				if(constantFirst){
					return std::vector<double>{-constant/(x[0]*x[0])};
				}
				return std::vector<double>{1.0/constant};
			}
			return std::vector<double>{1.0/x[1], -x[0]/(x[1]*x[1])};
		}
	
		Divide(): constant(0.0), useConstant(false), constantFirst(false) {}
		Divide(double constant_, bool constantFirst_): constant(constant_), useConstant(true), constantFirst(constantFirst_) {
			if(!constantFirst && constant == 0) {
				throw "Tried to create a Divide operation that divides by zero";
			}
		}
	};
	
	struct Log: Operation {
		double base;
		bool doNaturalLog;
		virtual double evaluate(std::vector<double>& x) {
			if(x.size() != 1) {
				throw "Input to Log Operation must have exactly one argument";
			}
			if(x[0] <= 0) {
				throw "Log operation tried to take log of non-positive number";
			}
			if(doNaturalLog) {
				return log(x[0]);
			} else {
				return log(x[0])/log(base);
			}
		}
		virtual std::vector<double> differentiate(std::vector<double>& x) {
			if(x.size() != 1) {
				throw "Input to Log Operation must have exactly one argument";
			}
			if(x[0] == 0) {
				throw "Log Operation tried to divide by zero during differentiation";
			}
			if(doNaturalLog) {
				return std::vector<double>{1.0/x[0]};
			} else {
				return std::vector<double>{1.0/(log(base)*x[0])};
			}
		}
		
		Log(): base(0.0), doNaturalLog(true) {}
		Log(double base_): base(base_), doNaturalLog(false) {
			if(base <= 0) {
				throw "Log operation requires base > 0";
			}
		}
	};
}