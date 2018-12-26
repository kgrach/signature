#pragma once

#include "ThreadPoolIo.h"
#include "ItemConveyer.h"


template<>
bool ThreadPool::ThreadPoolIO<IItemWrite>::IoPending(std::shared_ptr<IOOverlapped<IItemWrite>>& ov_item) {
	OVERLAPPED* ov = static_cast<OVERLAPPED*>(ov_item.get());

	ov_item->SetOffset(ov_item->m_item->GetOffset());

	return ::WriteFile(m_hFile, ov_item->m_item->GetBuff().data(), ov_item->m_item->GetBuff().size(), NULL, ov) ? true : false;
}

template <>
void ThreadPool::ThreadPoolIO<IItemWrite>::IoCompletion(OVERLAPPED* Overlapped, ULONG IoResult, PLARGE_INTEGER NumberOfBytesTransferred) {

	IOOverlapped<IItemWrite> *p = static_cast<IOOverlapped<IItemWrite>*>(Overlapped);

	p->m_item->Callback();
	p->m_item = nullptr;

	//p->Clear();
	m_ov_complete.push(m_ov[p->m_num]);
}


using WriteFileIO = ThreadPool::ThreadPoolIO<IItemWrite>;