#include <gui/View.h>
#include <gui/GridLayout.h>
#include <gui/ComboBox.h>
#include <gui/GridComposer.h>
#include <gui/Slider.h>
#include <gui/Label.h>
#include <gui/plot/View.h>
#include <gui/StandardTabView.h>
#include <gui/ViewScroller.h>
#include <gui/ColorPicker.h>
#include <gui/ComboBox.h>
#include <gui/NumericEdit.h>
#include <gui/DotPatternPicker.h>
#include <gui/LinePatternPicker.h>

namespace gui
{
namespace plot
{


class Settings : public gui::StandardTabView{
    gui::View _generalView, _helperView, _helperView2;
    gui::VerticalLayout _vlHelper;
    gui::ViewScroller _colorsView;

    gui::GridLayout _grid;
    gui::Label _legendColsLabel;
    gui::ComboBox _comboCols;

    gui::Label _marginLeftLbl, _marginRightLbl, _marginTopLbl, _marginBottomLbl;
    gui::Slider _marginLeftSlider, _marginRightSlider, _marginTopSlider, _marginBottomSlider;

    double _margins[4];

    gui::Label _xSpaceLbl, _ySpaceLbl;
    gui::Slider _xSpaceSlider, _ySpaceSlider;

    gui::NumericEdit _numWidth, _numHeight;

    double _axisSpace[2];

    gui::Label _lblOutputRes;
    gui::ComboBox _combBoxSaveResolution;
    gui::Label _lblHeight, _lblWidth;

    static constexpr int _colorCnt = 16;
    gui::ColorPicker _colorPickers[_colorCnt];
    gui::Label _labelsColor[_colorCnt];

    gui::Label _labelColorDefinition;
    gui::ComboBox _combColorMode;
    gui::LinePatternPicker _pattLines[_colorCnt];
    gui::DotPatternPicker _pattDots[_colorCnt];

    gui::GridLayout _gridColors;

    void measure(gui::CellInfo &c) override{
        gui::StandardTabView::measure(c);
        //c.minHor = 200;
        //c.minVer = 350;
    }

public:
    Settings(gui::plot::View *parent):
    _grid(12,2),
    _legendColsLabel(tr("@Plot_legendCols")),
    _marginLeftLbl(tr("@Plot_marginLeft")),
    _marginRightLbl(tr("@Plot_marginRight")),
    _marginTopLbl(tr("@Plot_marginTop")),
    _marginBottomLbl(tr("@Plot_marginBottom")),

    _xSpaceLbl(tr("@Plot_xSpace")),
    _ySpaceLbl(tr("@Plot_ySpace")),

    _lblOutputRes(tr("@Plot_savedImageResolution")),
    _lblHeight(tr("@Plot_height")),
    _lblWidth(tr("@Plot_width")),
    _labelColorDefinition(tr("@Plot_colorDef")),
    _numHeight(td::int4, gui::LineEdit::Messages::Send),
    _numWidth(td::int4, gui::LineEdit::Messages::Send),

