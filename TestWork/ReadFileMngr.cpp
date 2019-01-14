#include "ReadFileMngr.h"
#include "ISettings.h"
#include "ItemConveyer.h"
#include "ReadFileIO.h"
#include <iostream>


ReadFileMngr::ReadFileMngr(std::shared_ptr<ISettings>& settings) : m_settings(settings){
}

bool ReadFileMngr::InitializeWork(size_t& size) {

	m_hfileSrc = std::shared_ptr<HANDLE>(new HANDLE(INVALID_HANDLE_VALUE), [](HANDLE* p) { ::CloseHandle(*p); delete p; });

	*m_hfileSrc = ::CreateFile(m_settings->GetInFileName().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if (*m_hfileSrc == INVALID_HANDLE_VALUE)
		return false;

	if (!::GetFileSizeEx(*m_hfileSrc, &m_SrcFileSize)) {
		return false;
	}

	size = (size_t)m_SrcFileSize.QuadPart;

	m_offset = 0ull;

	m_io = std::move(std::make_shared<ThreadPool::ThreadPoolIO<IItemRead>>(m_hfileSrc));

	return true;
}

bool ReadFileMngr::Reading(std::shared_ptr<IItemRead>& item) {

	item->GetBuff().resize(m_settings->GetChunkSize());
	item->SetOffset(m_offset.fetch_add(m_settings->GetChunkSize()));

	if (item->GetOffset() >= m_SrcFileSize.QuadPart) {
		std::cout << "Done" << std::endl;
		return false;
	}

	return m_io->StartIO(item);
}
