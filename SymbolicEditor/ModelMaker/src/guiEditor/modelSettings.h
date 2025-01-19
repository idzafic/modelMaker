#pragma once
#include "gui/View.h"
#include "../../../common/Property.h"
#include "gui/VerticalLayout.h"
#include "gui/TextEdit.h"
#include "gui/Button.h"

class modelSettings : public gui::View {
	gui::VerticalLayout v;
	gui::Label editLbl;
	gui::Button exportBtn;

public:
	elementProperty name;
	gui::TextEdit edit;
	modelSettings();



};

