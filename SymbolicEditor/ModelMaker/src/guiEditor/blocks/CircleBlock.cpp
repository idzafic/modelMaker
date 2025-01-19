#include "CircleBlock.h"

CircleBlock::CircleBlock()
{
}

void CircleBlock::setUpBlock()
{
    constexpr gui::CoordType radius = 50;
    _r.right = _r.left + radius*2;

    squareBlock::setUpBlock();

    constexpr float lineWidth = 2;
    _circleShape.createCircle(gui::Circle(_r.center(), radius), lineWidth);
}

void CircleBlock::drawBlock(const td::ColorID &color)
{
    _circleShape.drawWire(color);
    squareBlock::drawArrows(color);
}
