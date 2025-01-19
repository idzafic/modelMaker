#pragma once
#include "gui/View.h"
#include "gui/HorizontalLayout.h"
#include "gui/LineEdit.h"
#include "gui/Label.h"
#include "gui/NumericEdit.h"
#include "td/Variant.h"
#include "td/String.h"


class elementProperty : public gui::View {
	gui::Label p_name;
	gui::Control* edit;
	bool numeric;
	gui::HorizontalLayout layout;


public:

	elementProperty(const td::String& name, td::DataType type, const td::String& tooltip = "", td::Variant defaultValue = td::Variant(td::DataType::TD_NONE));

	void setLabelMinSize(int width);
	td::Variant getValue();
	void setValue(const td::Variant &value, bool doAction = false);

	bool onFinishEdit(gui::LineEdit* pCtrl) override;
	std::function<void(const td::Variant &)> Action = nullptr;
	bool onActivate(gui::LineEdit* pCtrl) override;

	~elementProperty();

};

