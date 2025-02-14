#include <gui/plot/View.h>
#include <gui/Panel.h>
#include <gui/Dialog.h>
#include "Settings.h"
#include <gui/FileDialog.h>
#include "TxtDlg.h"
#include <xml/Writer.h>
#include <fstream>
#include <xml/DOMParser.h>
#include "AnnotationDialog.h"

namespace gui
{
namespace plot
{

const std::initializer_list<gui::InputDevice::Event> View::inputs = { gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks,
gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag, gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::Keyboard, 
gui::InputDevice::Event::CursorEnterLeave, gui::InputDevice::Event::PrimaryDblClick};

const std::initializer_list<gui::InputDevice::Event> View::noInputs = {};

static constexpr double buttonsHeight = 47; // slika je visine 32, a odvaja se od vrha 5 tako da 37 ukupno

View::View(gui::Font *fontAxis, gui::Font *fontLegend, td::ColorID backgroundColor, bool DrawButtons, bool takeUserInput):
    Renderer(_axisColor, fontAxis, fontLegend, true),
    gui::Canvas(takeUserInput ? inputs : noInputs), 
    _backgroundColor(backgroundColor)
{
    enableResizeEvent(true);
    registerForScrollEvents();
   
    drawButtons(DrawButtons);

    for (const char *ime : buttonsImages)
        _slike.emplace_back(ime, gui::Rect({ 0,0 }, gui::Size(32, 32)));
    
    double distance = 20;
    for (size_t i = 0; i < _slike.size(); ++i) {
        _slike[i].rect.setOrigin(distance, 5);
        distance += 32 + _slike[i].rect.width();
    }
    
    _axisColor = td::ColorID::Black;
    if (backgroundColor == td::ColorID::SysCtrlBack)
        setAxisColor(td::ColorID::SysText);
    else if (backgroundColor == td::ColorID::SysText)
        setAxisColor(td::ColorID::SysCtrlBack);
    else if (backgroundColor == _axisColor)
       setAxisColor(td::ColorID::Black);
    else if (backgroundColor == _axisColor)
       setAxisColor(td::ColorID::White);

    setBackgroundColor(_backgroundColor);
    
    removeColorFromAutopicker(_backgroundColor);

    auto props = getAppProperties();
    setMargins(props->getValue<float>("@Plotter_margin0", _margins.marginLeft), props->getValue<float>("@Plotter_margin1", _margins.marginRight),\
    props->getValue<float>("@Plotter_margin3", _margins.marginBottom), props->getValue<float>("@Plotter_margin2", _margins.marginTop));

    setAxisNameDistance(props->getValue<float>("@Plotter_AxisSpace0", _xAxisNameSeperation), props->getValue<float>("@Plotter_AxisSpace1", _yAxisNameSeperation));

    setColorsAndPatternStack();

    _imageSaveSettings._mode = (ImageSaveSettings::ImageSaveStyle)props->getValue<int>(td::String("@Plotter_resolutionIndex"), 0);
    _imageSaveSettings._width = props->getValue<float>(td::String("@Plotter_resolutionWidth"), 500);
    _imageSaveSettings._height = props->getValue<float>(td::String("@Plotter_resolutionHeight"), 500);

}

static inline td::ColorID invertColor(const td::ColorID &color){
    //static_assert(false && "Napisati implementaciju");
    return color; //vratiti invertovanu boju
}

void gui::plot::View::setColorsAndPatternStack()
{

    while(!_defaultColors.empty())
        _defaultColors.pop();
    while(!_defaultPatterns.empty())
        _defaultPatterns.pop();

    td::ColorID color;
    td::String keyVal;
    int ind = 0;
    auto props = getAppProperties();

    int colorMode = props->getValue<int>("@Plot_DefaultColorsMode", 0);
    bool darkMode = getApplication()->isDarkMode();
       
    int tempPat;

    for(int i = 0; i<16; ind+=23, ++i){
        keyVal.format("@Plotter_defaultColor%d", i);
        color = (td::ColorID)props->getValue<int>(keyVal, ind % 137);
        props->setValue<int>(keyVal, (int)color);
        switch (colorMode)
        {
        case 1:
            _defaultColors.emplace(darkMode ? color : invertColor(color));
            break;
        case 2:
            _defaultColors.emplace(darkMode ? invertColor(color): color);
            break;
        default:
            _defaultColors.emplace(color);
            break;
        }

        keyVal.format("@Plotter_defaultPattern%d", i);
        tempPat = props->getValue<int>(keyVal, (int)td::LinePattern::Solid );
        keyVal.format("@Plotter_defaultDotPattern%d", i);

        _defaultPatterns.emplace((td::LinePattern)tempPat, (td::DotPattern)props->getValue<int>(keyVal, (int)td::DotPattern::X ));

    }

}


void View::resetGraph()
{
    setColorsAndPatternStack();
}

void View::onResize(const gui::Size &newSize)
{
    _realSize = newSize;
    static_cast<Renderer *>(this)->setGraphSize({newSize.width, newSize.height - _drawButtons * buttonsHeight});
}

void View::drawButtons(bool draw)
{
    _drawButtons = draw;
    if(_drawButtons){
        _tr.identity();
        _trPoint = {0,buttonsHeight};
        _tr.translate(_trPoint);        
    }else{
        _trPoint = {0,0};
        _tr.identity();
    }
    onResize(_realSize);
    reDraw();
}

void View::onDraw(const gui::Rect &rect)
{

    if (action == Actions::select) 
        gui::Shape::drawRect(_selectRect, SELECT_COLOR, 3, td::LinePattern::Dash);

    if(_drawButtons && _drawMargins)
        for(auto & img : _slike)
            img.image.draw(img.rect);

    _tr.setToContext();

    static_cast<Renderer *>(this)->draw();

    if (action == Actions::pointPeek) {

        td::String broj;
        broj.format("{ %g , %g }", _funkcije[0].transformedToRealX(_lastMousePos.x), _funkcije[0].TrasformedToRealY(_lastMousePos.y - _trPoint.y));
        gui::DrawableString str(broj);
        gui::Size sz;
        str.measure(_font, sz);

        gui::Rect pointRect({_lastMousePos.x, _lastMousePos.y - _trPoint.y}, gui::Size(sz.width + 23, sz.height + 23));
        gui::Shape shape;
        shape.createRoundedRect(pointRect, 17.5);
        shape.drawFill(td::ColorID::LightGray);
        pointRect.setTopAndHeight(pointRect.top + 11.5, 0);
        str.draw(pointRect, _font, td::ColorID::Black, td::TextAlignment::Center, td::VAlignment::Center, td::TextEllipsize::None);
        
    }
    
}


void View::handleAction(View::Action viewAction)
{

    switch (viewAction)
    {
        case Action::MinMarginOnOff:
            
            if (!_drawNumbersOutside)
                _drawNumbersOutside = true;
            else
                _drawNumbersOutside = false;
            
            setUpDrawingWindow();
            reDraw();
            break;
        case Action::GridOnOf:
            showGrid(!_drawGrid);
            break;
        case Action::LegendOnOff:
            showLegend(!_drawLegend);
            break;
        case Action::Export:
            saveMenu();
            break;
        case Action::Reset:
            fitToWindow();
            break;
        case Action::Settings:
            gui::Panel::show<Settings>(this, tr("@Plot_settings"), (td::UINT4)9859, \
            {{gui::Dialog::Button::ID::OK, tr("@Plot_zatvori") , gui::Button::Type::Normal}}, \
            [](gui::Dialog *ptr){}, this);
            break;
        case Action::ShowInfo:
            showInformation();
            break;

        case Action::DeleteInfo:
            verticals.clear();
            horizontals.clear();
            reDraw();
            break;

        case Action::Help:
            showHelp();
            break;
    }
}


void View::onPrimaryButtonPressed(const gui::InputDevice& inputDevice) {
    action = Actions::none;

    gui::Point transformedPoint = inputDevice.getFramePoint();
    transformedPoint.y -= _trPoint.y;

    if(_drawLegend && _legenda.getWindow().contains(transformedPoint)){
        action = Actions::moveLegend;
        return;
    }

    for(int i = 0; i<_slike.size(); ++i){
        if (_slike[i].rect.contains(inputDevice.getFramePoint())){
            handleAction((View::Action)i);
            return;
        }
    }


    if (_drawingRect.contains(inputDevice.getFramePoint())) {
        action = Actions::select;
        _selectRect.setOrigin(inputDevice.getFramePoint());
        _selectRect.setWidth(0);
        _selectRect.setHeight(0);
        return;
    }

}

void View::onPrimaryButtonDblClick(const gui::InputDevice& inputDevice){
    if(!_drawingRect.contains(inputDevice.getFramePoint()))
        return;
    action = Actions::pointPeek;
    _lastMousePos = inputDevice.getFramePoint();
    reDraw();
}

void View::setBackgroundColor(td::ColorID color){
    _backgroundColor = color;
    static_cast<Canvas *>(this)->setBackgroundColor(_backgroundColor);
    reDraw();
}

bool View::onScroll(const gui::InputDevice& inputDevice)
{
    gui::CoordType x = inputDevice.getScrollDelta().x * 20;
    gui::CoordType y = inputDevice.getScrollDelta().y * 20;
    if (inputDevice.getKey().isShiftPressed()) {
        std::swap(x, y);
        x *= -1;
    }

    moveGraph(x,y);
    return true;
}

void View::onPrimaryButtonReleased(const gui::InputDevice& inputDevice){
    if (action == Actions::select) {
        action = Actions::none;
        _selectRect.setOrigin(_selectRect.left, _selectRect.top - _trPoint.y);
        gui::Geometry g({ _selectRect.left, _selectRect.top }, gui::Size(_selectRect.width(), _selectRect.height()));
        if (_selectRect.width() < 0) {
            g.point.x += g.size.width;
            g.size.width *= -1;
        }
        if (_selectRect.height() < 0) {
            g.point.y += g.size.height;
            g.size.height *= -1;
        }

        ZoomToWindow(g);

        reDraw();
    }
    if(action == Actions::moveLegend){
        action = Actions::none;
    }

}


void View::onSecondaryButtonPressed(const gui::InputDevice& inputDevice){
    if (action == Actions::pointPeek)
        action = Actions::none;
    if (action != Actions::none)
        return;

    action = Actions::secondaryClick;
    _lastMousePos = inputDevice.getFramePoint();

}

void View::onSecondaryButtonReleased(const gui::InputDevice& inputDevice){
    if (action == Actions::drag) {
        action = Actions::none;
        return;
    }
    if (action == Actions::secondaryClick) {
        action = Actions::none;
        Zoom(0.5);
        return;
    }

}

void View::onCursorMoved(const gui::InputDevice& inputDevice){
    if (action == Actions::secondaryClick) {
        action = Actions::drag;
        return; // moze i bez returna
    }
    if(action == Actions::drag){
        moveGraph(inputDevice.getFramePoint().x - _lastMousePos.x, _lastMousePos.y - inputDevice.getFramePoint().y );
    }

    _lastMousePos = inputDevice.getFramePoint();

}

void View::onCursorDragged(const gui::InputDevice& inputDevice){
    if (action == Actions::select) {
        _selectRect.setWidth(inputDevice.getFramePoint().x - _selectRect.left);
        _selectRect.setHeight(inputDevice.getFramePoint().y - _selectRect.top);
        reDraw();
    }
    if(action == Actions::moveLegend){
        setLegendLocation({inputDevice.getFramePoint().x, inputDevice.getFramePoint().y - _trPoint.y});
    }
}

bool View::onZoom(const gui::InputDevice& inputDevice){
    auto x = (inputDevice.getScale());
    Zoom((x));
    return true;
}

void View::onCursorExited(const gui::InputDevice& inputDevice) {
    _active = false;

}

void View::onCursorEntered(const gui::InputDevice& inputDevice) {
    _active = true;
}


bool View::onKeyPressed(const gui::Key& key) {
    char c = key.getChar();

    if (c == 'f' || c == 'F') {
        fitToWindow();
        return true;
    }

    if (c == 's' || c == 'S') {
        saveMenu();
        return true;
    }

    if (c == 'l' || c == 'L') {
        showLegend(!_drawLegend);
        return true;
    }

    if (c == '+') {
        Zoom(2);
        return true;
    }

    if (c == '-') {
        Zoom(0.5);
        return true;
    }

    if (c == 'i' || c == 'I') {
        showInformation();
        return true;
    }

    if (c == 'g' || c == 'G') {
        showGrid(!_drawGrid);
        return true;
    }

    if (key.getVirtual() == gui::Key::Virtual::F1) {
        showHelp();
        return true;
    }

    if (key.getVirtual() == gui::Key::Virtual::F5) {
        if (_lastPath.isNull())
            return false;
       

        reset();
        openFile(_lastPath, true);

/*
        auto funCnt = _funkcije.size();

        delete _Limits;
        _Limits = nullptr;
        delete legenda;
        legenda = new legend(_axisColor);
        xAxisName = "";
        yAxisName = "";
        _pastColors.clear();

        

        _funkcije.erase(_funkcije.begin(), _funkcije.begin() + funCnt);
        */
        reDraw();


        return true;
    }


    if (key.getVirtual() == gui::Key::Virtual::F11) {
        _drawMargins = !_drawMargins;
        setUpDrawingWindow();
        reDraw();
        return true;
    }

    if (_funkcije.size() == 0)
        return false;

    if (c == 'v' || c == 'V' || c == 'h' || c == 'H') {
        if (!gui::Rect({ drawingWindow.point.x, drawingWindow.point.y }, gui::Size(drawingWindow.size.width, drawingWindow.size.height)).contains(_lastMousePos))
            return true;
        if (!_active)
            return false;

        if (c == 'v' || c == 'V')
            verticals.emplace_back(_funkcije[0].transformedToRealX(_lastMousePos.x));
        else
            horizontals.emplace_back(_funkcije[0].TrasformedToRealY(_lastMousePos.y - _trPoint.y));

        reDraw();
        return true;
    }


    return false;
}



void View::showHelp(){

    showAlert("Manual", "F11 - toggle fullscreen\nf - fit to window\nv - add vertical annotaion\nh - add horizontal annotation\ni - show annotations"
        "\ng - toggle grid\n'-' - zoom out\n'+' - zoom in\nL - toggle legend"
        "\nRight click - zoom out\nRight drag (mouse) - move plot\nLeft drag (mouse) - zoom to window\nDouble right click - read point\nF5 - reload from file");

    /*
    showAlert("Uputstvo", "F11 - toggle fullscreen\nf - fit to window\nv - dodaj vertikalnu liniju\nh - dodaj horizontalnu liniju\ni - prikazi informacije o vertiklanim i horizontalnim linijama"
        "\ng - toggle grid\n'-' - zoom out\n'+' - zoom in\nL - toggle legend"
        "\nDesni klik - zoom out\nDesni drag (mis) - pomjeranje grafika\nLijevi drag (mis) - povecavanje na zabiljezeni prozor\nDupli desni klik - ocitanje tacke");
        */
}

void View::showInformation(){
      gui::Panel::show<AnnotDiag>(this, "Annotations", (td::UINT4)9858,\
            {{gui::Dialog::Button::ID::OK, tr("@Plot_zatvori") , gui::Button::Type::Normal}}, \
            [](gui::Dialog *ptr){}, this, verticals, horizontals);
    
}

void View::saveMenu()
{
    td::UINT4 dlgID = 999999;
    gui::SaveFileDialog::show(this, "Save plot", \
                              { /*{"@Plot_epsDesc", "*.eps"},*/ 
                              {"@Plot_jpgDesc", "*.jpg"},\
                              {"@Plot_pngDesc", "*.png"}, \
                              {"@Plot_pdfDesc", "*.pdf"}, \
                              /*{"@Plot_svgDesc", "*.svg"},*/\
                              {"@Plot_txtDesc", "*.txt"}, \
                              {"@Plot_xmlDesc", "*.xml"}}, dlgID, [this](gui::FileDialog* pDlg)
    {
        return save(pDlg->getFileName());
    });

}

static td::String toString(const Function::Pattern &pattern)
{
    return pattern.toString();
}

static gui::plot::Renderer::Pattern toLinePattern(const char *pattern){
    return Function::Pattern::toLinePattern(pattern);
}

bool View::saveXML(const td::String& path){
    if (_funkcije.size() == 0)
        return true;

    xml::Writer w;
    w.open(path);
    w.startDocument();

    if (!w.isOk())
        return false;

    w.startElement("Model");

    gui::CoordType scaleX, scaleY, shiftX, shiftY;
    _funkcije[0].getScale(scaleX, scaleY);
    _funkcije[0].getShift(shiftX, shiftY);

    w.attribute("minX", to_string(_funkcije[0].transformedToRealX(drawingWindow.point.x)));
    w.attribute("maxX", to_string(_funkcije[0].transformedToRealX(drawingWindow.point.x + drawingWindow.size.width)));
    w.attribute("maxY", to_string(_funkcije[0].TrasformedToRealY(drawingWindow.point.y)));
    w.attribute("minY", to_string(_funkcije[0].TrasformedToRealY(drawingWindow.point.y + drawingWindow.size.height)));

    w.attribute("xLabel", xAxisName.toString());
    w.attribute("yLabel", yAxisName.toString());

    w.attributeC("background", toString(_backgroundColor));
    w.attributeC("axis", toString(_axisColor));
    

    for (size_t i = 0; i < _funkcije.size(); ++i){
        size_t length = _funkcije[i].getLength();

        w.startElement("function");
        w.attributeC("name", _funkcije[i].name);
        w.attribute("width", to_string(_funkcije[i].getLineWidth()));
        w.attributeC("color", toString(_funkcije[i].getColor()));
        w.attributeC("pattern", toString(_funkcije[i].getPattern()));
        w.attribute("points", to_string(length));
        
        
        w.nodeString("x=[");
        const gui::Point* tacke = _funkcije[i].getPoints();
        --length;


        for (size_t j = 0; j < length; ++j) {
            w.nodeString(to_string(_funkcije[i].transformedToRealX(tacke[j].x)));
            w.nodeString(", ");
        }
        w.nodeString(to_string(_funkcije[i].transformedToRealX(tacke[length].x)));
        w.nodeString("]\ny=[");
        for (size_t j = 0; j < length; ++j) {
            w.nodeString(to_string(_funkcije[i].TrasformedToRealY(tacke[j].y)));
            w.nodeString(", ");
        }
        w.nodeString(to_string(_funkcije[i].TrasformedToRealY(tacke[length].y)));
        w.nodeString("]");
        w.endElement();
       
    }


        for (auto& an : verticals) {
            w.startNode("annotation");
            w.attribute("x", to_string(an));
            w.endNode();
        }
        for (auto& an : horizontals) {
            w.startNode("annotation");
            w.attribute("y", to_string(an));
            w.endNode();
        }


        

    w.endDocument();
    w.close();

    return true;
}

void View::saveTXT(const td::String& path){
    auto d = new TxtDlg(this, 10);
    d->openModal([path, this](gui::Dialog* dDlg) {
            saveTXT(path, ((TxtDlg*)dDlg)->horizontal);
        });
}



bool View::saveTXT(const td::String& path, bool horizontal){
    std::ofstream out;
    out.open(path.c_str());

    if (!out.is_open()) {
        showAlert("Error", "Cant write to file");
        return false;
    }
    
    if (horizontal) {
        for (auto & fun : _funkcije) {
            size_t length = fun.getLength() - 1;
            const gui::Point* tacke = fun.getPoints();

            out << "x=[";
            for (size_t j = 0; j < length; ++j)
                out << to_string(fun.transformedToRealX(tacke[j].x)) << ", ";
            out << to_string(fun.transformedToRealX(tacke[length].x)) << "]\n\n";

            out << fun.name->c_str() << "=[";

            for (size_t j = 0; j < length; ++j)
                out << to_string(fun.TrasformedToRealY(tacke[j].y)) << ", ";
            out << to_string(fun.TrasformedToRealY(tacke[length].y)) << "]\n\n";

        }

    }
    else{

        std::sort(_funkcije.begin(), _funkcije.end(), [](const Function& f1, const Function& f2) {return f1.getLength() > f2.getLength(); });

        size_t length = 0;
        for (auto & fun : _funkcije) {
            out << fun.name->c_str() << "\t\t";
            if (length < fun.getLength())
                length = fun.getLength();
        }
        

        for (size_t i = 0; i < length; ++i){
            out << "\n";
            for (auto & fun : _funkcije) {
                if (fun.getLength() > i) 
                    out << to_string(fun.transformedToRealX(fun.getPoints()[i].x)) << "\t" << to_string(fun.TrasformedToRealY(fun.getPoints()[i].y)) << "\t";
                else 
                    out << "\t\t";
                
            }

        }
           

    }

    return true;
}

bool View::openFile(td::String path, bool readOnlyFunctions){
    bool success = false;

    if (path.endsWith(".txt")) {
        readTXT(path);
        success = true;
    }
    if (path.endsWith(".xml")) {
        readXML(path, readOnlyFunctions);
        success = true;
    }

    if (success) {
        _lastPath = path;
        reDraw();
        return true;
    }

    return false;
}

void View::readTXT(const td::String& path){
    std::ifstream file(path.c_str());

    if (!file.is_open()) {
        showAlert("Error", "Cant open file");
        return;
    }

    std::string line;
    bool containsBracket = false;
    


    while (getline(file, line)) { // pokusava skontati radi li se o horizontalnim podacima ili vertikalnim
        size_t inBracketCharCount = 0;
        size_t outBracketCharCount = 0;
        size_t poz1 = 0;
        if (poz1 = line.find('['), poz1 != std::string::npos) {
            size_t poz2 = line.find(']');
            if (poz2 == std::string::npos) {
                containsBracket = true;
                break;
            }
            inBracketCharCount += poz2 - poz1;
            outBracketCharCount += line.size() - (poz2 - poz1);
            if (inBracketCharCount > outBracketCharCount * 2) {
                containsBracket = true;
                break;
            }
        }
    }
    
    file.clear();
    file.seekg(0, std::ios::beg);

    auto isWhitespace = [](const std::string& line) {
            return line.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
        };

    if (containsBracket) {
        // Handle the "%s[%g, %g, %g, ....]" pattern

        auto readData = [&file, &line, &isWhitespace](std::string& name, std::vector<gui::CoordType>& values) {
            while (getline(file, line)) {
                if (isWhitespace(line))
                    continue;

                
                std::istringstream iss(line);
                char ch;
                while (iss >> ch) {
                    if (ch == '[')
                        break;
                    name += ch;
                }

                if (name.back() == '=') 
                    name.pop_back();
                

                bool done = true;
                do {
                    if (!done)
                        iss.str(line);
                    else
                        done = false;
                       
                    gui::CoordType val;
                    while (iss >> val) {
                        values.emplace_back(val);
                        iss >> ch;
                        if (ch == ']') {
                            done = true;
                            break;
                        }
                    }
                } while (!done && getline(file, line));
                break;
            }

        };

        while (!file.fail() || !file.eof()) {
            std::vector<gui::CoordType> x, y;
            std::string name;
            readData(name, x);
            setxAxisName(name);
            name.clear();
            readData(name, y);
            setyAxisName(name);
            size_t bigger = (x.size() > y.size()) ? y.size() : x.size();
            if (bigger == 0)
                continue;
            this->addFunction(x.data(), y.data(), bigger, 2, Pattern(), name.c_str());
        }

    }
    else {
        // Handle the "%s %s %s....\n%g %g %g %g %g %g\n%g %g %g %g %g %g..." pattern

        auto skipWhiteLine = [&isWhitespace, &line, &file]() {
            while (getline(file, line))
                if (!isWhitespace(line))
                    break;
            return;
            };


        int headerCount = 0;
        skipWhiteLine();
        std::istringstream issHeader(line);
        std::string header;
        std::vector<td::String> names;
        while (issHeader >> header) {
            ++headerCount;
            names.emplace_back(header.c_str());
            header.clear();
        }

        headerCount = headerCount;

        if (headerCount == 0)
            return;

        /*
        char c;
        size_t lines_cnt = 0;
        while (file.get(c)) {
            if (c == '\n')
                ++lines_cnt;
        }
        */

        file.clear();
        file.seekg(0, std::ios::beg);
        skipWhiteLine();
        //skipWhiteLine();
        std::vector<std::vector<gui::Point>> tacke;
   
        try
        {
            tacke.resize(headerCount);
            

            while (getline(file, line)) {
                if (isWhitespace(line))
                    continue;
                std::istringstream iss(line);

                for (size_t i = 0; i < headerCount; ++i) {
                    gui::CoordType val1, val2;
                    if (iss >> val1 >> val2) 
                        tacke[i].emplace_back(val1, val2);
                }

            }

            for (size_t i = 0; i < headerCount; ++i) {
                addFunction(Function(tacke[i].data(), tacke[i].size(), nextColor(), names[i], 2));
            }
        }
    
        catch (...) {
            showAlert("Error", "No memory");
        }
        
    }
    file.close();
   
}

void View::readXML(const td::String& path, bool onlyData){
    xml::FileParser par;
    par.parseFile(path);

    if (!par.isOk()) {
        showAlert("Error", "cant open file");
        return;
    }


    
    auto root = par.getRootNode();

    gui::CoordType minX = 0, maxX = 0, maxY = 0, minY = 0;
    td::ColorID color = td::ColorID::Transparent;
    if (!onlyData) {
        for (auto & att : root->attribs) {

            if (att.getName().cCompareNoCase("xLabel") == 0)
                xAxisName = att.getValue();
            if (att.getName().cCompareNoCase("yLabel") == 0)
                yAxisName = att.getValue();

            if (att.getName().cCompareNoCase("minX") == 0)
                minX = att.value.toDouble();
            if (att.getName().cCompareNoCase("maxX") == 0)
                maxX = att.value.toDouble();
            if (att.getName().cCompareNoCase("minY") == 0)
                maxY = att.value.toDouble();
            if (att.getName().cCompareNoCase("maxY") == 0)
                minY = att.value.toDouble();
            if (att.getName().cCompareNoCase("color") == 0)
                color = td::toColorID(att.getValue().c_str());
            if (att.getName().cCompareNoCase("background") == 0) {
                _backgroundColor = td::toColorID(att.getValue().c_str());
                setBackgroundColor(_backgroundColor);
            }
            if (att.getName().cCompareNoCase("axis") == 0)
                setAxisColor(td::toColorID(att.getValue().c_str()));
        }



    }



    auto funs = root.getChildNode();

    while (funs.isOk()) {
        if (funs->getName().cCompareNoCase("function") == 0) {
            td::String name = "line";
            double width = 2;
            size_t points = 0;
            Pattern pattern;
            for (auto & att : funs->attribs) {
                if (att.getName().cCompareNoCase("name") == 0)
                    name = att.getValue();
                if (att.getName().cCompareNoCase("width") == 0)
                    width = att.value.toDouble();
                if (att.getName().cCompareNoCase("points") == 0)
                    points = att.value.toInt();
                if (att.getName().cCompareNoCase("pattern") == 0)
                    pattern = toLinePattern(att.getValue().c_str());
            }

            

            auto &text = funs->text;

            auto getPoints = [&text, &points](const char* tag, std::vector<gui::CoordType>& vek) {
                int poz = text.find(tag);
                if (poz == -1)
                    return;
                int poz2 = text.find("]", td::UINT4(poz+std::strlen(tag)));
                if (poz2 == -1)
                    return;

                cnt::PushBackVector<td::String> brojevi;
                text.subStr(int(poz + std::strlen(tag)), poz2 - 1).split(",", brojevi, true, true);

                size_t smaller = (points > 0 && points < brojevi.size()) ? points : brojevi.size();

                for (size_t i = 0; i < smaller; ++i) 
                    vek.emplace_back(brojevi[i].toDouble());
                    
                

            };
            
            std::vector<gui::CoordType> x, y;
            getPoints("x=[", x);
            getPoints("y=[", y);

            if (x.size() == 0 || y.size() == 0 || x.size() != y.size())
                continue;

            addFunction(x.data(), y.data(), x.size(), (color == td::ColorID::Transparent) ? nextColor() : color, width, pattern, name);
                
            if (_funkcije.size() == 1 && !onlyData) {
                if (minX == maxX || minY == maxY)
                    fitToWindow();
                else
                    ZoomToArea(&minX, &maxX, &minY, &maxY);
            }

        }

        if (funs->getName().cCompareNoCase("annotation") == 0) {
            auto at = funs.getAttrib("x");
            if (at != nullptr)
                verticals.emplace_back(at->getValue().toDouble());
            at = funs.getAttrib("y");
            if (at != nullptr)
                horizontals.emplace_back(at->getValue().toDouble());
        }

        ++funs;
    }
    reDraw();
}

bool View::save(const td::String& path){

    if (path.endsWith(".txt")) {
        saveTXT(path);
        return true;
    }

    if (path.endsWith(".xml")) {
        saveXML(path);
        return true;
    }

   bool drawButton = false;
   bool success = false;
   std::swap(_drawButtons, drawButton);
   auto oldSize = _size;
   auto newSize = gui::Size(_imageSaveSettings._width, _imageSaveSettings._height);
    if(_imageSaveSettings._mode == ImageSaveSettings::ImageSaveStyle::fixedHeight)
        newSize.width = newSize.height * oldSize.width/oldSize.height; //keeping aspect ratio the same as window
    if(_imageSaveSettings._mode == ImageSaveSettings::ImageSaveStyle::fixedWidth)
        newSize.height = newSize.width * oldSize.height / oldSize.width;


   gui::Rect r(gui::Point(0,0), newSize);
   
    if(_imageSaveSettings._mode != ImageSaveSettings::ImageSaveStyle::windowSize){
        setGraphSize(newSize);
    }
    else{
        setUpDrawingWindow();
        reDraw();
    }
    

    if (path.endsWith(".svg")) {
        if(_imageSaveSettings._mode == ImageSaveSettings::ImageSaveStyle::windowSize)
            exportToSVG(path, false);
        else
            exportToSVG(r, path);
        success = true;
    }

    if (path.endsWith(".pdf")) {
        if(_imageSaveSettings._mode == ImageSaveSettings::ImageSaveStyle::windowSize)
            exportToPDF(path, false);
        else
            exportToPDF(r, path);
        success = true;
    }

    if (path.endsWith(".eps")) {
        if(_imageSaveSettings._mode == ImageSaveSettings::ImageSaveStyle::windowSize)
            exportToEPS(path, false);
        else
            exportToEPS(r, path);
        success = true;
        success = true;
    }


    gui::Image::Type tip = (gui::Image::Type)-100;

    if (path.endsWith(".png"))
        tip = gui::Image::Type::PNG;

    if (path.endsWith(".jpg"))
        tip = gui::Image::Type::JPG;

    if((int)tip != -100){
        gui::Image img;

        if(_imageSaveSettings._mode == ImageSaveSettings::ImageSaveStyle::windowSize)
            this->drawToImage(img, 1, false);
        else
            this->drawToImage(img, r, 1);

         img.saveToFile(path, tip);
         success = true;
    }



    _drawButtons = drawButton;
    setGraphSize(oldSize);
    reDraw();
    

    return success;
}

} //namepsace plot
} //namespace gui
