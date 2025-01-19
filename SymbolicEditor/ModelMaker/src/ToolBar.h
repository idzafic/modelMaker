#pragma once
#include <gui/ToolBar.h>
#include <gui/ToolBarItem.h>
#include <gui/Image.h>
#include <gui/View.h>
#include <gui/HorizontalLayout.h>
#include <gui/NumericEdit.h>
#include <gui/Label.h>
#include "PopoverNewDocument.h"
#include "PopoverPlot.h"
#include "MenuBar.h"
#include <cnt/Array.h>

//enum class toolBarActionIDs{Simulate = 1};
//constexpr int toolBarID = 100;

class ToolBar : public gui::ToolBar
{
    gui::Image _settings;
    gui::Image _newEmpty;
    PopoverNewDocument _poNewDocument;
    PopoverPlot _poGraph;
    gui::Image _open;
    gui::Image _import;
    gui::Image _save;
    gui::Image _export;
    gui::Image _run;
    cnt::Array<gui::ToolBarItem*, 3> _txtOptions; //enable/disable options
public:
    enum class PopOverID : td::BYTE {NewFromTemplate=101, GraphCtrl};
	ToolBar()
    : gui::ToolBar("main", 10)
    , _settings(":settings")
    , _newEmpty(":newEmpty")
    , _open(":open")
    , _import(":import")
    , _save(":save")
    , _export(":export")
    , _run(":start")
	{
        addItem(tr("settingsShort"), &_settings, tr("settingsTT"), (td::BYTE) MenuBar::SubMenuID::App, 0, 0, (td::BYTE) MenuBar::ActionID::Settings);
        addItem(tr("newDocumentTB"), &_newEmpty, tr("newDocumentTT"), (td::BYTE) MenuBar::SubMenuID::Model, 0, 0, (td::BYTE) MenuBar::ActionID::EmptyModel);
        
        addItem(&_poNewDocument, (td::BYTE)PopOverID::NewFromTemplate, tr("newFromTemplate"), tr("newFromTemplateTT"));
        
        addItem(tr("openTB"), &_open, tr("openTT"), (td::BYTE) MenuBar::SubMenuID::Model, 0, 0, (td::BYTE) MenuBar::ActionID::OpenTextModel);
        
        auto pos = addItem(tr("saveTB"), &_save, tr("saveTT"), (td::BYTE) MenuBar::SubMenuID::Model, 0, 0, (td::BYTE) MenuBar::ActionID::Save);
        _txtOptions[0] = getItem(pos);
        
        addItem(tr("importTB"), &_import, tr("importTT"), (td::BYTE) MenuBar::SubMenuID::Model, 0, 0, (td::BYTE) MenuBar::ActionID::Import);
        
        pos = addItem(tr("exportTB"), &_export, tr("exportTT"), (td::BYTE) MenuBar::SubMenuID::Model, 0, 0, (td::BYTE) MenuBar::ActionID::Export);
        _txtOptions[1] = getItem(pos);
        
        addItem(&_poGraph, (td::UINT2)PopOverID::GraphCtrl, tr("plotTBI"), tr("plotRBITT"));
        
        addSpaceItem();
		pos = addItem(tr("startTB"), &_run, tr("startToolTip"), (td::BYTE) MenuBar::SubMenuID::Model, 0,0, (td::BYTE) MenuBar::ActionID::Simulate);
        _txtOptions[2] = getItem(pos);
	}
    
    void enableTxtOptions(bool bEnable)
    {
        for (auto item : _txtOptions)
            item->enable(bEnable);
    }
    
    void enablePlot(bool bEnable)
    {
        enablePopover((td::UINT2)ToolBar::PopOverID::GraphCtrl, bEnable);
    }
};
