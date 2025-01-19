#pragma once
#include "squareBlock.h"



class CircleBlock: public squareBlock
{
    gui::Shape _circleShape;
public:
	CircleBlock();
    void setUpBlock() override;
	void drawBlock(const td::ColorID &color) override;
};