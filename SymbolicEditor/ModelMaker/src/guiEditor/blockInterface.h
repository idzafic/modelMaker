#pragma once
#include <unordered_map>
#include "arch/ArchiveIn.h"
#include "arch/ArchiveOut.h"

class blockInterface {
	std::unordered_map<size_t, int> cnts, _cntsSaved;
public:

	void saveState();
	void restoreState();
	void writeState(arch::ArchiveOut &f);
	void reloadState(arch::ArchiveIn& f);
	void resetAll();

	template<typename Type>
	int getCnt() {
		return cnts[typeid(Type).hash_code()];
	}

	template<typename Type>
	void increaseCnt() {
		++cnts[typeid(Type).hash_code()];
	}

};