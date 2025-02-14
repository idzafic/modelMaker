#include <gui/plot/Renderer.h>
#include <gui/Shape.h>
#include <gui/DrawableString.h>
#include <gui/Transformation.h>

namespace gui
{
namespace plot
{

td::String Renderer::to_string(gui::CoordType x) {
    td::String s;
    s.format("%g", x);
    return s;
}

Renderer::Renderer(td::ColorID axisColor, gui::Font *axisFont, gui::Font *legendFont, bool startWithMargins):
    _drawMargins(startWithMargins),
    _marginsZero{0.0, 0.0, 0.0, 0.0},
    _axisColor(axisColor),
    _legenda(axisColor, legendFont, {0,0}),
    _font(axisFont)
{
    _disallowedColors.emplace(td::ColorID::Transparent);
    gui::DrawableString test("1234567890.1234567890e+(1234567890.1234567890)");
    gui::Size sz;
    test.measure(_font, sz);
    test.measure(_font, sz);
    _numberHeight = sz.height;

    if (startWithMargins)
        setUpDrawingWindow();
}

void Renderer::setUpDrawingWindow(){
    auto past = drawingWindow;
    drawingWindow.size = _size;
    drawingWindow.point.y = 0;
    drawingWindow.point.x = 0;

    const auto &margins = _drawNumbersOutside ? _margins : _marginsZero;
    _yAxisNameIsNotNull = (yAxisName.toString().length() > 0) ? true : false;

    if (_drawMargins) {
        drawingWindow.point.y = margins.marginTop;
        drawingWindow.point.x = margins.marginLeft;

        if(_drawNumbersOutside){
            if(true || yAxisName.toString().length() > 0){
                drawingWindow.point.x += _yAxisNameSeperation;
                drawingWindow.size.width -= _yAxisNameSeperation;
            }
            if(true || xAxisName.toString().length() > 0)
                drawingWindow.size.height -= _xAxisNameSeperation;
        }

        drawingWindow.size.width -= margins.marginLeft + margins.marginRight;
        if(!_legendPositionChanged && _drawLegend){
            double constexpr marginLeftLegend = 20;
            drawingWindow.size.width -= _legenda.getWindow().width() + marginLeftLegend;
            _legenda.setOrigin({drawingWindow.point.x + drawingWindow.size.width + marginLeftLegend, _legenda.getWindow().top});
        }
        
        drawingWindow.size.height -= margins.marginTop + margins.marginBottom;

    }

    drawingWindow.size.height = std::max(drawingWindow.size.height, 5.);
    drawingWindow.size.width = std::max(drawingWindow.size.width, 5.);

    ZoomToWindow(past);
    _drawingRect.setOriginAndSize({ drawingWindow.point.x, drawingWindow.point.y }, drawingWindow.size);

}



void Renderer::reset(){
    _pastColors.clear();
    verticals.clear();
    horizontals.clear();
    xAxisName = "";
    yAxisName = "";
    _funkcije.clear();
    delete _Limits;
    _Limits = nullptr;
    _legenda.reset();
    resetGraph();
}



void Renderer::setAxisColor(td::ColorID boja){
    _axisColor = boja;
    _legenda.setTextColor(boja);
    drawAgain();
}

void Renderer::setyAxisName(const td::String &yName)
{
    yAxisName = yName;
    //_margins.marginLeft = 10;
    setUpDrawingWindow();
    drawAgain();
}

void Renderer::setxAxisName(const td::String &xName){
    xAxisName = xName;
    //_margins.marginBottom = 10;
    setUpDrawingWindow();
    drawAgain();
}

void Renderer::setAxisNameDistance(double xNameDistanceFromAxis, double yNameDistanceFromAxis)
{
    _xAxisNameSeperation = xNameDistanceFromAxis;
    _yAxisNameSeperation = yNameDistanceFromAxis;
    setUpDrawingWindow();
    drawAgain();
}

void Renderer::getAxisNameDistance(double &xNameDistanceFromAxis, double &yNameDistanceFromAxis)
{
    xNameDistanceFromAxis = _xAxisNameSeperation;
    yNameDistanceFromAxis = _yAxisNameSeperation;
}

void Renderer::setMargins(double left, double right, double bottom, double top)
{
    _margins.marginTop = top;
    _margins.marginRight = right;
    _margins.marginBottom = bottom;
    _margins.marginLeft = left;
    setUpDrawingWindow();
    drawAgain();
}

void Renderer::getMargins(double &left, double &right, double &bottom, double &top)
{
    top = _margins.marginTop;
    left = _margins.marginLeft;
    right = _margins.marginRight;
    bottom = _margins.marginBottom;
}

void Renderer::showLegend(bool draw)
{
    _drawLegend = draw;
    setUpDrawingWindow();
    drawAgain();
}

void Renderer::addFunction(gui::CoordType *x, gui::CoordType *y, size_t length, td::ColorID color, double lineWidth, Function::Pattern pattern, td::String name)
{
    _funkcije.emplace_back(x, y, length, color, name, lineWidth, pattern);
    finishAddingFunction(_funkcije.back());
}

void Renderer::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, double lineWidth, Function::Pattern pattern, td::String name){
    _funkcije.emplace_back(x, y, length, nextColor(), name, lineWidth, pattern);
    finishAddingFunction(_funkcije.back());
}

void Renderer::addFunction(Function&& fun){
    _funkcije.emplace_back(std::move(fun));
    finishAddingFunction(_funkcije.back());
}

void Renderer::finishAddingFunction(Function& newFun) {
    newFun.setPattern(checkDefaultPattern(newFun.getPattern()));
    _pastColors.push_back(newFun.getColor());
    updateLimits(newFun);
    if (_funkcije.size() == 1) {
        newFun.increaseScaleAndShiftY(-1, 0);
    }
    else {
        gui::CoordType scaleX, scaleY, shiftX, shiftY;
        _funkcije[0].getScale(scaleX, scaleY);
        _funkcije[0].getShift(shiftX, shiftY);
        newFun.increaseScaleAndShiftX(scaleX, shiftX);
        newFun.increaseScaleAndShiftY(scaleY, shiftY);
    }

    _legenda.addFunction(newFun);
}

void Renderer::updateLimits(const Function& newFun){
    size_t length = newFun.getLength();
    if (length == 0)
        return;

    const gui::Point *tacke = newFun.getPoints();

    if (_Limits == nullptr) {
        _Limits = new gui::CoordType[4];
        _Limits[int(limits::xMin)] = tacke[0].x;
        _Limits[int(limits::xMax)] = tacke[0].x;
        _Limits[int(limits::yMin)] = tacke[0].y;
        _Limits[int(limits::yMax)] = tacke[0].y;
    }

    for (size_t i = 0; i < length; ++i) {
        if (_Limits[int(limits::xMax)] < tacke[i].x)
            _Limits[int(limits::xMax)] = tacke[i].x;

        if (_Limits[int(limits::yMax)] < tacke[i].y)
            _Limits[int(limits::yMax)] = tacke[i].y;

        if (_Limits[int(limits::xMin)] > tacke[i].x)
            _Limits[int(limits::xMin)] = tacke[i].x;

        if (_Limits[int(limits::yMin)] > tacke[i].y)
            _Limits[int(limits::yMin)] = tacke[i].y;
    }
}

void Renderer::onSizeChanged()
{
    setUpDrawingWindow();

    if (!_initalDraw)
        fitToWindow();
}

void Renderer::setGraphSize(const gui::Size &sz)
{
    _size = sz;
    onSizeChanged();
}

void Renderer::fitToWindow(){
    if (_Limits == nullptr)
        return;
    if (drawingWindow.size.width < 1 || drawingWindow.size.height < 1)
        return;

    _initalDraw = true;
    gui::CoordType scaleX, scaleY, shiftX, shiftY;
    _funkcije[0].getScale(scaleX, scaleY);
    _funkcije[0].getShift(shiftX, shiftY);

    gui::Geometry g;
    g.point.x = _Limits[int(limits::xMin)] * scaleX + shiftX;
    g.point.y = _Limits[int(limits::yMax)] * scaleY - shiftY;
    g.size.width = (_Limits[int(limits::xMax)] - _Limits[int(limits::xMin)]) * scaleX;
    g.size.height = (_Limits[int(limits::yMin)] - _Limits[int(limits::yMax)]) * scaleY;


    ZoomToWindow(g);
    drawAgain();
}

void Renderer::ZoomToWindow(const gui::Geometry& window){
    if (_funkcije.size() == 0)
        return;
    if (window.size.width < 0.001 || window.size.height < 0.001) {
        return;
    }
    if (drawingWindow.size.width < 1 || drawingWindow.size.height < 1)
        return;

    gui::CoordType shiftY = window.point.y + window.size.height - drawingWindow.point.y - drawingWindow.size.height;
    gui::CoordType shiftX = drawingWindow.point.x - window.point.x;
    
    gui::CoordType scaleX = drawingWindow.size.width / window.size.width;
    gui::CoordType scaleY = drawingWindow.size.height / window.size.height;

    gui::CoordType accumShiftX, accumShiftY;
    _funkcije[0].getShift(accumShiftX, accumShiftY);
    accumShiftX -= drawingWindow.point.x;
    accumShiftY += drawingWindow.point.y + drawingWindow.size.height;

    for (int i = 0; i < _funkcije.size(); ++i){
        _funkcije[i].increaseScaleAndShiftX(scaleX, shiftX * scaleX + accumShiftX * (scaleX-1));
        _funkcije[i].increaseScaleAndShiftY(scaleY, shiftY * scaleY + accumShiftY * (scaleY-1));
    }

}


void Renderer::ZoomToArea(gui::CoordType* minX, gui::CoordType* maxX, gui::CoordType* minY, gui::CoordType* maxY){
    if (_funkcije.size() == 0)
        return;
    if (drawingWindow.size.width < 1 || drawingWindow.size.height < 1)
        return;

    gui::CoordType left, right, top, bottom;

    left = (minX == nullptr) ? drawingWindow.point.x : _funkcije[0].realToTransformedX(*minX);
    right = (maxX == nullptr) ? (drawingWindow.point.x + drawingWindow.size.width) : _funkcije[0].realToTransformedX(*maxX);

    if (left > right)
        std::swap(left, right);

    bottom = (minY == nullptr) ? (drawingWindow.point.y + drawingWindow.size.height) : _funkcije[0].realToTransformedY(*minY);
    top = (maxY == nullptr) ? drawingWindow.point.y : _funkcije[0].realToTransformedY(*maxY);

    if (top > bottom)
        std::swap(top, bottom);

    

    gui::Geometry g(left, top, right-left, bottom-top);
    ZoomToWindow(g);
    drawAgain();

}



void Renderer::draw(){
    //const gui::Rect rect(0, 0, _size.width, _size.height);
    for (int i = 0; i < _funkcije.size(); ++i)
        _funkcije[i].draw(_drawingRect);


    if (_drawMargins) {
        gui::Shape::drawRect(_drawingRect, _axisColor, 2);
    }

    drawAxis();


    if (_funkcije.size() == 0)
        return;

    for (size_t i = 0; i < verticals.size(); ++i) {
        gui::CoordType xVal = _funkcije[0].realToTransformedX(verticals[i]);
        if(xVal >= _drawingRect.left && xVal <= _drawingRect.right)
            gui::Shape::drawLine({ xVal, _drawingRect.bottom }, { xVal, _drawingRect.top }, _axisColor, 1.8);
    }
    for (size_t i = 0; i < horizontals.size(); ++i) {
        gui::CoordType yVal = _funkcije[0].realToTransformedY(horizontals[i]);
        if(yVal <= _drawingRect.bottom && yVal >= _drawingRect.top)
            gui::Shape::drawLine({ _drawingRect.left, yVal }, { _drawingRect.right, yVal }, _axisColor, 1.8);
    }
    

    if (_drawLegend)
        _legenda.draw();

}

Function::Pattern Renderer::checkDefaultPattern(const Function::Pattern &pattern){
    if(pattern.pattern != Pattern::LinePattern::DefaultDot && pattern.pattern != Pattern::LinePattern::DefaultLine)
        return pattern;
    while(_defaultColors.size() > _defaultPatterns.size())
        _defaultPatterns.pop();

    if(_defaultPatterns.empty())
        return (pattern.pattern == Pattern::LinePattern::DefaultLine) ? Pattern(td::LinePattern::Solid) : Pattern(td::DotPattern::X);
    auto p = _defaultPatterns.front();
    _defaultPatterns.pop();

    if(pattern.pattern == Pattern::LinePattern::DefaultDot)
        return Pattern(td::DotPattern(p.second));
    else
        return Pattern(td::LinePattern(p.first));
    

}
void Renderer::changeWidth(double width, size_t function)
{
    if (checkRange(function))
        return;

    _funkcije[function].setLineWidth(width);
    drawAgain();
}

void Renderer::changeName(const td::String& name, size_t function){
    if (checkRange(function))
        return;

    *_funkcije[function].name = name;
    _legenda.changeName(name, function);
    drawAgain();
}

void Renderer::changePattern(Pattern pattern, size_t function){
    if (checkRange(function))
        return;

    _funkcije[function].setPattern(pattern);
    drawAgain();

}

void Renderer::changeColor(td::ColorID color, size_t function){ // todo: modifokovati pastColors da reflektuje promjenu
    if (checkRange(function))
        return;

    _funkcije[function].setColor(color);
    _legenda.changeColor(color, function);
    drawAgain();
}

void Renderer::setLegendLocation(const gui::Point& location)
{
    if(!_legendPositionChanged){
        _legendPositionChanged = true;
        setUpDrawingWindow();
    }
    _legendPositionChanged = true;
    _legenda.setOrigin(location);
    drawAgain();
}

void Renderer::setLegendCols(int cols)
{
    _legenda.changeColumnCnt(cols);
    setUpDrawingWindow();
    drawAgain();
}

void Renderer::removeColorFromAutopicker(td::ColorID color)
{
    _disallowedColors.emplace(color);
}

td::ColorID Renderer::nextColor(){
    td::ColorID boja = td::ColorID::Transparent;
    if (_pastColors.size() == 0) {
        if(_axisColor == td::ColorID::SysText || _axisColor == td::ColorID::SysCtrlBack){
            _disallowedColors.emplace(td::ColorID::White);
            _disallowedColors.emplace(td::ColorID::Black);
        }else{
            _disallowedColors.emplace(td::ColorID::SysText);
            _disallowedColors.emplace(td::ColorID::SysCtrlBack);
        }
        boja = _axisColor;
    }

    while(!_defaultColors.empty())
    {
        auto boja = _defaultColors.front();
        _defaultColors.pop();
        //if(!_disallowedColors.contains(boja))
            return boja;
    }

    if(boja == td::ColorID::Transparent)
    {
        bool repeat;
        int current = int(_pastColors.back());
        int infiniteLoopCheck = current;
        do {
            repeat = false;
            current += 23;
            current = current % 137; //otprilike sve boje su obuhvacene i svaka boja ce se izabrati prije nego sto se pocnu ponavljati
            if(_disallowedColors.contains((td::ColorID)current)){
                repeat = true;
                continue;
            }
            for (td::ColorID boja : _pastColors)
                if (current == int(boja)) {
                    repeat = true;
                    break;
                }
            if (infiniteLoopCheck == current)
                _pastColors.clear();
        } while (repeat);
        boja = td::ColorID(current);
    }

    //pastColors.push_back(boja);
    return boja;
}

void Renderer::drawAxis(){
    if (_funkcije.size() == 0)
        return;


        gui::Point zero(_funkcije[0].realToTransformedX(0), _funkcije[0].realToTransformedY(0)); //drawing x and y axis
        if (_drawingRect.left <= zero.x && zero.x <= _drawingRect.right)
            gui::Shape::drawLine({ zero.x, _drawingRect.bottom }, { zero.x, _drawingRect.top }, _axisColor, 1.8);
        if (_drawingRect.top <= zero.y && zero.y <= _drawingRect.bottom)
            gui::Shape::drawLine({ _drawingRect.left, zero.y }, { _drawingRect.right, zero.y }, _axisColor, 1.8);
   


    gui::CoordType scaleX, scaleY;
    _funkcije[0].getScale(scaleX, scaleY);

    static double gridLineSpaceX = std::log(7000.0 / gui::Display::getDefaultLogicalPixelToMmVRatio());  //200 mm za svaku grid liniju
    static double gridLineSpaceY = std::log(5000.0 / gui::Display::getDefaultLogicalPixelToMmHRatio());

    gui::CoordType len = drawingWindow.size.width / scaleX;
    gui::CoordType razmak = gridLineSpaceX + std::log2(1 / scaleX);
    razmak = std::round(razmak);
    razmak = std::pow(2.0, razmak);
    gui::CoordType startVal = std::ceil(_funkcije[0].transformedToRealX(drawingWindow.point.x) / razmak) * razmak;
    gui::CoordType line = _funkcije[0].realToTransformedX(startVal);



    gui::CoordType razmakY = gridLineSpaceY + std::log2(-1 / scaleY);
    razmakY = std::round(razmakY);
    razmakY = std::pow(2.0, razmakY);
    gui::CoordType startValY = std::ceil(_funkcije[0].TrasformedToRealY(drawingWindow.point.y + drawingWindow.size.height) / razmakY) * razmakY;
    gui::CoordType lineY = _funkcije[0].realToTransformedY(startValY);





   

    gui::CoordType xAxisHeight;
    gui::CoordType yAxisWidth;

    if(_funkcije[0].TrasformedToRealY(_drawingRect.top) < 0)
        xAxisHeight = _drawingRect.top;
    else if(_funkcije[0].TrasformedToRealY(_drawingRect.bottom) > 0)
        xAxisHeight = _drawingRect.bottom;
    else
        xAxisHeight = _funkcije[0].realToTransformedY(0);

    
    if(_funkcije[0].transformedToRealX(_drawingRect.right) < 0)
        yAxisWidth = _drawingRect.right;
    else if(_funkcije[0].transformedToRealX(_drawingRect.left) > 0)
        yAxisWidth = _drawingRect.left;
    else
        yAxisWidth = _funkcije[0].realToTransformedX(0);
    
    gui::Size sz;
    while (line < drawingWindow.point.x + drawingWindow.size.width || lineY >= drawingWindow.point.y){

        constexpr double markLen = 7;
        if (lineY >= drawingWindow.point.y) { // Y osa
            gui::Shape::drawLine({ yAxisWidth - markLen, lineY }, { yAxisWidth + markLen,  lineY }, _axisColor, 2);

            gui::DrawableString broj(to_string(startValY));

            
            broj.measure(_font, sz);
            constexpr double yAxisNumberOffset = 7 + markLen;

            if ((_drawNumbersOutside && yAxisWidth <= _drawingRect.left + yAxisNumberOffset) || (!_drawNumbersOutside && yAxisWidth >= _drawingRect.right - sz.width - yAxisNumberOffset)) {
                broj.draw({ yAxisWidth - yAxisNumberOffset - sz.width,  lineY - _numberHeight / 2 }, _font, _axisColor);
            }
            else
                broj.draw({ yAxisWidth + yAxisNumberOffset,  lineY}, _font, _axisColor);
            
            if (_drawGrid)
                gui::Shape::drawLine({ drawingWindow.point.x, lineY }, { drawingWindow.point.x + drawingWindow.size.width,  lineY }, _axisColor, 1, td::LinePattern::Dash);

        }

        if (line < drawingWindow.point.x + drawingWindow.size.width) { // X osa
            gui::Shape::drawLine({ line, xAxisHeight - markLen }, { line,  xAxisHeight + markLen }, _axisColor, 2);

            gui::DrawableString broj(to_string(startVal));
            broj.measure(_font, sz);
            constexpr double xAxisNumberOffset = 7 + markLen;

            if ((_drawNumbersOutside && xAxisHeight >= _drawingRect.top + xAxisNumberOffset) || ( !_drawNumbersOutside && xAxisHeight <= _drawingRect.top + _numberHeight + xAxisNumberOffset*2))
                broj.draw({ line - sz.width / 2, xAxisHeight + xAxisNumberOffset }, _font, _axisColor);
            else
                broj.draw({ line - sz.width / 2 + 9, xAxisHeight - _numberHeight/2 - xAxisNumberOffset }, _font, _axisColor);

            constexpr double gridLineWidth = 0.9;

            if (_drawGrid)
                gui::Shape::drawLine({ line, _drawingRect.bottom }, { line,  drawingWindow.point.y }, _axisColor, gridLineWidth, td::LinePattern::Dash);

        }



        startVal += razmak;
        startValY += razmakY;
        line += razmak * scaleX;
        lineY += razmakY * scaleY;

    }


    gui::Rect r({ 0, -100}, gui::Size(drawingWindow.size.width, 100 ));
    //axis names
    {
        gui::Transformation tr;

        tr.saveContext();
        tr.translate(_drawingRect.left, _drawingRect.bottom + _xAxisNameSeperation);
        tr.appendToContext();
        this->xAxisName.draw(r, _font, _axisColor, td::TextAlignment::Center, td::VAlignment::Bottom);
        tr.restoreContext();
        if(_yAxisNameIsNotNull){
        tr.saveContext();

        r.setWidth(drawingWindow.size.height);
        tr.identity();

        tr.translate(_drawingRect.left - _yAxisNameSeperation + _numberHeight, _drawingRect.bottom);
        tr.rotateDeg(-90);
        
        tr.appendToContext();
        this->yAxisName.draw(r, _font, _axisColor, td::TextAlignment::Center, td::VAlignment::Bottom);
        tr.restoreContext();
        }
        
    }

}













void Renderer::moveGraph(const gui::CoordType& x, const gui::CoordType& y)
{
    for (int i = 0; i < _funkcije.size(); ++i) {
        _funkcije[i].increaseShiftX(x);
        _funkcije[i].increaseShiftY(y);
    }
    drawAgain();
}








void Renderer::Zoom(const gui::CoordType &scale){
    if (scale < 0.0001)
        return;

  
    gui::CoordType y = drawingWindow.size.height * (1 - 1 / scale) / 2;
    gui::CoordType x = drawingWindow.size.width * (1 - 1 / scale) / 2;

    auto g = drawingWindow;
    g.point.x += x;
    g.point.y += y;
    g.size.height /= scale;
    g.size.width /= scale;

    ZoomToWindow(g);

    drawAgain();
}



Renderer::~Renderer()
{
    delete[] _Limits;
}


} //namepsace plot
} //namespace gui
