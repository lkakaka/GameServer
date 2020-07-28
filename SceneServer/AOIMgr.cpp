#include "AOIMgr.h"
#include "Logger.h"
#include <algorithm>
#include <iterator>

#define X_VIEW_RANGE 5
#define Y_VIEW_RANGE 5

AOINode::AOINode(int actorId, int x, int y) : actorId(actorId), x(x), y(y), 
x_pre(nullptr), x_next(nullptr), y_pre(nullptr), y_next(nullptr)
{

}

AOINode::~AOINode()
{
	printf("AOINode free, id:%d!!!\n", actorId);
}

std::string AOINode::toString()
{
	std::string s;
	char buff[128]{0};
	sprintf(buff, "id:%d,x:%d,y:%d", actorId, x, y);
	s.append(buff);
	return s;
}


AOIMgr::AOIMgr() : x_head(std::make_shared<AOINode>(-1, 0x8fffffff, 0)), y_head(std::make_shared<AOINode>(-1, 0, 0x8fffffff)),
x_tail(std::make_shared<AOINode>(-1, 0x0fffffff, 0)), y_tail(std::make_shared<AOINode>(-1, 0, 0x0fffffff))
{
	x_head->x_next = x_tail;
	x_tail->x_pre = x_head;
	y_head->y_next = y_tail;
	y_tail->y_pre = y_head;
}

void AOIMgr::addNode(int actorId, int x, int y, std::set<int>& neighbours)
{
	if (m_nodes.find(actorId) != m_nodes.end()) {
		Logger::logError("$add aoi node repeat, actorId:%d", actorId);
		return;
	}
	/*std::shared_ptr<AOINode*> node(new AOINode(actorId, x, y));*/
	std::shared_ptr<AOINode> node = std::make_shared<AOINode>(actorId, x, y);
	m_nodes.emplace(std::make_pair(actorId, node));

	// x链表
	std::shared_ptr<AOINode> x_cur_node = x_head->x_next;
	while (x_cur_node != NULL) {
		if (x_cur_node->x > x || x_cur_node == x_tail) {
			node->x_pre = x_cur_node->x_pre;
			node->x_pre->x_next = node;
			x_cur_node->x_pre = node;
			node->x_next = x_cur_node;
			break;
		}
		x_cur_node = x_cur_node->x_next;
	}

	// y链表
	std::shared_ptr<AOINode> y_cur_node = y_head->y_next;
	while (y_cur_node != NULL) {
		if (y_cur_node->y > y || y_cur_node == y_tail) {
			node->y_pre = y_cur_node->y_pre;
			node->y_pre->y_next = node;
			y_cur_node->y_pre = node;
			node->y_next = y_cur_node;
			break;
		}
		y_cur_node = y_cur_node->y_next;
	}


	getNeighbours(node, neighbours);

	Logger::logInfo("$add aoi node, actorId:%d", actorId);
	printf("neighbours addr=%p\n", &neighbours);
}

void AOIMgr::removeNode(int actorId, std::set<int>& neighbours)
{
	auto iter = m_nodes.find(actorId);
	if (iter == m_nodes.end()) {
		return;
	}

	std::shared_ptr<AOINode> x_cur_node = iter->second;
	getNeighbours(x_cur_node, neighbours);

	// 从X链上移除
	x_cur_node->x_pre->x_next = x_cur_node->x_next;
	x_cur_node->x_next->x_pre = x_cur_node->x_pre;

	printf("remove x link\n");
	std::shared_ptr<AOINode> y_cur_node = iter->second;
	// 从Y链上移除
	y_cur_node->y_pre->y_next = y_cur_node->y_next;
	y_cur_node->y_next->y_pre = y_cur_node->y_pre;

	m_nodes.erase(actorId);
	printf("remove y link\n");
	Logger::logInfo("$remove aoi node, actorId:%d", actorId);
}

