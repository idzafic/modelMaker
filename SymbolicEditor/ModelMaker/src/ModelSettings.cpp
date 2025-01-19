#include "ModelSettings.h"
#include <gui/GridComposer.h>
#include <regex>
#include "GlobalEvents.h"

using timeType = td::Decimal3;
auto timeTypeTD = td::DataType::decimal3;

using stepType = td::Decimal4;
auto stepTypeTD = td::DataType::decimal4;

static void setUpLineEditSize(gui::NumericEdit& edit, const td::Variant &startValue) {
	edit.setSizeLimits(55, gui::Control::Limit::UseAsMin);
	edit.setValue(startValue);
}


ModelSettings::PlotDesc::PlotDesc(const td::String &name, const td::String &xName):
	title(name),
	xAxis(xName)
{
}


ModelSettings::ModelSettings() :
	_gridLayout(4,7),
	_lblStart(tr("startTime")), _lblEnd(tr("endTime")), _lblStep(tr("stepTime")),
	_lblMaxIter(tr("maxIter")), _lblPreproc(tr("preproc")),
	startTime(timeTypeTD, gui::LineEdit::Messages::Send),
	endTime(timeTypeTD, gui::LineEdit::Messages::Send),
	stepTime(stepTypeTD, gui::LineEdit::Messages::Send),
	maxIter(td::DataType::uint4, gui::LineEdit::Messages::Send),
	preprocesCommands(this),
	_chBoxUseAtoFuncs(tr("useDefaultFuncs"))
{
	_chBoxUseAtoFuncs.setChecked(GlobalEvents::settingsVars.autoPlotFuncs);

	setUpLineEditSize(startTime,  td::Variant(timeType(0.0)));
	setUpLineEditSize(endTime,  td::Variant(timeType(10.0)));
	setUpLineEditSize(stepTime,  td::Variant(stepType(0.01)));
	setUpLineEditSize(maxIter, td::Variant(td::UINT4(500)));



	gui::GridComposer grid(_gridLayout);
	grid.appendRow(_lblStart) << startTime << _lblStep << stepTime << _lblEnd << endTime;
    grid.appendSpacer(10, 6);
	grid.appendRow(_lblMaxIter) << maxIter;
	grid.appendRow(_chBoxUseAtoFuncs);
    grid.appendRow(_lblPreproc, td::HAlignment::Left, td::VAlignment::Top);
    grid.appendCol(preprocesCommands, 0, td::HAlignment::Left, 0);

	setLayout(&_gridLayout);
}


void ModelSettings::showTimes(bool show)
{
	startTime.show(show);
	_lblStart.show(show);
	stepTime.show(show);
	_lblStep.show(show);
	endTime.show(show);
	_lblEnd.show(show);
	
}

void ModelSettings::getDependencies(std::vector<DependencyDesc>& desc)
{
	desc.clear();
	std::regex pattern(R"(import\s+(?:(init)\s+)?([^;\n]+?)(?:\s+as\s+(\w+))?(?:\s+)?(?:[;\n]|$))");
	std::cmatch match;

	const auto& cmnds = preprocesCommands.getText();
	auto start = cmnds.begin();
	while (std::regex_search(start, cmnds.end(), match, pattern)) {
		desc.emplace_back(match[2].first, match[2].length(),  match[3].first, match[3].length(), match[1].matched ? ModelNode::addType::init : ModelNode::addType::combine);
		start = match.suffix().first;
	}

}

inline static void markComplex(td::String& name, bool& isComplex) {
	if (name.endsWith(".real"))
		name.reduceSize(5);
	else if (name.endsWith(".imag")) {
		name.reduceSize(5);
		isComplex = true;
	}
}


void ModelSettings::getFunctions(std::vector<PlotDesc>& desc)
{
	desc.clear();
	std::regex pattern(R"((?:\s|^)(!)?(line|scatter)\s+(?:([^;\n]+?)\s+of\s+)?([^\n;]+?)\s+(?:versus|vs|wrt)\s+([^\s\n;]+?)(?:\s+)?(?:[;\n]|$))");
	std::regex patternYAxis(R"((?:(?:^|,|\s)+)([^,\[\]]+)(?:\s*\[(.+?)\])?)");
	std::regex patternAttrib(R"((?:(?:^|,|\s)+)([^,=\]]+)(?:\s*=\s*([^,\]]+)\s*)?)");
	std::cmatch match, match2, match3;

	const auto& cmnds = preprocesCommands.getText();
	auto start = cmnds.begin();
	while (std::regex_search(start, cmnds.end(), match, pattern)){
		if(match[1].matched){
			start = match.suffix().first;
			continue;;
		}

		desc.resize(desc.size() + 1);
		auto &funs = desc.back();
	
		td::String plotType(match[2].first, match[2].length());
		if(plotType.cCompare("scatter") == 0)
			funs.type = PlotDesc::Type::scatter;
		else
			funs.type = PlotDesc::Type::graph;

		if(match[3].matched)
			funs.title = match[3].first;

		funs.xAxis.fromKnownString(match[5].first, match[5].length());

		markComplex(funs.xAxis, funs.xComplex);



		auto startXAxis = match[4].first;
		while (std::regex_search(startXAxis, match[4].first + match[4].length(), match2, patternYAxis)){
			funs.yAxis.resize(funs.yAxis.size() + 1);
			funs.yAxis.back().name = td::String(match2[1].first, match2[1].length()).trim();
			markComplex(funs.yAxis.back().name, funs.yAxis.back().complex);

			if(match2[2].matched){
				auto startAttrib = match2[2].first;
				while (std::regex_search(startAttrib, match2.suffix().first, match3, patternAttrib)){
					funs.yAxis.back().attribs[td::String(match3[1].first, match3[1].length()).trim()] = td::String(match3[2].first, match3[2].matched ? match3[2].length() : 0).trim();
					//const char *debug = funs.yAxis.back().attribs.begin()->first.c_str();
					startAttrib = match3.suffix().first;
				}
			}

			startXAxis = match2.suffix().first;
		}
		start = match.suffix().first;
	}

}

