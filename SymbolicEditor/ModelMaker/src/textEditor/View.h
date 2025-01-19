#pragma once
#include "CodeEdit.h"
#include "../../../EquationToXML/inc/nodes.h"
#include <gui/HorizontalLayout.h>
#include "../BaseView.h"
#include <td/StringConverter.h>



class TextEditorView : public ViewForTab::BaseClass {

	CodeEdit textMain;
	gui::HorizontalLayout _hl;

	std::unordered_set<td::String> _varsNotRoot;
	std::unordered_set<td::String> _paramsNotRoot;
	std::unordered_set<td::String> _expressions;

	friend class CodeEdit;
public:
	TextEditorView();
	void getModel(ModelNode& model) override;
	bool save(const td::String& path, const td::String& settingsString) override;
	void saveAs(const td::String& settingsString, td::String *newPath) override;
	bool openFile(const td::String& path, td::String& settingsString) override;
	void refreshVisuals() override;

};
