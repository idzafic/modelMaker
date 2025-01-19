#include "blockInterface.h"
//#include "tBlock.h"
//#include "sumBlock.h"
//#include "nlBlock.h"



void blockInterface::saveState()
{
	_cntsSaved = cnts;
}

void blockInterface::restoreState()
{
	cnts = _cntsSaved;
}

void blockInterface::writeState(arch::ArchiveOut& f)
{
	f << (td::UINT8)cnts.size();
	for (const auto& c : cnts)
		f << (td::UINT8)c.first << c.second;
}

void blockInterface::reloadState(arch::ArchiveIn& f)
{
	td::UINT8 size;
	f >> size;
	td::UINT8 hash;
	for (int i = 0; i < size; ++i) {
		f >> hash;
		f >> cnts[hash];
	}
		
}

void blockInterface::resetAll()
{
	cnts.clear();
	_cntsSaved.clear();
}
