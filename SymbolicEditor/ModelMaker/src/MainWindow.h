#pragma once
#include <gui/SplitterLayout.h>
#include <gui/Window.h>
#include <gui/TabView.h>
#include <td/String.h>
#include <gui/ViewSwitcher.h>

#include "guiEditor/contextMenu.h"
#include "MenuBar.h"
#include "ToolBar.h"
#include "../../EquationToXML/inc/nodes.h"
#include "BaseView.h"
#include "DataDrawer.h"
#include "StartingScreen.h"

class ViewForTab;

struct FileModels{
    td::String path;
    ModelNode model;
    std::chrono::nanoseconds timeModified;
    FileModels(const td::String &path):
        timeModified(0),
        path(path)
        {}
};

class DataDrawerWindow : public gui::Window
{
    DataDraw* storedView;
public:
    DataDrawerWindow(gui::Window* parent, DataDraw *mainView);
    ~DataDrawerWindow();
};

class MainWindow : public gui::Window
{
private:
    MenuBar _mainMenuBar;
    ToolBar _toolBar;
    gui::TabView _tabView;
    //gui::ViewSwitcher _switcherView;
    DataDraw _dataDrawer;
    bool _plotEmbedded = false;

    StartingScreen _startingView;
    ContextMenus _contextMenu;

    gui::Image _textEditorIcon, _guiEditorIcon;

    static std::vector<FileModels> _loadedModels;

    void simulate();
    template<typename tabType>
    void addTab(const td::String &path = td::String());

//    std::set<gui::BaseView*> _tabsToProcess;
    td::String _paths;
    
    DocumentType _prevDocType = DocumentType::Unknown;
//    bool _closeWindow = false;
    
protected:    
    bool handleUserEvent(td::UINT4 eventID, const td::Variant& userParam) override;
    void removePath();
    bool onToolbarsPopoverSelectionChange(gui::PopoverView* pPOView, td::UINT2 ctrlID, td::UINT2 selection) override;
    bool onChangedSelection(gui::TabView* pNavigator) override;
public:
    enum class DialogID{DataDrawer = 500, SaveData, Settings, Unused};

    MainWindow();
    
    bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
    
    struct exceptionCantAccessFile { td::String message; };
    struct exceptionCantFindTab { td::String message; };
    
    //void showStartScreen(bool show);
    void changeTabName(const td::String &name, ViewForTab *tab);
    void closeTab(gui::BaseView *tab);
    int simulate(ViewForTab* tab);
    bool openFile(const td::String& path);
    void onInitialAppearance() override;
    const ModelNode &getModelFromTabOrFile(const td::String &modelNameOrPath, const td::String &startingPath = td::String());
    DataDraw* getDataDrawer(bool openWindow = true);

    bool prepareForClose();
    bool shouldClose() override;
    ~MainWindow();
};




