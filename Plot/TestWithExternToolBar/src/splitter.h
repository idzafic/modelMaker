#include <gui/SplitterLayout.h>
#include <gui/View.h>
#include <gui/plot/View.h>
#include "gui/ComboBox.h"
#include <gui/VerticalLayout.h>

class switcher;


class splitterLayout : public gui::View {
	gui::SplitterLayout _mainLayout;

	gui::ComboBox picker;
	gui::VerticalLayout v;
	switcher* props = nullptr;
	gui::plot::View& _graph;
	gui::View _settingsView;

	bool ignoreSelections = false;

public:
	splitterLayout(gui::plot::View& mainView);
	void refreshPicks();


	bool onChangedSelection(gui::ComboBox* pCmb) override;

	~splitterLayout();
	
};