void AOIMgr::moveNode(int actorId, int x, int y, std::set<int>& leaveIds, std::set<int>& enterIds)
{
	auto iter = m_nodes.find(actorId);
	if (iter == m_nodes.end()) {
		return;
	}
	std::shared_ptr<AOINode> aoi_node = iter->second;
	std::set<int> orgNeighbours;
	getNeighbours(aoi_node, orgNeighbours);

	int org_x = aoi_node->x;
	int org_y = aoi_node->y;
	aoi_node->x = x;
	aoi_node->y = y;

	if (org_x < x) {
		std::shared_ptr<AOINode> node = aoi_node->x_next;
		while (node != NULL) {
			if (node->x > x || node == x_tail) {
				aoi_node->x_pre->x_next = aoi_node->x_next;
				aoi_node->x_next->x_pre = aoi_node->x_pre;

				aoi_node->x_next = node;
				aoi_node->x_pre = node->x_pre;
				aoi_node->x_pre->x_next = aoi_node;
				node->x_pre = aoi_node;
				break;
			}
			node = node->x_next;
		}
	} else if (org_x > x) {
		std::shared_ptr<AOINode> node = aoi_node->x_pre;
		while (node != NULL) {
			if (node->x < x || node == x_head) {
				aoi_node->x_pre->x_next = aoi_node->x_next;
				aoi_node->x_next->x_pre = aoi_node->x_pre;

				aoi_node->x_next = node->x_next;
				node->x_next = aoi_node;
				aoi_node->x_next->x_pre = aoi_node;
				aoi_node->x_pre = node;
				break;
			}
			node = node->x_pre;
		}
	}

	if (org_y < y) {
		std::shared_ptr<AOINode> node = aoi_node->y_next;
		while (node != NULL) {
			if (node->y > y || node == y_tail) {
				aoi_node->y_pre->y_next = aoi_node->y_next;
				aoi_node->y_next->y_pre = aoi_node->y_pre;

				aoi_node->y_next = node;
				aoi_node->y_pre = node->y_pre;
				aoi_node->y_pre->y_next = aoi_node;
				node->y_pre = aoi_node;
				break;
			}
			node = node->y_next;
		}
	}
	else if (org_y > y) {
		std::shared_ptr<AOINode> node = aoi_node->y_pre;
		while (node != NULL) {
			if (node->y < y || node == y_head) {
				aoi_node->y_pre->y_next = aoi_node->y_next;
				aoi_node->y_next->y_pre = aoi_node->y_pre;

				aoi_node->y_next = node->y_next;
				node->y_next = aoi_node;
				aoi_node->y_next->y_pre = aoi_node;
				aoi_node->y_pre = node;
				break;
			}
			node = node->y_pre;
		}
	}

	std::set<int> newNeighbours;
	getNeighbours(aoi_node, newNeighbours);

	std::set<int> orgNeighSet;
	for (auto iter = orgNeighbours.begin(); iter != orgNeighbours.end(); iter++) {
		orgNeighSet.emplace(*iter);
	}

	std::set<int> newNeighSet;
	for (auto iter = newNeighbours.begin(); iter != newNeighbours.end(); iter++) {
		newNeighSet.emplace(*iter);
	}

	//std::set_difference()

	std::set_difference(orgNeighbours.begin(), orgNeighbours.end(), newNeighbours.begin(), newNeighbours.end(), std::inserter(leaveIds, std::end(leaveIds)));
	std::set_difference(newNeighbours.begin(), newNeighbours.end(), orgNeighbours.begin(), orgNeighbours.end(), std::inserter(enterIds, std::end(enterIds)));
}

void AOIMgr::getNeighbours(std::shared_ptr<AOINode>& node, std::set<int>& neighbours) {
	int x = node->x;
	std::set<int> xNeighbours;
	// 往后找在X_VIEW_RANGE范围类的玩家
	for (auto x_node = node->x_next; x_node != NULL; x_node = x_node->x_next) {
		if (x_node->x - x > X_VIEW_RANGE) break;
		xNeighbours.emplace(x_node->actorId);
	}
	//往前找在X_VIEW_RANGE范围类的玩家
	for (auto x_node = node->x_pre; x_node != NULL; x_node = x_node->x_pre) {
		if (x - x_node->x > X_VIEW_RANGE) break;
		xNeighbours.emplace(x_node->actorId);
	}

	int y = node->y;
	std::set<int> yNeighbours;
	// 往后找在Y_VIEW_RANGE范围类的玩家
	for (auto y_node = node->y_next; y_node != NULL; y_node = y_node->y_next) {
		if (y_node->y - y > Y_VIEW_RANGE) break;
		yNeighbours.emplace(y_node->actorId);
	}
	//往前找在Y_VIEW_RANGE范围类的玩家
	for (auto y_node = node->y_pre; y_node != NULL; y_node = y_node->y_pre) {
		if (y - y_node->y > Y_VIEW_RANGE) break;
		yNeighbours.emplace(y_node->actorId);
	}
	std::set_intersection(xNeighbours.begin(), xNeighbours.end(), yNeighbours.begin(), yNeighbours.end(), std::inserter(neighbours, std::end(neighbours)));
}

void AOIMgr::dump() 
{
	printf("x link:\n");
	std::shared_ptr<AOINode> x_cur_node = x_head->x_next;
	while (x_cur_node != NULL) {
		printf("%s\n", x_cur_node->toString().c_str());
		x_cur_node = x_cur_node->x_next;
	}

	printf("\ny link:\n");
	std::shared_ptr<AOINode> y_cur_node = y_head->y_next;
	while (y_cur_node != NULL) {
		printf("%s\n", y_cur_node->toString().c_str());
		y_cur_node = y_cur_node->y_next;
	}
}
