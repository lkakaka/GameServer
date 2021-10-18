#include "TaskMgr.h"
#include "Logger.h"

TaskMgr::TaskMgr() {

}

void TaskMgr::addTask(std::function<void()> task) {
	std::unique_lock<std::mutex> lock(m_taskLock);
	m_tasks.emplace_back(task);
}

void TaskMgr::runTask() {
	if (m_tasks.size() == 0) return;
	std::unique_lock<std::mutex> lock(m_taskLock);
	std::vector<std::function<void()>> tmp = m_tasks;
	m_tasks.clear();
	lock.unlock();
	for (std::function<void()> func : tmp) {
		func();
	}
}