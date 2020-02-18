#include "AOIMgr.h"
#include "Logger.h"

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


AOIMgr::AOIMgr() : x_head(std::make_shared<AOINode>(-1, 0, 0)), y_head(std::make_shared<AOINode>(-1, 0, 0)),
x_tail(std::make_shared<AOINode>(-1, 0x0fffffff, 0)), y_tail(std::make_shared<AOINode>(-1, 0, 0x0fffffff))
{
	x_head->x_next = x_tail;
	x_tail->x_pre = x_head;
	y_head->y_next = y_tail;
	y_tail->y_pre = y_head;
}

void AOIMgr::addNode(int actorId, int x, int y)
{
	if (m_nodes.find(actorId) != m_nodes.end()) {
		Logger::logError("$add aoi node repeat, actorId:%d", actorId);
		return;
	}
	/*std::shared_ptr<AOINode*> node(new AOINode(actorId, x, y));*/
	std::shared_ptr<AOINode> node = std::make_shared<AOINode>(actorId, x, y);
	m_nodes.emplace(std::make_pair(actorId, node));

	// x¡¥±Ì
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

	// y¡¥±Ì
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

	Logger::logInfo("$add aoi node, actorId:%d", actorId);
}

void AOIMgr::removeNode(int actorId)
{
	std::shared_ptr<AOINode> x_cur_node = x_head->x_next;
	while (x_cur_node != NULL) {
		if (x_cur_node->actorId == actorId) {
			x_cur_node->x_pre->x_next = x_cur_node->x_next;
			x_cur_node->x_next->x_pre = x_cur_node->x_pre;
			break;
		}
		x_cur_node = x_cur_node->x_next;
	}

	printf("remove x link\n");
	std::shared_ptr<AOINode> y_cur_node = y_head->y_next;
	while (y_cur_node != NULL) {
		if (y_cur_node->actorId == actorId) {
			y_cur_node->y_pre->y_next = y_cur_node->y_next;
			y_cur_node->y_next->y_pre = y_cur_node->y_pre;
			break;
		}
		y_cur_node = y_cur_node->y_next;
	}

	if (m_nodes.find(actorId) != m_nodes.end()) {
		m_nodes.erase(actorId);
	}
	printf("remove y link\n");
	Logger::logInfo("$remove aoi node, actorId:%d", actorId);
}

void AOIMgr::moveNode(int actorId, int x, int y)
{
	auto iter = m_nodes.find(actorId);
	if (iter == m_nodes.end()) {
		return;
	}
	std::shared_ptr<AOINode> aoi_node = iter->second;
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
