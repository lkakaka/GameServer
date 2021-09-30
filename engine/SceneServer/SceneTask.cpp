#include "SceneTask.h"
#include "Logger.h"

void SceneTask::addTask(std::function<void()> task) {
	std::unique_lock<std::mutex> lock(m_taskLock);
	m_tasks.emplace_back(task);
}

void SceneTask::runTask() {
	if (m_tasks.size() == 0) return;
	std::unique_lock<std::mutex> lock(m_taskLock);
	std::vector<std::function<void()>> tmp = m_tasks;
	lock.unlock();
	for (std::function<void()> func : tmp) {
		func();
	}
}