#pragma once
#include <gui/TextEdit.h>
#include <gui/HorizontalLayout.h>
#include <regex>
#include "../BaseView.h"

class TextEditorView;

class CodeEdit : public gui::TextEdit
{
    //gui::AsyncFn _asyncInsertTabs;
	TextEditorView* _parent;
	gui::Range _pastRange;
	td::String _text;
	std::cmatch match;
    TabStop _tabStop;
    td::BYTE _nTabsToInsert = 0;
protected:
	void onPaste() override;
    void onCut() override;
    void onFontChange() override;
    
	static const std::regex varPattern;
	static const std::regex attribPattern;
	static const std::regex expPattern;
	static const std::regex modelKeywordSearch;
	static const std::regex numberPattern;

	//varsBranch _rootVars;

	void syntaxHighlightingForCurrentLine();
	bool processKeyPress(const gui::Key& key);
	bool _cursorChangeGuard = true, _disableGuardOnNextChange = true;
    void insertTabsAfterReturn();
    
public:

	CodeEdit(TextEditorView* parent);
	bool onKeyPressed(const gui::Key& key) override;
	bool onKeyReleased(const gui::Key& key) override;
	void highlightSyntax(const gui::Range& r);
	void highlightSyntax(bool getText = true);
    void updateTabStops()
    {
        setTabStops(&_tabStop);
    }

};












/*

class varsBranch {
	std::set<td::String> _vars, _params;
	bool _init = false;
	std::vector<varsBranch*> _kids;
	varsBranch* _parent;
	int currentChildIndex = -1;

public:
	varsBranch(varsBranch* _parent = nullptr) :
		_parent(_parent)
	{

	}

	void addBranch() {
		_kids.emplace_back(this);
	}

	varsBranch* getPtrToNext() {
		if (_kids.size() < currentChildIndex + 2)
			return nullptr;
		++currentChildIndex;

		_kids[currentChildIndex]->currentChildIndex = -1;
		return _kids[currentChildIndex];
	}

	varsBranch* getPtrToPrevious() {
		return _parent;
	}


	~varsBranch() {
		for (const auto& ptr : _kids)
			delete ptr;
	}

};
*/


