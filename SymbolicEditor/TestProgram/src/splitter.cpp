#include "splitter.h"

#include "gui/VerticalLayout.h"
#include "gui/ComboBox.h"
#include "gui/Slider.h"
#include "gui/ColorPicker.h"
#include "gui/ViewSwitcher.h"
#include "./../../common/Property.h"
#include "./../../Graph/inc/Graph.h"


const std::vector<td::String> patternNames = { "Solid", "Dash", "Dot", "DashDot", "DashEq", "Data points" };

class switcher : public gui::ViewSwitcher {
	gui::VerticalLayout graphProps;
	gui::VerticalLayout lineProps;
	gui::View view1, view2;
	Graph& mainGraph;
	size_t index = 0;

	elementProperty name;
	gui::Slider slider; gui::Label sliderLabel;
	gui::ColorPicker line_color;
	gui::ComboBox pattern;


	gui::ColorPicker backgroundColor; gui::Label backgroundLabel;
	gui::ColorPicker axisColor; gui::Label axisLabel;

	elementProperty xAxisName, yAxisName;

	
public:
	switcher(Graph& mainView) : gui::ViewSwitcher(2), graphProps(8), lineProps(6), mainGraph(mainView),
		name("name: ", td::string8, "name of the function that appears on the legend"),
		sliderLabel("Line width:"),
		axisLabel("Axis color:"), backgroundLabel("Background Color:"),
		xAxisName("x axis label:", td::string8, "label of the x axis"),
		yAxisName("y axis label:", td::string8, "label of the y axis")
	{
		slider.setRange(0.1, 10);
		pattern.addItems(patternNames.data(), patternNames.size());
		size_t& index = this->index;
		name.Action = [&mainView, &index](const td::Variant& v) {
			mainView.changeName(v.strVal(), index);
		};

		lineProps << name << sliderLabel << slider << line_color;
		lineProps.appendSpace(10);
		lineProps << pattern;


		graphProps << backgroundLabel << backgroundColor;
		graphProps.appendSpace(10);
		graphProps << axisLabel << axisColor;

		xAxisName.Action = [&mainView](const td::Variant& v) {mainView.setxAxisName(v.strVal()); };
		yAxisName.Action = [&mainView](const td::Variant& v) {mainView.setyAxisName(v.strVal()); };

		graphProps.appendSpace(10);
		graphProps << xAxisName << yAxisName;

		view1.setLayout(&lineProps);
		view1.forwardMessagesTo(this);
		view2.setLayout(&graphProps);
		view2.forwardMessagesTo(this);
		addView(&view2, false);
		addView(&view1, false);
	}

	bool onChangedValue(gui::Slider* pSlider) override;
	bool onChangedSelection(gui::ComboBox* pCmb) override;
	bool onChangedValue(gui::ColorPicker* pCP);
	void showFunction(size_t pos);
	void showGraph();

	virtual ~switcher() {

	}

};



bool switcher::onChangedValue(gui::Slider* pSlider){
	mainGraph.changeWidth(pSlider->getValue(), index);
	return true;
}

bool switcher::onChangedSelection(gui::ComboBox* pCmb){
	if (pCmb != &pattern)
		return false;

	mainGraph.changePattern(td::LinePattern(pCmb->getSelectedIndex()), index);
	return true;
}

bool switcher::onChangedValue(gui::ColorPicker* pCP){

	if (pCP == &line_color) {
		mainGraph.changeColor((td::ColorID)line_color.getCurrentSelection(), index);
	}
	if (pCP == &axisColor) {
		mainGraph.setAxisColor((td::ColorID)axisColor.getCurrentSelection());
	}
	if (pCP == &backgroundColor) {
		mainGraph.setBackgroundColor((td::ColorID)backgroundColor.getCurrentSelection());
		mainGraph.setAxisColor((td::ColorID)axisColor.getCurrentSelection());
	}

	return true;
}

void switcher::showFunction(size_t pos){
	auto& funs = mainGraph.getFunctions();
	if (pos >= funs.size())
		return;
	index = pos;
	
	td::Variant var(funs[pos].name->c_str());
	name.setValue(var, false);

	slider.setValue(funs[pos].getLineWidth(), false);
	var = funs[pos].getColor();
	line_color.setValue(var);
	pattern.selectIndex(int(funs[pos].getPattern()));
	showView(1, false);
}

void switcher::showGraph(){
	backgroundColor.setValue(mainGraph.getBackgroundColor(), false);
	axisColor.setValue(mainGraph.getAxisColor(), false);
	xAxisName.setValue(td::Variant(mainGraph.getxAxisName()));
	yAxisName.setValue(td::Variant(mainGraph.getyAxisName()));
	showView(0, false);
}





splitterLayout::splitterLayout(Graph& mainView) : _mainLayout(gui::SplitterLayout::Orientation::Horizontal), v(4), _graph(mainView){
	props = new switcher(mainView);
	picker.forwardMessagesTo(this);
	picker.addItem("Graph");

	v << picker;
	v.appendSpace(10);
	v << *props;
	v.appendSpacer();
	
	_settingsView.setLayout(&v);

	_mainLayout.setContent(mainView, _settingsView);

	setLayout(&_mainLayout);
}

void splitterLayout::refreshPicks(){
	ignoreSelections = true;
	int curent = picker.getSelectedIndex();
	props->showGraph();
	picker.selectIndex(0);
	int len = picker.getNoOfItems();
	for (int i = 1; i < len; ++i)
		picker.removeItem(1);
	auto &funs = _graph.getFunctions();
	for (const Function& fun : funs) {
		picker.addItem(fun.name->c_str());
	}
	
	if (picker.getNoOfItems() > curent && curent != -1)
		picker.selectIndex(curent);

	ignoreSelections = false;
}

bool splitterLayout::onChangedSelection(gui::ComboBox* pCmb){
	
	if (pCmb != &picker)
		return false;


	if (pCmb->getSelectedIndex() == 0) {
		props->showGraph();
		return true;
	}


	props->showFunction(pCmb->getSelectedIndex() - 1);

	if(!ignoreSelections)
		refreshPicks();

	return true;
}

splitterLayout::~splitterLayout()
{
	delete props;
}
