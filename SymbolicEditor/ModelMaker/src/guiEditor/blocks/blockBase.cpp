#include "blockBase.h"
#include "../canvas.h"
#include "../../GlobalEvents.h"
#include <gui/DrawableString.h>

const gui::Rect& BlockBase::getRect() const{
	return _r;
}

const bool BlockBase::getInputSwitched() const{
	return switchOutput;
}


const gui::Point BlockBase::getLocation() const{
	return gui::Point(_r.left, _r.top);
}

const std::vector<std::set<std::pair<BlockBase*, int>>>& BlockBase::getConnectedToBlocks() const
{
	return connectedTo;
}

const std::vector<std::pair<BlockBase*, int>>& BlockBase::getConnectedFromBlocks() const
{
	return connectedFrom;
}

bool BlockBase::getIsConnectedFrom() const
{
	for (const auto& par : connectedFrom)
		if (par.first != nullptr)
			return true;

	return false;
}

bool BlockBase::getIsConnectedTo() const
{
	for (const auto& set : connectedTo)
		if (!set.empty())
			return true;
	return false;
}

void BlockBase::setUpAll(bool ignoreRelatedBlocks)
{
	if (disableSetUp)
		return;

	setUpBlock();

	if (!ignoreRelatedBlocks)
		for (const auto& var : connectedFrom)
			if (var.first != nullptr)
				var.first->setUpWires(false);


	setUpWires(true); // will refresh canvas
}




bool BlockBase::intersectsBlock(const gui::Point& p) {
	return _r.contains(p);
}

int BlockBase::intersectsInput(const gui::Point& p) const
{
	for (int i = 0; i < getInputCnt(); ++i)
		if (gui::Circle(getInput(i), 20).contains(p))
			return i;

	return -1;
}

int BlockBase::intersectsOutput(const gui::Point& p) const
{
	for (int i = 0; i < getOutputCnt(); ++i)
		if (gui::Circle(getOutput(i), 20).contains(p))
			return i;

	return -1;
}


void BlockBase::connectTo(BlockBase* block, int pozFrom, int pozTo) {
	if (block == this)
		return;

	block->disconnectInput(pozTo);
	connectedTo.at(pozFrom).emplace( block, pozTo );
	block->connectedFrom.at(pozTo) = { this, pozFrom };

	block->setUpBlock();
	setUpBlock();

	if(!disableSetUp)
		setUpWires(true);
}

void BlockBase::disableLogic(bool disable){
	disableSetUp = disable;
}




void BlockBase::removeConnections() {
	bool keepDisabled = disableSetUp;
	disableSetUp = true;
	for (int i = 0; i < getOutputCnt(); ++i)
		disconnectOutput(i);

	for (int i = 0; i < getInputCnt(); ++i)
		disconnectInput(i);

	disableSetUp = keepDisabled;
	setUpWires(true);
}

void BlockBase::disconnectInput(int poz){
	auto &par = connectedFrom.at(poz);
	if (par.first == nullptr) return;
	par.first->connectedTo.at(par.second).erase({ this, poz });
	if(!par.first->disableSetUp)
		par.first->setUpWires(false);
	par.first = nullptr;

	if (!disableSetUp)
		setUpWires(false);
}

void BlockBase::disconnectOutput(int poz){
	auto& set = connectedTo.at(poz);
	
	for (auto& par : set) {
		par.first->connectedFrom.at(par.second).first = nullptr;
		if (!par.first->disableSetUp)
			par.first->setUpWires(false);
	}

	set.clear();
	if (!disableSetUp)
		setUpWires(false);
}


void BlockBase::switchInput() {
	switchOutput = !switchOutput;
	if (!disableSetUp)
		this->setUpAll();
}

void BlockBase::setPosition(const gui::Point& position){
	_r.setOrigin(position);
	if (!disableSetUp)
		this->setUpAll();
}





 void BlockBase::populateNodes(const Nodes::name* id, int array_size, ModelNode& model, Nodes& nodes) //nez zasto samo ovo bas ovako odradio ali ideja je da se ova funkcija poziva da se dobiju pointeri na specificne tag-ove u modelu odnosno pointer na Vars: Params:...
{
	static constexpr const char* nazivi[] = { "TFs", "NLEqs", "Vars", "Params" }; //redom imena iz Node::enum class-a
	static constexpr const char* naziviInit[] = { "TFs:", "NLEqs:", "Vars:", "Params:" };
	for (int i = 0; i < array_size; ++i) {
		if (nodes.nodes[(int)id[i]] != nullptr)
			continue;

		for (const auto& node : model.getNodes())
			if (std::strcmp(node->getName(), nazivi[(int)id[i]]) == 0) {
				nodes.nodes[(int)id[i]] = node;
				goto NEXT_NODE;
			}

		model.processCommands(naziviInit[(int)id[i]]);
		--i;

	NEXT_NODE:
		continue;
	}
}

 gui::Font BlockBase::blockFont;
 bool BlockBase::fontInit = false;

 BlockBase::BlockBase(const gui::Point& position, kanvas* parent):
	 canvasParent(parent)
{
	_r.setOrigin(position);
	if (!fontInit) {
		const double fontSize = GlobalEvents::settingsVars.textSize;
		if(GlobalEvents::settingsVars.font.cCompareNoCase("Default") == 0)
			blockFont.create("Times", fontSize, gui::Font::Style::Normal, gui::Font::Unit::LogicalPixel);
		else
			blockFont.create(GlobalEvents::settingsVars.font, fontSize, gui::Font::Style::Normal, gui::Font::Unit::LogicalPixel);

		gui::Size sz;
		gui::DrawableString s = "1234567890?+-";
		s.measure(&blockFont, sz);
		s.measure(&blockFont, sz);
		_textHeight = sz.height;

		fontInit = true;
	}
}



BlockBase::~BlockBase(){

}


void BlockBase::updateSettingsView(settingsView* view)
{
	view->currentBlock = this;
	view->checkBoxSwitch.setChecked(switchOutput, false);

}
