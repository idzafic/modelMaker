#pragma once
#include "squareBlock.h"


class TriangleBlock: public squareBlock
{
    gui::Shape _trShape;
public:
	TriangleBlock();
    void setUpBlock() override;
	void drawBlock(const td::ColorID &color) override;
};