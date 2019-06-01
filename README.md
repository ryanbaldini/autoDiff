A C++ library for automatic differentiation. Intended primarily for my own edification, but it does work.

One can build highly complex functions by chaining together many simple mathematical operations on many inputs. The derivative of this function with respect to each input can then be computed via the chain rule, using the derivative functions of each simple operation. So using the library involves building a computational graph via a set of predefined nodes with a set operation. Then you simply ask for the derivative given some input.

This project was inspired in part by thinking about how deep learning frameworks (e.g. Tensorflow) work, behind the scenes. Training neural networks involves computing a lot of gradients, and autodiff is probably the only feasible way to do it on custom networks.

As of now, the library only handles single float nodes, in double precision. I might get around to incorporating a linear algebra library (like Armadillo) so that nodes can have either a float, vector, or matrix.
