#pragma once
#include <gui/MenuBar.h>
#include <gui/MenuItem.h>
#include <cnt/Array.h>

#ifdef MU_MACOS
    #define CTRL "<Cmd>"
#else
    #define CTRL "<Ctrl>"
#endif

class MenuBar : public gui::MenuBar {
	gui::SubMenu _appDropDown, _modelDropDown, _plotDropDown;
    cnt::Array<gui::MenuItem*, 4> _txtOptions; //enable/disable options
    cnt::Array<gui::MenuItem*, 1> _graphOptions;
    
	//gui::SubMenu modelText, modelGraphical;
public:
    enum class SubMenuID : td::BYTE {None = 0, App, Model, Plot, NewExampleModel};
    
    enum class ActionID : td::BYTE {None=0, New, OpenTextModel, OpenTFModel, Save, SaveAs, Simulate, Import, Export, Settings, EmptyModel, EmptyModelTF, OpenFromFile, ODE, NR, WLS, NRCmplx, WLSCmplx, DAE, PlotFullScreen, PlotGrid, PlotLegend, PlotSave, PlotReset, PlotSettings};

public:
	MenuBar()
    : gui::MenuBar(3)
    , _appDropDown((td::BYTE) SubMenuID::App, tr("app"), 3)
    , _modelDropDown((td::BYTE) SubMenuID::Model, tr("Model"), 14)
    , _plotDropDown((td::BYTE) SubMenuID::Plot, tr("Plot"), 6)
	{
//        _txtOptions.toZero();
		{
			auto &items = _modelDropDown.getItems();
            int i = 0;
            items[i++].initAsActionItem(tr("emptyModel"), (td::BYTE) ActionID::EmptyModel, CTRL "n");
            items[i++].initAsActionItem(tr("emptyModelTF"), (td::BYTE) ActionID::EmptyModelTF, CTRL "<Alt>n");
            items[i].initAsSubMenu((td::BYTE)SubMenuID::NewExampleModel, tr("NewSimpleModel"), 10);
            {
                auto &exampleItems = items[i].getItems();
                exampleItems[2].initAsActionItem(tr("nrModel"), (td::BYTE) ActionID::NR);
                exampleItems[3].initAsActionItem(tr("wlsModel"), (td::BYTE) ActionID::WLS);
                exampleItems[4].initAsSeparator();
                exampleItems[5].initAsActionItem(tr("nrModelCmplx"), (td::BYTE) ActionID::NRCmplx);
                exampleItems[6].initAsActionItem(tr("wlsModelCmplx"), (td::BYTE) ActionID::WLSCmplx);
                exampleItems[7].initAsSeparator();
                exampleItems[8].initAsActionItem(tr("odeModel"), (td::BYTE) ActionID::ODE);
                exampleItems[9].initAsActionItem(tr("daeModel"), (td::BYTE) ActionID::DAE);
                i++;
            }
            
            items[i++].initAsSeparator();
            items[i++].initAsActionItem(tr("OpenTxtModel"), (td::BYTE) ActionID::OpenTextModel, CTRL "o");
            items[i++].initAsActionItem(tr("OpenTFModel"), (td::BYTE) ActionID::OpenTFModel);
            items[i++].initAsSeparator();
			items[i].initAsActionItem(tr("Save"), (td::BYTE) ActionID::Save,  CTRL "s");
            _txtOptions[0] = &items[i++];
			items[i].initAsActionItem(tr("SaveAs"), (td::BYTE) ActionID::SaveAs, CTRL "<Alt>s");
            _txtOptions[1] = &items[i++];
            items[i++].initAsSeparator();
            items[i++].initAsActionItem(tr("ImportFromXML"), (td::BYTE) ActionID::Import,  CTRL "i");
			items[i].initAsActionItem(tr("ExporttoXML"), (td::BYTE) ActionID::Export, CTRL "e");
            _txtOptions[2] = &items[i++];
            items[i++].initAsSeparator();
            items[i].initAsActionItem(tr("startToolTip"), (td::BYTE) ActionID::Simulate, "<F5>");
            _txtOptions[3] = &items[i++];
		}
		{
			auto &items = _appDropDown.getItems();
			items[0].initAsActionItem(tr("settingsShort"), (td::BYTE) ActionID::Settings, tr("<Ctrl>,").c_str());
            items[1].initAsSeparator();
            items[2].initAsQuitAppActionItem(tr("Quit"));
			
		}
        
        {
            auto &items = _plotDropDown.getItems();
            int i= 0;
            items[i++].initAsActionItem(tr("@PlotFullScreen"), (td::BYTE) ActionID::PlotFullScreen);
            items[i++].initAsActionItem(tr("@PlotGrid"), (td::BYTE) ActionID::PlotGrid);
            items[i++].initAsActionItem(tr("@PlotLegend"), (td::BYTE) ActionID::PlotLegend);
            items[i++].initAsActionItem(tr("@PlotSave"), (td::BYTE) ActionID::PlotSave);
            items[i++].initAsActionItem(tr("@PlotReset"), (td::BYTE) ActionID::PlotReset);
            items[i++].initAsActionItem(tr("@PlotSettings"), (td::BYTE) ActionID::PlotSettings);
        }
        
        _menus[0] = &_appDropDown;
		_menus[1] = &_modelDropDown;
        _menus[2] = &_plotDropDown;
	}

	void enablePlotMenu(bool bEnable)
    {
        auto& items = _plotDropDown.getItems();
        for (auto& m : items)
            m.enable(bEnable);
	}
    
    void enableTxtOptions(bool bEnable)
    {
        for (auto mi : _txtOptions)
            mi->enable(bEnable);
    }
};
