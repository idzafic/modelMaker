#include "Property.h"


elementProperty::elementProperty(const td::String& name, td::DataType type, const td::String& tooltip, td::Variant defaultValue) : layout(2), p_name(name) {
	layout << p_name;
	td::Variant v(type);
	numeric = v.isNumeric();
	if (numeric)
		edit = new gui::NumericEdit(type, gui::LineEdit::Messages::Send, true, tooltip);
	else
		edit = new gui::LineEdit(tooltip, gui::LineEdit::Messages::Send);

	if (defaultValue.getType() != td::DataType::TD_NONE)
		setValue(defaultValue, false);


	gui::Size sz;
	edit->getSize(sz);
	edit->setSize(gui::Size(100, sz.height)); // naknadno postavljanje velicine ne radi (dok za label radi)

	layout << *edit;
	setLayout(&layout);

	

}

void elementProperty::setLabelMinSize(int width) {
	gui::Size sz;
	p_name.getSize(sz);
	if (sz.width < width) {
		p_name.setSize(gui::Size(width, sz.height));
	}
	this->reMeasure();
}

td::Variant elementProperty::getValue(){
	td::Variant val;

	if (numeric)
		val = ((gui::NumericEdit*)edit)->getValue();
	else
		((gui::LineEdit*)edit)->getValue(val);
	
	return val;
}

void elementProperty::setValue(const td::Variant &value, bool doAction){

	

	if (numeric)
		((gui::NumericEdit*)edit)->setValue(value, doAction);
	else
		((gui::LineEdit*)edit)->setValue(value, doAction);

}

bool elementProperty::onFinishEdit(gui::LineEdit* pCtrl){
	
	if (this->isHidden())
		return true;

	if (Action != nullptr) {
		td::Variant v;
		pCtrl->getValue(v);
		Action(v);
	}
	else
		return false;

	return true;
}

bool elementProperty::onActivate(gui::LineEdit* pCtrl){
	edit->hide(true, false);
	edit->hide(false, false);
	return true;
}

elementProperty::~elementProperty()
{
	delete edit;
}
