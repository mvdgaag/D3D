#pragma once

#include <string>
#include <vector>
#include <unordered_set>


class GraphNode
{
public:
	GraphNode(std::string inName, int inNumInputs, int inNumOutputs);
	~GraphNode();

	std::string		getName() const;

	int				getNumOutputs() { return mOutputs.size; }
	void			setNumOutputs(int inNumOutputs) { mOutputs.resize(inNumOutputs); }
	int				getNumInputs() { return mInputs.size; }
	void			setNumInputs(int inNumInputs) { mInputs.resize(inNumInputs); }

	void			connect(GraphNode* inTarget, int inMyOutput, int inTargetInput);
	void			disconnect(int inMyOutput);
	void			invalidate();
	void			execute();
	virtual bool	doStuff();

private:

	struct NodeConnection
	{
		GraphNode* source;
		GraphNode* target;
		int sourceId;
		int targetId;
	};

	bool		mValid;
	void*		mData;
	std::string	mName;
	std::vector<NodeConnection> mInputs;
	std::vector<std::unordered_set<NodeConnection*>> mOutputs;
};

