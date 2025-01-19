#include "TriangleBlock.h"

TriangleBlock::TriangleBlock()
{
}

void TriangleBlock::setUpBlock()
{
    squareBlock::setUpBlock();
    constexpr float lineWidth = 2;

    if(getInputSwitched()){
         gui::Point points[3] = {{_r.left, (_r.top + _r.bottom)/2}, {_r.right, _r.bottom}, {_r.right, _r.top}};
         _trShape.createPolygon(points, 3, lineWidth);
    }else{
        gui::Point points[3] = {{_r.left, _r.bottom}, {_r.left, _r.top}, {_r.right, (_r.top + _r.bottom)/2}};
        _trShape.createPolygon(points, 3, lineWidth);
    }
}

void TriangleBlock::drawBlock(const td::ColorID &color)
{
    _trShape.drawWire(color);
	drawArrows(color);
}


