#pragma once
#include <gui/ToolBar.h>
#include <gui/plot/View.h>


class ToolBar: public gui::ToolBar{
    static constexpr int cnt = sizeof(gui::plot::View::buttonsImages) / sizeof(gui::plot::View::buttonsImages[0]);
    gui::Image _imgs[cnt];
public:

    ToolBar():
    gui::ToolBar("main", cnt)
    {
        const char *labels[cnt] = {"Minimize margins", "Show grid", "Show legend", "Save", "Fit to window", "Settings", "Show annotations", "Delete Annotations", "Help"};
        const char *desc[cnt] = {"Set margins to zero", "Show grid lines", "Toggle plot legend",
        "Export as data or image", "Reset to original state", "Plot settings", "Show lines added with v/h", "Delete all lines added with v/h", "Show help (keyboard shortcuts)"};

        for(int i = 0; i<cnt; ++i){
            if(i == 7 || i==4) //preskakanje nekih ikona
                continue;
            _imgs[i].load(gui::plot::View::buttonsImages[i]);
            addItem(labels[i], _imgs + i, desc[i], 100, 0, 0, i);
        }
    }

};
