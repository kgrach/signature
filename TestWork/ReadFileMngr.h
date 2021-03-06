#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <concurrent_queue.h>

#include "ScopeHandle.h"
#include "ReadFileIO.h"

class ISettings;

using fReadChunckCallback = std::function<void(size_t, std::unique_ptr<std::vector<unsigned char>>)>;

class ReadFileMngr {

	std::shared_ptr<ISettings> m_settings;

	ScopeHandle m_hfileSrc;
	LARGE_INTEGER m_SrcFileSize;

	std::atomic<unsigned __int64> m_offset;

	std::vector<std::shared_ptr<ReadFileIO>> m_vecIOTask;
	concurrency::concurrent_queue<std::shared_ptr<ReadFileIO>> m_vecIOTaskCompleted;


	std::function<void(size_t, std::unique_ptr<std::vector<unsigned char>>)> m_fCallback;
	
	void ReadComplete(size_t taskNum, size_t offset, std::unique_ptr<std::vector<unsigned char>> data);

public:
	ReadFileMngr(std::shared_ptr<ISettings>& settings, fReadChunckCallback f);

	bool Reading(std::unique_ptr<std::vector<unsigned char>> buff);
	bool InitializeWork();
};