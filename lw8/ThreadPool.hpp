#pragma once

#include <vector>
#include <atomic>
#include <queue>
#include <mutex>

using Task = std::function<void()>;

class ThreadPool
{
public:
	ThreadPool(const std::vector<ThreadPriority> & priorities, size_t coresCount)
	{
		for (const auto priority : priorities)
		{
			m_threads.emplace_back([&]() {
				while (m_keepAlive)
				{
					if (const auto task = GetNextTask(); task)
					{
						++m_tasksInProgress;
						task.value()();
						--m_tasksInProgress;
					}
				}
			});
			SetThreadAffinityMask(m_threads.back(), coresCount);
			SetThreadPriority(m_threads.back(), priority);
		}
	}

	void AddTasks(std::queue<Task> && tasks)
	{
		std::lock_guard lock(m_tasksMutex);
		while (!tasks.empty())
		{
			AddTask(std::move(tasks.front()));
			tasks.pop();
		}
	}

	void AddTask(Task && task)
	{
		std::lock_guard lock(m_tasksMutex);
		m_tasks.push(std::move(task));
	}

	void Join()
	{
		while (Wait());
	}

	~ThreadPool()
	{
		m_keepAlive = false;
		for_each(m_threads.begin(), m_threads.end(), std::mem_fn(&std::thread::join));
	}

private:
	std::optional<Task> GetNextTask()
	{
		std::lock_guard lock(m_tasksMutex);
		if (m_tasks.empty())
		{
			return std::nullopt;
		}

		const auto task = m_tasks.front();
		m_tasks.pop();
		return task;
	}

	bool Wait()
	{
		std::lock_guard lock(m_tasksMutex);
		return !m_tasks.empty() || m_tasksInProgress;
	}
	
	std::vector<std::thread> m_threads;
	std::queue<Task> m_tasks;
	std::mutex m_tasksMutex;
	std::atomic<size_t> m_tasksInProgress = 0;
	bool m_keepAlive = true;
};
