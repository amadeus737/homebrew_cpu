#pragma once

#include <string>

class filestack
{
private:
	struct filenode
	{
		std::string name = "";
		int line = 0;
		filenode* parent;
	};

public:
	~filestack()
	{
		if (_currNode)
			delete _currNode;
	}

	const std::string& currName()
	{
		return _currNode->name;
	}

	int getLine()
	{
		return _currNode->line;
	}

	void push(std::string name)
	{
		bool isHeadNode = false;
		if (_currNode == nullptr)
		{
			_currNode = new filenode;
			isHeadNode = true;
		}

		_lastNode = _currNode;
		_lastNode->line++;

		filenode* tmp = new filenode;
		tmp->name = name;
		tmp->line = 0;
		tmp->parent = _currNode;

		_currNode = tmp;

		if (isHeadNode)
			_currNode->parent = nullptr;
	}
	
	void makeParentActive()
	{
		filenode* tmp = new filenode;
		tmp = _currNode;
		_currNode = _currNode->parent;
		_lastNode = tmp;
	}

	bool hasNullParent()
	{
		return _currNode->parent == nullptr;
	}

	bool processingNewFile()
	{
		return _lastNode != _currNode;
	}

private:
	filenode* _currNode = nullptr;
	filenode* _lastNode = nullptr;
};