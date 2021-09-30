#pragma once
#include <functional>
#include <mutex>
#include <vector>

class SceneTask
{
private:
	std::mutex m_taskLock;
	std::vector<std::function<void()>> m_tasks;
public:
	void addTask(std::function<void()> task);
	void runTask();
};

