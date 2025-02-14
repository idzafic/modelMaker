#include <gui/SplitterLayout.h>
#include "gui/View.h"
#include "./../../Graph/inc/Graph.h"
#include "gui/ComboBox.h"
#include "gui/VerticalLayout.h"

class switcher;



class splitterLayout : public gui::View {
	gui::SplitterLayout _mainLayout;

	gui::ComboBox picker;
	gui::VerticalLayout v;
	switcher* props = nullptr;
	Graph& _graph;
	gui::View _settingsView;

	bool ignoreSelections = false;

public:
	splitterLayout(Graph& mainView);
	void refreshPicks();


	bool onChangedSelection(gui::ComboBox* pCmb) override;

	~splitterLayout();
	
};