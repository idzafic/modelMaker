#include <gui/Window.h>
#include <gui/plot/View.h>
#include <gui/MenuBar.h>
#include <gui/Font.h>
#include "splitter.h"
#include "toolbar.h"


class MenuBar : public gui::MenuBar {
    gui::SubMenu open, save;
public: 
    MenuBar() : gui::MenuBar(2), open(1, "Open", 2), save(2, "Export", 2) {

        auto &items = open.getItems();
        items[0].initAsActionItem("Open function", 1);
        items[1].initAsActionItem("Append function", 2);

        auto& items2 = save.getItems();
        items2[0].initAsActionItem("txt", 10);
        items2[1].initAsActionItem("xml", 11);
        //items2[2].initAsActionItem("EPS", 3);
        //items2[3].initAsActionItem("PDF", 4);
        //items2[4].initAsActionItem("SVG", 5);

        auto& items3 = getItems();

        items3[0] = &open;
        items3[1] = &save;


    }

};


class MainWindow : public gui::Window
{
private:

    gui::Font font;

    MenuBar _mainMenuBar;
    ToolBar _toolBar;
    gui::plot::View _graph;
    splitterLayout splitter;

    bool resetGraph = true;

public:
    MainWindow();
    ~MainWindow();

    bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
    bool open(const td::String& path);
    void loadFromTerminal(int argc, char** argv);


};




