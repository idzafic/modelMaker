#pragma once
#include <gui/View.h>
#include <gui/Label.h>
#include <gui/ComboBox.h>
#include <gui/GridLayout.h>
#include <gui/GridComposer.h>
#include <mu/IAppProperties.h>
#include <gui/Application.h>
#include <gui/VerticalLayout.h>
#include <gui/CheckBox.h>
#include <gui/StandardTabView.h>
#include <gui/ColorPicker.h>
#include <gui/Application.h>

class SettingsView: public gui::View
{
    gui::ToolBar* _pMainTB = nullptr;
    gui::View _vMain, _vColors;
    gui::VerticalLayout _vl, _tabViewLayout;
    gui::StandardTabView _tabView;
    gui::Image _settingsImg;
    static constexpr int colorsCnt = GlobalEvents::settingsVars.colorCnt;

    gui::Label _colorLabels[colorsCnt];
    gui::ColorPicker _colorPickers[colorsCnt];

    mu::IAppProperties *props;
    gui::Label _lblLang;
    gui::GridLayout layout, _colorsGrid;
    gui::ComboBox langCombo;
    const cnt::SafeFullVector<gui::Lang> langs;
    int currentLangIndex = -1;
    gui::Frame *mainWindow;

    gui::CheckBox _chbToolbarIconsAndLabels;

    gui::CheckBox _chBoxEmbed, _chBoxRestoreTabs, _chBoxConfirmClose, _chBoxDrawFuncsAuto;

    gui::Label _lblFont;
    gui::ComboBox fontCombo;

    bool embededCurrent;

    double width = 270;
    
    bool _restartRequired = false;
    
protected:
    bool onClick(gui::Dialog* pDlg, td::UINT4 dlgID) override
    {
        if(fontCombo.getSelectedIndex() >= 0)
            GlobalEvents::settingsVars.font = fontCombo.getSelectedText();
      

        GlobalEvents::settingsVars.saveValues();
        
        int selectedLang = langCombo.getSelectedIndex();
        if(selectedLang != currentLangIndex){
            props->setKeyValue("Laungage", langs[selectedLang].getExtension());
            _restartRequired = true;
        }

        if(_chBoxEmbed.getValue() != embededCurrent){

            _restartRequired = true;
        }
        
        return true;
    }

public:

