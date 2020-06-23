#pragma once

#include <mutex>

#include "LogFileWriter.hpp"
#include "MyList.hpp"

class LogBuffer
{
public:
	LogBuffer(LogFileWriter & logFileWriter)
		:m_logWriter(logFileWriter)
		,m_dropperThread([&]() {
			while (m_keepAlive)
			{
				std::unique_lock lock(m_dropperMutex);
				m_cv.wait(lock, [&]() {
					return !m_keepAlive || (m_storage.size() == MAX_CAPACITY);
				});
				
				for (const auto & msg : m_storage)
				{
					m_logWriter.Log(msg);
				}
				m_storage.clear();
			}
		})
	{
	}

	~LogBuffer()
	{
		std::lock_guard<std::mutex> lg(m_storageMutex);
		m_keepAlive = false;
		m_cv.notify_one();
		m_dropperThread.join();
	}

	void Log(const std::string data)
	{
		std::lock_guard<std::mutex> lg(m_storageMutex);

		m_storage.push_back(data);
		if (m_storage.size() == MAX_CAPACITY)
		{
			m_cv.notify_one();
		}
	}

private:
	const size_t MAX_CAPACITY = 1'000;

	LogFileWriter & m_logWriter;
	CMyList<std::string> m_storage;
	std::mutex m_storageMutex;
	std::thread m_dropperThread;
	std::mutex m_dropperMutex;
	std::condition_variable m_cv;
	bool m_keepAlive = true;
};
