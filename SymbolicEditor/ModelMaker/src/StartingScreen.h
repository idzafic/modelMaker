#include <gui/Canvas.h>
#include <gui/Image.h>

class StartingScreen: public gui::Canvas{
    gui::Image _imgLogo;
    gui::Rect _drawingRect;
    constexpr static double _imageSize = 128;

public:
    StartingScreen():
        _imgLogo(":logo")
    {
        this->enableResizeEvent(true);
        _drawingRect.setSize(_imageSize, _imageSize);
    }


    void onResize(const gui::Size& newSize){
        _drawingRect.setOrigin( newSize.width/2 - _imageSize/2, newSize.height/2 - _imageSize/2);
    }

    void onDraw(const gui::Rect& rect) override
    {
        _imgLogo.draw(_drawingRect, gui::Image::AspectRatio::No, td::HAlignment::Center, td::VAlignment::Center);
    }
 

};