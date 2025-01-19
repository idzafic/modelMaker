#pragma once
#include "gui/SplitterLayout.h"
#include "gui/View.h"
#include "canvas.h"
#include <gui/ViewScroller.h>
#include "propertySwitcher.h"
#include "../BaseView.h"





class GraphicalEditorView: public ViewForTab::BaseClass {
	gui::SplitterLayout spliter;
	properties props;
	
	kanvas _canvas;

	friend class kanvas;
public:
	GraphicalEditorView();
	void switchToView(int number) {
		props.showView(number, true);
	}


	bool save(const td::String& path, const td::String& settingsString);
	void saveAs(const td::String& settingsString, td::String* newPath);
	void getModel(ModelNode& model);
	void refreshVisuals();
	bool openFile(const td::String& path, td::String& settingsString);
	

};







