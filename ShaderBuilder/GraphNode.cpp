#include "GraphNode.h"
#include <assert.h>



GraphNode::GraphNode(std::string inName, int inNumInputs, int inNumOutputs) 
{ 
	mName = inName; 
	mValid = false;  
	mData = nullptr;
	setNumInputs(inNumInputs); 
	setNumOutputs(inNumOutputs); 
}


void GraphNode::connect(GraphNode* inSource, int inSourceOutput, int inInput)
{
	// test if the data is valid
	assert(inSource != nullptr);
	assert(inInput >= 0 && inInput < getNumInputs());
	assert(inSourceOutput >= 0 && inSourceOutput < inSource->getNumOutputs());

	// disconnect the previous connection
	disconnect(inInput);

	// set connection data
	mInputs[inInput].source = inSource;
	mInputs[inInput].target = this;
	mInputs[inInput].sourceId = inSourceOutput;
	mInputs[inInput].targetId = inInput;

	inSource->mOutputs[inSourceOutput].insert(&mInputs[inInput]);
	invalidate();
}


void GraphNode::disconnect(int inInput)
{
	// make sure the input is valid
	assert(inInput >= 0 && inInput < getNumInputs());

	// set the targets input pointer to null
	NodeConnection connection = mInputs[inInput];
	if (connection.source != nullptr)
		connection.source->mOutputs[connection.sourceId].erase(&connection);

	// reset the connection
	connection.source = nullptr;
	connection.target = nullptr;
	connection.sourceId = -1;
	connection.targetId = -1;

	invalidate();
}


// will invalidate this node and all children
void GraphNode::invalidate()
{
	// set validation for this node and all children
	mValid = false;
	for each (std::unordered_set<NodeConnection*> connections in mOutputs)
	{
		for each (NodeConnection* connection in connections)
		{
			if (connection->target != nullptr)
				connection->target->invalidate();
		}
	}
}


void GraphNode::execute()
{
	// if there is no data or invalid data, execute source nodes
	for each (NodeConnection connection in mInputs)
	{
		assert(connection.source != nullptr);
		if (!connection.source->mValid)
			connection.source->execute();
	}

	// make sure all data is now available
	for each (NodeConnection connection in mInputs)
		assert(connection.data == nullptr);

	// call our implementation
	if (doStuff())
		mValid = true;
}