ModelSettings::SimulationSettings ModelSettings::getSimulationSettings()
{
	td::Variant temp;
	this->startTime.getValue(temp);
	_settings.startTime = temp.dec3Val().getAsFloat();

	this->endTime.getValue(temp);
	_settings.endTime = temp.dec3Val().getAsFloat();

	this->stepTime.getValue(temp);
	_settings.stepTime = temp.dec4Val().getAsFloat();

	this->maxIter.getValue(temp);
	_settings.maxIterations = temp.u4Val();

	_settings.useAutoFuncs = _chBoxUseAtoFuncs.isChecked();

	return _settings;
}

unsigned int ModelSettings::getVersion() const
{
    return version;
}



void ModelSettings::loadFromString(const td::String &settingsString)
{
	if(settingsString.isNull())
		return;
	
	std::regex pattern;
	td::String patternStr, retVal;
	std::cmatch match;

	auto getVal = [&settingsString, &pattern, &match, &patternStr](const td::String& tag, td::String &retVal){
		patternStr.format(R"(%s\(((?:\n|.)+?)\))", tag.c_str());
		pattern = patternStr.c_str();
		if (std::regex_search(settingsString.begin(), settingsString.end(), match, pattern)){
			retVal = td::String(match[1].first, match[1].length());
			return true;
		}
		return false;
	};

	if(getVal("start", retVal))
		startTime.setValue((timeType)retVal.toDouble());

	if(getVal("end", retVal))
		endTime.setValue((timeType)retVal.toDouble());

	if(getVal("step", retVal))
		stepTime.setValue((stepType)retVal.toDouble());

	if(getVal("maxIter", retVal))
		maxIter.setValue((td::UINT4)retVal.toInt());

	if(getVal("autoFuncs", retVal))
		_chBoxUseAtoFuncs.setChecked(retVal.cCompareNoCase("true") == 0);

	if(getVal("preProc", retVal))
		preprocesCommands.setText(retVal);
	

	preprocesCommands.setFontSize(GlobalEvents::settingsVars.textSize);

	++version;

}

const td::String ModelSettings::getString()
{
    cnt::StringBuilder s; //format: start(0)end(10)step(1)...
	s << "start(" << startTime.getValue()\
	<< ")end(" << endTime.getValue()\
	<< ")step(" << stepTime.getValue()\
	<< ")maxIter(" << maxIter.getValue()\
	<< ")autoFuncs(" << _chBoxUseAtoFuncs.isChecked()\
	<< ")preProc(" << preprocesCommands.getText()\
	<< ")";
	td::String str;
	s.getString(str);
	return str;
}

bool ModelSettings::onChangedValue(gui::Slider *pSlider){
	++version;
    return true;
}

bool ModelSettings::onFinishEdit(gui::LineEdit *pCtrl){
	++version;
    return true;
}

bool ModelSettings::onClick(gui::CheckBox *pBtn)
{
	++version;
    return true;
}


ModelSettings::DependencyDesc::DependencyDesc(const char *path, int str1Size, const char *alias, int str2Size, ModelNode::addType type):
	pathOrTabName(path, str1Size),
	alias(alias, str2Size),
	type(type)
{
}

void SyntaxText::highlightText()
{
	auto text = getText();
	const char *start = text.begin();
	static std::cmatch match;
	static std::regex keywordPatten(R"(((?:^|\s)(import|as|!?line|!?scatter|of|versus|vs|wrt|0|init)|(.real|.imag))(?:$|\s))");
	static gui::Range rangeFound;
    removeColor(gui::Range(0, text.glyphLength()));
	while(std::regex_search((const td::UTF8 *)start, (const td::UTF8 *) text.end(), match, keywordPatten)){
		rangeFound.location = match[1].first - text.begin();
		rangeFound.length = match[1].length();
//		setColor(rangeFound, GlobalEvents::settingsVars.colorKeyword);
        setColor(rangeFound, GlobalEvents::settingsVars.colors[(int)ColorType::Keyword]);
		start = match.suffix().first;
	}
}

SyntaxText::SyntaxText(ModelSettings *parent) : gui::TextEdit(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::Send),
                                                _parent(parent)
{
	setFontSize(GlobalEvents::settingsVars.textSize);
}

bool SyntaxText::onKeyPressed(const gui::Key &k)
{
	++_parent->version;
	highlightText();
    return false;
}

void SyntaxText::setText(const td::String &text)
{
	++_parent->version;
	gui::TextEdit::setText(text);
	highlightText();
}
