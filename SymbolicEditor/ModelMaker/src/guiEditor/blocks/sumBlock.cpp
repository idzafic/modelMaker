#include "sumBlock.h"

sumBlock::sumBlock(gui::Point position, kanvas* parent, bool addition_operator, int cnt):
	squareBlockMInameless(cnt),
	BlockBase(position, parent)
{
	changeSign(addition_operator);
	td::String name;
	name.format("sum%d", getCnt());
	canvasParent->getBlockCnt().increaseCnt<decltype(*this)>();
	setOutputName(name);

	setUpAll();

}

void sumBlock::setUpBlock()
{
	if (disableSetUp)
		return;

	_r.setWidth(100);
	squareBlock::setUpBlock();
	if (!znak.isInitialized())
		changeSign(sumOperator);

	squareBlockSO::setUpBlock();



}


void sumBlock::drawBlock(const td::ColorID &color)
{
	static gui::DrawableString plusZnak = "+";
	squareBlock::drawBlock(color);
	for(int i = 0; i<getInputCnt(); ++i){
		if(i == 0)
			plusZnak.draw({_r.left + 5, getInput(i).y - _textHeight/2}, &blockFont, color);
		else
			znak.draw({_r.left + 5, getInput(i).y - _textHeight/2}, &blockFont, color);
	}


	squareBlockSO::drawBlock(color);
	
}

void sumBlock::writeToModel(ModelNode& model, Nodes& nodes)
{
	static constexpr Nodes::name pop_nodes[] = { Nodes::name::var, Nodes::name::nl};
	BlockBase::populateNodes(pop_nodes, 2, model, nodes);

	auto& var = *nodes.nodes[(int)Nodes::name::var];
	auto& nle = *nodes.nodes[(int)Nodes::name::nl];

	td::String outputName = getOutputName(0);

	var.processCommands(outputName);
	if (!getIsConnectedTo())
		var.getNodes().back()->_attribs["out"] = "true";

	outputName = outputName.subStr(0, outputName.find("=") - 1);

	char znak = sumOperator ? '+' : '-';

	cnt::StringBuilder cmnd;
	cmnd << outputName << " = ";
	bool first = true;
	for (const auto& par : getConnectedFromBlocks()) 
		if (par.first != nullptr) {
			if (!first) 
				cmnd << " " << znak << " ";
			td::String name = par.first->getOutputName(par.second);
			cmnd << name.subStr(0, name.find("=") - 1);
			first = false;
		}

	if (!first)
		nle.processCommands(cmnd.toString());
	else {
		cmnd << "0";
		nle.processCommands(cmnd.toString());
	}
	


}

void sumBlock::saveToFile(arch::ArchiveOut& f)
{
	f << sumBlock::getID() << getLocation().x << getLocation().y << sumOperator << getInputCnt() << switchOutput << izlazName;
}

sumBlock* sumBlock::restoreFromFile(arch::ArchiveIn& f, kanvas *parent)
{
	gui::Point p;
	bool opr;
	int cnt;
	f >> p.x >> p.y >> opr >> cnt;
	auto pok = new sumBlock(p, parent, opr, cnt);
	f >> opr;
	if(opr)
		pok->switchInput();

	f >> pok->izlazName;

	return pok;
}

void sumBlock::updateSettingsView(BlockBase::settingsView* view)
{
	squareBlockMInameless::updateSettingsView(view);
	squareBlockSO::updateSettingsView(view);
	BlockBase::updateSettingsView(view);
	auto pogled = (settingsView*)view;
	pogled->currentBlock = this;
	pogled->izbor.selectIndex(sumOperator);
}

void sumBlock::changeSign(bool addition)
{
	sumOperator = addition;
	znak = sumOperator ? "+" : "-";
	setUpBlock();
	canvasParent->reDraw();
}

sumBlock::~sumBlock()
{
	disableSetUp = true;
	removeConnections();
}
