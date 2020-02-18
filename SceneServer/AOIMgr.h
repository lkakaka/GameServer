#pragma once

#include <memory>
#include <string>
#include <map>

class AOINode
{
public:
	int actorId;
	int x, y;
	std::shared_ptr<AOINode> x_pre, x_next;
	std::shared_ptr<AOINode> y_pre, y_next;

	AOINode(int actorId, int x, int y);
	~AOINode();

	std::string toString();
};

class AOIMgr
{
private:
	std::shared_ptr<AOINode> x_head, x_tail;
	std::shared_ptr<AOINode> y_head, y_tail;
	std::map<int, std::shared_ptr<AOINode>> m_nodes;
public:
	AOIMgr::AOIMgr();
	void addNode(int actorId, int x, int y);
	void removeNode(int actorId);
	void moveNode(int actorId, int x, int y);

	void dump();
};

