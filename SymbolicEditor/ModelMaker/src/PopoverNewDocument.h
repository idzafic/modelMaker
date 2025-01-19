//
//  Created by Izudin Dzafic on 28/07/2020.
//  Copyright © 2020 IDz. All rights reserved.
//
#pragma once
#include <gui/SymbolPopoverView.h>
#include <gui/Image.h>


class PopoverNewDocument : public gui::SymbolPopoverView
{
    gui::Image _newDocument;
    gui::Image _nl;
    gui::Image _wls;
    gui::Image _nlCmplx;
    gui::Image _wlsCmplx;
    gui::Image _ode;
    gui::Image _dae;
public:
    PopoverNewDocument()
    : SymbolPopoverView(gui::PopoverView::Type::SymbolAndText, 6, PopoverView::SymbolWidth::AdjustToMaxWHRatio)
    , _newDocument(":newFromTemplate")
    , _nl(":newNL")
    , _wls(":newWLS")
    , _nlCmplx(":newNLCmplx")
    , _wlsCmplx(":newWLSCmplx")
    , _ode(":newODE")
    , _dae(":newDAE")
    {
        setButtonImage(&_newDocument);
        addItem(&_nl, tr("nrModel"), tr("nrModelTT"), 2.0f, 2.0f);
        addItem(&_wls, tr("wlsModel"), tr("wlsModelTT"), 2.0f, 2.0f);
        addItem(&_nlCmplx, tr("nrModelCmplx"), tr("nrModelCmplxTT"), 2.0f, 2.0f);
        addItem(&_wlsCmplx, tr("wlsModelCmplx"), tr("wlsModelCmplxTT"), 2.0f, 2.0f);
        addItem(&_ode, tr("odeModel"), tr("odeModelTT"), 2.0f, 2.0f);
        addItem(&_dae, tr("daeModel"), tr("daeModelTT"), 2.0f, 2.0f);
        transferLabelToButton(false);
        styleOnParent(false);
        highlightCurrentSelection(false);
    }
};
