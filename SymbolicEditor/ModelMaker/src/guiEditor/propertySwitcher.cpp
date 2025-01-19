#include "propertySwitcher.h"


properties::properties() :
	gui::ViewSwitcher(settingsCnt+1)
{

	addView(&modSettings, true);
	for(const auto&pok : pogledi)
		addView(pok);
}

modelSettings* properties::getModelSettings()
{
	return &modSettings;
}

void properties::showSettings(BlockBase* block)
{
	if (block == nullptr) {
		showView(0);
		return;
	}
	for (int i = 0; i < settingsCnt; ++i)
		if (pogledi_ID[i] == typeid(*block).hash_code()) {
			block->updateSettingsView(pogledi[i]);
			showView(i + 1);
			return;
		}
		
	int i = 0;
	
	
}
