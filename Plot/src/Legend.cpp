#include <gui/plot/Legend.h>

namespace gui
{
namespace plot
{

void Legend::changeLocation(const gui::Point &location)
{
    _window.setOrigin(location);
}

Legend::Legend(td::ColorID textColor, gui::Font *font, const gui::Point &location) :
textColor(textColor),
_font(font)
{
    changeLocation(location);
    changeColumnCnt(1);
}

int Legend::getPerColumn(){
    return 0.5 + colors.size() / double(_columns);
}

void Legend::draw()
{
    int perColumn = getPerColumn();
    gui::CoordType height = _window.top, length = _window.left;
    
    int cnt = 0, column = 0;
    for (size_t i = 0; i < colors.size(); ++i)
    {
        imena[i].draw({ length + rectSize + offsetToName, height}, _font, textColor);
        gui::Shape::drawRect(gui::Rect({ length, height }, gui::Size(rectSize, rectSize)), colors[i]);
        height += rectSize + offsetHeight;
        
        if(++cnt == perColumn)
        {
            cnt = 0;
            length += _lengths[column++] + offsetToName + rectSize + offsetColumn;
            height = _window.top;
        }
    }
}

void Legend::changeName(const td::String& name, size_t poz)
{
    imena[poz] = name;
    gui::Size sz;
    imena[poz].measure(_font, sz);
    
    size_t perColumn = (size_t) getPerColumn();
    size_t column = poz / perColumn;
    
    
    if(_lengths.size() < column + 1)
        _lengths.resize(column + 1);
    
    if(_lengths[column] < sz.width){
        _window.setWidth(_window.width() - _lengths[column] + sz.width);
        _lengths[column] = sz.width;
    }
    
}

void Legend::changeColor(td::ColorID color, size_t poz) {
    colors[poz] = color;
}

void Legend::changeColumnCnt(size_t columnCnt){
    _columns = columnCnt;
    _columns = (_columns > imena.size()) ? imena.size() : _columns;
    _columns = (_columns < 1) ? 1 : _columns;
    constexpr double widthSpace = rectSize + offsetToName;
    _lengths.resize(_columns);
    
    if(imena.size() == 0){
        _window.setSize({0,0});
        return;
    }
    
    double width = widthSpace * _columns + offsetColumn * (_columns - 1);
    gui::Size sz;
    const int perColumn = getPerColumn();
    int cnt = 0;
    int column = 0;
    double biggestSize = 0;
    for(int i = 0; i<imena.size(); ++i){
        imena[i].measure(_font, sz);
        biggestSize = std::max(biggestSize, sz.width);
        if(perColumn == ++cnt){
            _lengths[column] = biggestSize;
            width += biggestSize;
            biggestSize = 0;
            cnt = 0;
            ++column;
        }
    }
    
    _window.setSize({width, perColumn * (rectSize + offsetHeight)});
    
}

void Legend::addFunction(const Function& f) {
    imena.emplace_back(*f.name);
    colors.emplace_back(f.getColor());
    changeColumnCnt(_columns);
};

const gui::Rect& Legend::getWindow() {
    return _window;
}

void Legend::setOrigin(const gui::Point &location){
    _window.setOrigin(location);
}

void Legend::setTextColor(const td::ColorID &color){
    textColor = color;
}

void Legend::reset(){
    imena.clear();
    colors.clear();
    changeColumnCnt(_columns);
}

} //namepsace plot
} //namespace gui