    _colorsView(gui::ViewScroller::Type::NoScroll, gui::ViewScroller::Type::ScrollerAlwaysVisible),
    _gridColors(_colorCnt + 1, 4),
    _vlHelper(1)
    {
        gui::GridComposer gc(_grid);
        _comboCols.addItem("1");
        _comboCols.addItem("2");
        _comboCols.addItem("3");
        _comboCols.addItem("4");

        _comboCols.onChangedSelection([parent, this](){parent->setLegendCols(_comboCols.getSelectedIndex()+1);});

        gc.appendRow(_legendColsLabel) << _comboCols;

        parent->getMargins(_margins[0], _margins[1], _margins[3], _margins[2]);


        _marginLeftSlider.setRange(0, 200);
        _marginRightSlider.setRange(0, 200);
        _marginTopSlider.setRange(0, 200);
        _marginBottomSlider.setRange(0, 200);

        _marginLeftSlider.setValue(_margins[0], false);
        _marginRightSlider.setValue(_margins[1], false);
        _marginTopSlider.setValue(_margins[2], false);
        _marginBottomSlider.setValue(_margins[3], false);


        const auto getMarginUpdater = [this, parent](gui::Slider* slider, double *margin){
            td::String keyVal;
            keyVal.format("@Plotter_margin%d", margin-_margins);
            return [slider, margin, this, parent, keyVal](){
                td::Variant v; 
                slider->getValue(v); 
                *margin = v.r8Val(); 
                parent->setMargins(_margins[0], _margins[1], _margins[3], _margins[2]);
                getAppProperties()->setValue<float>(keyVal, *margin);
            };
        };

        _marginLeftSlider.onChangedValue(getMarginUpdater(&_marginLeftSlider, _margins));
        _marginRightSlider.onChangedValue(getMarginUpdater(&_marginRightSlider, _margins+1));
        _marginTopSlider.onChangedValue(getMarginUpdater(&_marginTopSlider, _margins+2));
        _marginBottomSlider.onChangedValue(getMarginUpdater(&_marginBottomSlider, _margins+3));

        gc.appendRow(_marginLeftLbl) << _marginLeftSlider;
        gc.appendRow(_marginRightLbl) << _marginRightSlider;
        gc.appendRow(_marginTopLbl) << _marginTopSlider;
        gc.appendRow(_marginBottomLbl) << _marginBottomSlider;

        gc.startNewRowWithSpace(0, 10);

        parent->getAxisNameDistance(_axisSpace[0], _axisSpace[1]);

        _xSpaceSlider.setRange(20, 200);
        _ySpaceSlider.setRange(20, 200);

        _xSpaceSlider.setValue(_axisSpace[0], false);
        _ySpaceSlider.setValue(_axisSpace[1], false);

        const auto getSpaceSliderUpdater = [this, parent](gui::Slider* slider, double *space){
            td::String keyVal;
            keyVal.format("@Plotter_AxisSpace%d", space - _axisSpace);
            return [slider, space, this, parent, keyVal](){
                td::Variant v; 
                slider->getValue(v); 
                *space = v.r8Val(); 
                parent->setAxisNameDistance(_axisSpace[0], _axisSpace[1]);
                getAppProperties()->setValue<float>(keyVal, *space);
            };
        };

        _xSpaceSlider.onChangedValue(getSpaceSliderUpdater(&_xSpaceSlider, _axisSpace));
        _ySpaceSlider.onChangedValue(getSpaceSliderUpdater(&_ySpaceSlider, _axisSpace + 1));

        gc.appendRow(_xSpaceLbl) << _xSpaceSlider;
        gc.appendRow(_ySpaceLbl) << _ySpaceSlider;

        td::String items[] = {tr("@Plot_WindowSizeRes"), tr("@Plot_FixedWidthRes"), tr("@Plot_FixedHeightRes"), tr("@Plot_FixedRes")};
        _combBoxSaveResolution.addItems(items, sizeof(items) / sizeof(td::String));

        _combBoxSaveResolution.onChangedSelection([this, parent](){
            int ind = _combBoxSaveResolution.getSelectedIndex();
            switch (ind)
            {
            case 0:
                _numWidth.disable(true);
                _numHeight.disable(true);
                break;
            case 1:
                _numWidth.disable(false);
                _numHeight.disable(true);
                break;
            case 2:
                _numWidth.disable(true);
                _numHeight.disable(false);
                break;
            case 3:
                _numWidth.disable(false);
                _numHeight.disable(false);
                break;
            }
            getAppProperties()->setValue<int>("@Plotter_resolutionIndex", ind);
            parent->_imageSaveSettings._mode = (View::ImageSaveSettings::ImageSaveStyle) ind;
        });

        _combBoxSaveResolution.selectIndex((int)parent->_imageSaveSettings._mode);

        gc.startNewRowWithSpace(0, 10);

        gc.appendRow(_lblOutputRes) << _combBoxSaveResolution;
        gc.appendRow(_lblWidth) << _numWidth;
        gc.appendRow(_lblHeight) << _numHeight;

        _numWidth.setValue((int)parent->_imageSaveSettings._width);
        _numHeight.setValue((int)parent->_imageSaveSettings._height);

        _numWidth.onFinishEdit([this, parent](){
            td::Variant val = _numWidth.getValue();
            parent->_imageSaveSettings._width = val.i4Val();
            getAppProperties()->setValue<float>("@Plotter_resolutionWidth", val.i4Val());
        });


        _numHeight.onFinishEdit([this, parent](){
            td::Variant val = _numHeight.getValue();
            parent->_imageSaveSettings._height = val.i4Val();
            getAppProperties()->setValue<float>("@Plotter_resolutionHeight", val.i4Val());
        });


        _generalView.setLayout(&_grid);

        addView(&_generalView, "General");

        //colors:
        auto props = getAppProperties();
        td::String lbl;
        gui::GridComposer colorComp(_gridColors);

        colorComp.appendRow(_labelColorDefinition);

        _combColorMode.addItems(std::vector<td::String>{tr("@Plot_allMode"), tr("@Plot_darkMode"), tr("@Plot_whiteMode")});
        colorComp.appendCol(_combColorMode, -1);
        _combColorMode.selectIndex(props->getValue<int>("@Plot_DefaultColorsMode", 0));

        _combColorMode.onChangedSelection([this](){
             getAppProperties()->setValue<int>("@Plot_DefaultColorsMode", _combColorMode.getSelectedIndex());
        });


        for(int i = 0; i<_colorCnt; ++i){
            lbl.format("Properties for line %d:", i+1);
            _labelsColor[i].setTitle(lbl);
            colorComp.appendRow(_labelsColor[i]) << _colorPickers[i] << _pattLines[i] << _pattDots[i];

            lbl.format("@Plotter_defaultColor%d", i);
            _colorPickers[i].setValue((td::ColorID)props->getValue<int>(lbl, 0));
            _colorPickers[i].onChangedValue([this, i, lbl](){
                getAppProperties()->setValue<int>(lbl , (int)_colorPickers[i].getValue());
            });

            lbl.format("@Plotter_defaultPattern%d", i);
            _pattLines[i].setValue((td::LinePattern)props->getValue<int>(lbl, 0));
            _pattLines[i].onChangedValue([this, i, lbl](){
                getAppProperties()->setValue<int>(lbl , (int)_pattLines[i].getValue());
            });

            lbl.format("@Plotter_defaultDotPattern%d", i);
            _pattDots[i].setValue((td::DotPattern)props->getValue<int>(lbl, 0));
            _pattDots[i].onChangedValue([this, i, lbl](){
                getAppProperties()->setValue<int>(lbl , (int)_pattDots[i].getValue());
            });

        }

        _helperView.setLayout(&_gridColors);
        _colorsView.setContentView(&_helperView);

        _vlHelper << _colorsView;
        _helperView2.setLayout(&_vlHelper);

        addView(&_helperView2, tr("@Plot_defLineProps"));
        gui::Size sz = {300,100};
        _colorsView.setSize(sz);

        
    }



};

} //namepsace plot
} //namespace gui