    SettingsView(gui::Frame *MainWindow)
    : mainWindow(MainWindow),
        _lblLang(tr("Laungage: ")),
        layout(2,2),
        props(getAppProperties()),
        _vl(6),
        _tabViewLayout(1),
        _colorsGrid(colorsCnt, 2)
    , _chbToolbarIconsAndLabels(tr("chbTBIcsAndLbls"))
    , _chBoxEmbed(tr("embedGraph")),
        _chBoxRestoreTabs(tr("restoreTabs")),
        _chBoxConfirmClose(tr("confirmCloseSetting")),
        embededCurrent(GlobalEvents::settingsVars.embedPlot),
        langs(getSupportedLanguages()),
        _lblFont(tr("fontLabel")),
        _settingsImg(":settings"),
        _chBoxDrawFuncsAuto(tr("drawFunctionsAutomatically"))
    {
        bool showLabels = props->getTBLabelVisibility(mu::IAppProperties::ToolBarType::Main, true);
        _chbToolbarIconsAndLabels.setChecked(showLabels);
        
        auto currentLang = props->getKeyValue("Laungage", "BA");
        auto &settings = GlobalEvents::settingsVars;

        for(int i = 0; i<langs.size(); ++i){
            langCombo.addItem(langs[i].getDescription());
            if(currentLang == langs[i].getExtension()){
                langCombo.selectIndex(langCombo.getNoOfItems() - 1);
                currentLangIndex = i;
            }
        }

        gui::CoordType w = langCombo.getWidthToFitLongestItem();
        langCombo.setSizeLimits(w, gui::Control::Limit::Fixed);

        _chBoxEmbed.setChecked(embededCurrent, true);
        _chBoxEmbed.disable();
        //_chBoxEmbed.onClick([this, &settings](){settings.embedPlot = _chBoxEmbed.isChecked();});

        _chBoxRestoreTabs.setChecked(settings.restoreTabs);
        _chBoxRestoreTabs.onClick([this, &settings](){settings.restoreTabs = _chBoxRestoreTabs.isChecked();});

        _chBoxConfirmClose.setChecked(settings.warnBeforeClose);
        _chBoxConfirmClose.onClick([this, &settings](){settings.warnBeforeClose = _chBoxConfirmClose.isChecked();});
        
        auto foundFonts = gui::Font::getSystemMonospacedFamilyNames();
        fontCombo.addItems(&(foundFonts[0]), foundFonts.size());
        fontCombo.setSizeLimits(fontCombo.getWidthToFitLongestItem(), gui::Control::Limit::Fixed);
        w = std::max(w, fontCombo.getWidthToFitLongestItem());

        for(int i = 0; i<foundFonts.size(); ++i)
            if(foundFonts[i] == settings.font){
                fontCombo.selectIndex(i);
                break;
            }


        width = std::max(width, w);
        
        gui::GridComposer gc(layout);
        gc.appendRow(_lblLang) << langCombo;
        gc.appendRow(_lblFont) << fontCombo;

        _vl << layout << _chBoxDrawFuncsAuto << _chbToolbarIconsAndLabels << _chBoxEmbed << _chBoxRestoreTabs << _chBoxConfirmClose;

        _chBoxDrawFuncsAuto.setChecked(settings.autoPlotFuncs, true);
        _chBoxDrawFuncsAuto.onClick([this, &settings](){settings.autoPlotFuncs = _chBoxDrawFuncsAuto.isChecked();});

        setSizeLimits(width, gui::Control::Limit::Fixed);

        gui::GridComposer cc(_colorsGrid);

        td::ColorID *boje = getApplication()->isDarkMode() ? GlobalEvents::settingsVars.colorsDark : GlobalEvents::settingsVars.colorsLight;
        

        gui::ColorPicker *ptr;
        for(int j = 0; j<colorsCnt; ++j){
            td::String strTitle = GlobalEvents::settingsVars.colorNames[j];
            _colorLabels[j].setTitle(strTitle);
            ptr = _colorPickers + j;
            ptr->setValue(boje[j]);
            cc.appendRow(_colorLabels[j]) << _colorPickers[j];
            ptr->onChangedValue([ptr, j, boje](){
                auto colorID = ptr->getValue();
                boje[j] = colorID;
                mu::dbgLog("j=%d", j);
                GlobalEvents::settingsVars.colors[j] = colorID;
                
                auto theApp = gui::getApplication();
                auto pMainWnd = theApp->getInitialWindow();
                pMainWnd->handleUserEvent((td::UINT4) UserEvent::ReformatTabs);
            });
        }
      


        _vMain.setLayout(&_vl);
        _vColors.setLayout(&_colorsGrid);

        _tabView.addView(&_vMain, tr("appSettings"), &_settingsImg);
        _tabView.addView(&_vColors, tr("colorsSettings"), &_settingsImg);

        _tabViewLayout << _tabView;
        setLayout(&_tabViewLayout);
        
        _chbToolbarIconsAndLabels.onClick([this](){
            if (_pMainTB)
            {
                bool bShowLabelsOnMTB = _chbToolbarIconsAndLabels.isChecked();
                _pMainTB->showLabels(bShowLabelsOnMTB);
            }
            else
            {
                auto app = gui::getApplication();
                auto wnd = app->getInitialWindow();
                auto tb = wnd->getToolBar();
                bool bShowLabelsOnMTB = _chbToolbarIconsAndLabels.isChecked();
                tb->showLabels(bShowLabelsOnMTB);
            }
        });
    }

    ~SettingsView()
    {
        if(_restartRequired)
        {
//            gui::Application* pApp = gui::getApplication();
//            gui::Window* pMainWnd = pApp->getInitialWindow();
            gui::Alert::showYesNoQuestion(tr("RestartRequired"), tr("RestartRequiredInfo"), tr("Restart"), tr("DoNoRestart"), [this] (gui::Alert::Answer answer) {
                if (answer == gui::Alert::Answer::Yes)
                {
                    getApplication()->restart();
                }
            });
        }
    }
    
    void setMainTB(gui::ToolBar* pTB)
    {
        _pMainTB = pTB;
    }

    void measure(gui::CellInfo &c) override{
        gui::View::measure(c);
        c.minHor = 450;
        c.minVer = 290;
    }

};
