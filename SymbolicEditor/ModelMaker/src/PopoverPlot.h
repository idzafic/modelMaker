//
//  Created by Izudin Dzafic on 28/07/2020.
//  Copyright © 2020 IDz. All rights reserved.
//
#pragma once
#include <gui/SymbolPopoverView.h>
#include <gui/Image.h>
//
//class SymbolsPopover : public gui::SymbolPopoverView
//{
//    gui::Symbol _pointer;
//    gui::Symbol _rect;
//    gui::Symbol _roundedRect;
//    gui::Symbol _circ;
//public:
//    SymbolsPopover()
//    : SymbolPopoverView(gui::PopoverView::Type::SymbolOnly, 4, 4)
//    , _pointer(":pointer")
//    , _rect(":rect")
//    , _roundedRect(":roundRct")
//    , _circ(":circ")
//    //    , _img(":imgSave")
//    //, _toolbarItemImage(":poly128")
//    {
//        gui::Symbol* nullSymbol = nullptr;
//        addItem(&_pointer, nullSymbol,  tr("pointer"), tr("pointerTT"), false, 0);
//        addItem(&_rect, nullSymbol, tr("rect"), tr("rectTT"), true, 0);
//        addItem(&_roundedRect, nullSymbol, tr("rndRct"), tr("rndRctTT"), true, 0);
//        addItem(&_circ, nullSymbol, tr("circ"), tr("circTT"), true, 0);
//    }
//};

class PopoverPlot : public gui::SymbolPopoverView
{
    gui::Image _graph;
    gui::Image _save;
    gui::Image _grid;
    gui::Image _fullScreen;
    gui::Image _legend;
    gui::Image _reset;
    gui::Image _settings;
    
//    gui::Image _meni;
//    gui::Image _fitToWindow;
//    gui::Image _info;
public:
    PopoverPlot()
    : SymbolPopoverView(gui::PopoverView::Type::SymbolOnly, 6, 6)
    , _graph(":graph")
    , _save(":@PlotSave")
    , _grid(":@PlotGrid")
    , _fullScreen(":@PlotFullScreen")
    , _legend(":@PlotLegend")
    , _reset(":@PlotReset")
    , _settings(":@PlotSettings")
//    , _info(":@PlotInfo")
//    , _fitToWindow(":@PlotFitToWindow")
//    , _meni(":@PlotMeni")
    {
        setButtonImage(&_graph);
        float margin = 6.0f;
        td::String emptyStr;
        addItem(&_fullScreen, emptyStr, tr("@PlotFullScreen"), margin, margin);
        addItem(&_grid, emptyStr, tr("@PlotGrid"), margin, margin);
        addItem(&_legend, emptyStr, tr("@PlotLegend"), margin, margin);
        addItem(&_save, emptyStr, tr("@PlotSave"), margin, margin);
        addItem(&_reset, emptyStr, tr("@PlotReset"), margin, margin);
        addItem(&_settings, emptyStr, tr("@PlotSettings"), margin, margin);
//        addItem(&_fitToWindow, emptyStr, tr("@PlotFitToWindowTT"), margin, margin);
//        addItem(&_meni, emptyStr, tr("@PlotMeniTT"), margin, margin);
//        addItem(&_info, emptyStr, tr("@PlotInfoTT"), margin, margin);
        
        transferLabelToButton(false);
        styleOnParent(false);
        highlightCurrentSelection(false);
    }
};


//
//class PopoverGraphTextMenu : public gui::SymbolPopoverView
//{
//    gui::Image _graph;
//    gui::Image _save;
//    gui::Image _grid;
//    gui::Image _fullScreen;
//    gui::Image _meni;
//    gui::Image _legend;
//    gui::Image _reset;
//    gui::Image _info;
//    gui::Image _fitToWindow;
//    gui::Image _settings;
//public:
//    PopoverGraphTextMenu()
//    : SymbolPopoverView(gui::PopoverView::Type::SymbolAndText, 9, PopoverView::SymbolWidth::AdjustToMaxWHRatio)
//    , _graph(":graph")
//    , _save(":@PlotSave")
//    , _grid(":@PlotGrid")
//    , _fullScreen(":@PlotFullScreen")
//    , _meni(":@PlotMeni")
//    , _legend(":@PlotLegend")
//    , _reset(":@PlotReset")
//    , _info(":@PlotInfo")
//    , _fitToWindow(":@PlotFitToWindow")
//    , _settings(":@PlotSettings")
//    {
//        setButtonImage(&_graph);
//        addItem(&_save, tr("@PlotSave"), tr("@PlotSaveTT"), 2.0f, 2.0f);
//        addItem(&_grid, tr("@PlotGrid"), tr("@PlotGridTT"), 2.0f, 2.0f);
//        addItem(&_fullScreen, tr("@PlotFullScreen"), tr("@PlotFullScreenTT"), 2.0f, 2.0f);
//        addItem(&_meni, tr("@PlotMeni"), tr("@PlotMeniTT"), 2.0f, 2.0f);
//        addItem(&_legend, tr("@PlotLegend"), tr("@PlotLegendTT"), 2.0f, 2.0f);
//        addItem(&_reset, tr("@PlotReset"), tr("@PlotResetTT"), 2.0f, 2.0f);
//        addItem(&_info, tr("@PlotInfo"), tr("@PlotInfoTT"), 2.0f, 2.0f);
//        addItem(&_fitToWindow, tr("@PlotFitToWindow"), tr("@PlotFitToWindowTT"), 2.0f, 2.0f);
//        addItem(&_settings, tr("@PlotSettings"), tr("@PlotSettings"), 2.0f, 2.0f);
//        transferLabelToButton(false);
//        styleOnParent(false);
//        highlightCurrentSelection(false);
//    }
//};
