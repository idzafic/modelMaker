#pragma once
#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/GridLayout.h>
#include <vector>
#include <gui/Label.h>
#include <gui/NumericEdit.h>
#include "../../EquationToXML/inc/nodes.h"
#include <gui/CheckBox.h>
#include <map>

class ModelSettings;

class SyntaxText: public gui::TextEdit{
	ModelSettings *_parent;
	void highlightText();
public:
	SyntaxText(ModelSettings *parent);
	bool onKeyPressed(const gui::Key &k) override;
	void setText(const td::String &text);

};

class ModelSettings : public gui::View {
	gui::GridLayout _gridLayout;
	gui::Label _lblStart, _lblEnd, _lblStep, _lblMaxIter, _lblPreproc;
	SyntaxText preprocesCommands;
	gui::NumericEdit startTime, endTime, stepTime, maxIter;
	gui::CheckBox _chBoxUseAtoFuncs;

	gui::View paramaterView;
	unsigned int version = 1;
	friend class SyntaxText;

public:
	struct PlotDesc {
		enum class Type{graph, scatter, table} type = Type::graph;
		td::String title, xAxis;
		bool xComplex = false;
		
		struct FunctionDesc{
			bool complex = false;
			td::String name;
			std::map<td::String, td::String> attribs;
			FunctionDesc(const td::String &name = ""): name(name){}
		};
		std::vector<FunctionDesc> yAxis;
		PlotDesc(const td::String &title, const td::String &xName);
		PlotDesc(){};
	};
	struct DependencyDesc {
		td::String pathOrTabName, alias;
		ModelNode::addType type;
		DependencyDesc(const char* path, int str1Size, const char* alias, int str2Size, ModelNode::addType);
	};
	struct SimulationSettings {
		double startTime, endTime, stepTime, maxIterations;
		bool useAutoFuncs;
	} _settings;

public:
	ModelSettings();
	void showTimes(bool show);
	void getDependencies(std::vector<DependencyDesc> &);
	void getFunctions(std::vector<PlotDesc>&);
	SimulationSettings getSimulationSettings();
	unsigned int getVersion() const;

	void loadFromString(const td::String &settingsString);
	const td::String getString();

	virtual bool onChangedValue(gui::Slider* pSlider) override;
	virtual bool onFinishEdit(gui::LineEdit* pCtrl) override;
	virtual bool onClick(gui::CheckBox* pBtn) override;

};
