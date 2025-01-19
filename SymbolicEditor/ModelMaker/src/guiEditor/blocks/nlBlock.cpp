#include "nlBlock.h"
#include <gui/Font.h>


NLBlock::NLBlock(const gui::Point& position, kanvas* parent):
	BlockBase(position, parent),
	squareBlockMI(1)
{
	squareBlockMI::setUp();
	td::String name;
	name.format("nl%d_out", getCnt());
	setOutputName(name);
	name.format("2 * nl%d_0", getCnt());
	setEquation(name);

	canvasParent->getBlockCnt().increaseCnt<decltype(*this)>();
}


void NLBlock::setUpBlock()
{
	gui::Size sz;
	eqDraw.measure(FONT_ID, sz);
	_r.setWidth(sz.width + 50);
	squareBlock::setUpBlock();
	squareBlockSO::setUpBlock();
	squareBlockMI::setUpBlock();

}

void NLBlock::drawBlock(const td::ColorID &color)
{
	eqDraw.draw(_r, &blockFont, color, td::TextAlignment::Center, td::VAlignment::Center);

	squareBlock::drawBlock(color);
	squareBlockSO::drawBlock(color);
	squareBlockMI::drawBlock(color);
}

void NLBlock::writeToModel(ModelNode& model, Nodes& nodes)
{
	static constexpr Nodes::name pop_nodes[] = { Nodes::name::var, Nodes::name::nl };
	BlockBase::populateNodes(pop_nodes, 2, model, nodes);

	auto& var = *nodes.nodes[(int)Nodes::name::var];
	auto& nle = *nodes.nodes[(int)Nodes::name::nl];

	td::String outputName = getOutputName(0);


	var.processCommands(outputName);
	if (!getIsConnectedTo())
		var.getNodes().back()->_attribs["out"] = "true";

	cnt::StringBuilder cmnd;
	outputName = outputName.subStr(0, outputName.find("=") - 1);

	cmnd << outputName << " = " << eqDraw.toString();
	nle.processCommands(cmnd.toString());

	for (int i = 0; i < getInputCnt(); ++i) {
		td::String name = getInputName(i);
		var.processCommands(name);
		const auto& par = getConnectedFromBlocks()[i];

		if (par.first != nullptr) {
			name = name.subStr(0, name.find("=") - 1);
			cmnd.reset();
			cmnd << name << " = " << par.first->getOutputName(par.second);
			nle.processCommands(cmnd.toString());
		}else
		if (name.find("=") != -1)
			nle.processCommands(name);
	}


	
}

void NLBlock::saveToFile(arch::ArchiveOut& f)
{
	f << NLBlock::getID() << getLocation().x << getLocation().y << switchOutput << eqDraw.toString();
	squareBlockMI::saveToFile(f);
	
}

NLBlock* NLBlock::restoreFromFile(arch::ArchiveIn& f, kanvas* parent)
{
	gui::Point p;
	f >> p.x >> p.y;
	auto pok = new NLBlock(p, parent);
	pok->disableSetUp = true;

	td::String name;

	f >> pok->switchOutput >> name;
	pok->eqDraw = name;

	pok->squareBlockMI::restoreFromFile(f);
	return pok;

}

void NLBlock::updateSettingsView(BlockBase::settingsView* view)
{
	squareBlockSO::updateSettingsView(view);
	squareBlockMI::updateSettingsView(view);
	BlockBase::updateSettingsView(view);

	auto pogled = dynamic_cast<settingsView*>(view);


	pogled->thisBlock = this;
	pogled->equation.setValue(eqDraw.toString());

	pogled->reMeasure();
	pogled->reDraw();

}

void NLBlock::setEquation(td::String eq)
{
	eqDraw = eq;
	setUpAll();
}

NLBlock::~NLBlock()
{
	disableSetUp = true;
	removeConnections();
}
