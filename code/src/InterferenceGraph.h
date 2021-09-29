/* Autor: Dusan Lekic Datum: 14.06.2020. */
#pragma once
#include "IR.h"

#include <vector>
#include <stack>


typedef std::vector<std::vector<int>> interferenceMatrix;
typedef std::stack<Variable*> variableStack;


/* Class that represents interference graph.
This class build graph(matrix) and does resource allocation.*/
class InterferenceGraph
{
public:
	/* Constructor, destructor */
	InterferenceGraph(Variables& vars);
	~InterferenceGraph();

	/* Creates interference graph (matrix) from instructions. */
	void buildInterferenceGraph(Instructions& instructions);

	/* Creates stack from list of register variables. */
	void buildVariableStack(int br);

	/* Allocates real registers to variables according to the interference. */
	bool resourceAllocation();

	/* Prints interference matrix to the console. */
	void printInterferenceMatrix();

	interferenceMatrix m_im;
	variableStack m_varStack;  // same as list of instructions, just in form of stack

private:

	/* Resizes matrix to the passed size. */
	void resizeInterferenceMatrix(size_t size);

	/* Applies reg to the variable that has varPos for it's position. */
	void applyRegToVariable(int varPos, Regs reg);

	Variables& m_regVariables;  // register variables
};
