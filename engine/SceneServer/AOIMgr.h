#pragma once

#include <memory>
#include <string>
#include <map>
#include <set>
#include <vector>

class AOINode
{
public:
	int actorId;
	int x, y;
	int x_viewRange, y_viewRange; //  ”“∞∑∂Œß
	std::shared_ptr<AOINode> x_pre, x_next;
	std::shared_ptr<AOINode> y_pre, y_next;

	AOINode(int actorId, int x, int y, int xViewRange, int yViewRange);
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
	AOIMgr();
	void addNode(int actorId, int x, int y, int viewRange, std::set<int>& neighbours);
	void removeNode(int actorId, std::set<int>& neighbours);
	void moveNode(int actorId, int x, int y, std::set<int>& leaveIds, std::set<int>& enterIds);

	void getNeighbours(std::shared_ptr<AOINode>& node, std::set<int>& neighbours);

	void dump();
};

