#include "outputBlock.h"


OutputBlock::OutputBlock(kanvas *parent):
    BlockBase({0,0}, parent)
{

}

OutputBlock::OutputBlock(const gui::Point& position, kanvas *parent):
    BlockBase(position, parent)
{
    td::String izl;
    izl.format("out_%d", getCnt());
    setOutputName(izl);

    canvasParent->getBlockCnt().increaseCnt<decltype(*this)>();
}

void OutputBlock::setUpBlock()
{
    if (disableSetUp)
		return;
    _valueDraw = _value;
    CircleBlock::setUpBlock();
    squareBlockSO::setUpBlock();

}

void OutputBlock::drawBlock(const td::ColorID &color)
{
    CircleBlock::drawBlock(color);
    squareBlockSO::drawBlock(color);

    _valueDraw.draw(_r, &blockFont, color, td::TextAlignment::Center, td::VAlignment::Center);
}

void OutputBlock::updateSettingsView(BlockBase::settingsView *view)
{
    BlockBase::updateSettingsView(view);
    squareBlockSO::updateSettingsView(view);

    auto viewOut = dynamic_cast<settingsView*>(view);
    assert(viewOut != nullptr);

    viewOut->_value.setValue(_value, false);
    viewOut->currentBlock = this;
    
}

void OutputBlock::setValue(const td::String &val)
{
    _value = val;
    _valueDraw = val;
    canvasParent->reDraw();
}

void OutputBlock::writeToModel(ModelNode &model, Nodes &nodes)
{

    static constexpr Nodes::name pop_nodes[] = {Nodes::name::var, Nodes::name::nl};
	BlockBase::populateNodes(pop_nodes, 2, model, nodes);

	auto& var = *nodes.nodes[(int)Nodes::name::var];
	auto& nle = *nodes.nodes[(int)Nodes::name::nl];

	td::String outputName = getOutputName(0);
    var.processCommands(outputName);

    _value = _value.trim();

    if (!getIsConnectedTo() || !_value.isNull())
		var.getNodes().back()->_attribs["out"] = "true";

    if(!_value.isNull() && outputName.find("=") == -1){
        td::String nl(outputName);
        nl += " = ";
        nl += _value;
        nle.processCommands(nl);
    }
     

}

void OutputBlock::saveToFile(arch::ArchiveOut &f)
{
    f << OutputBlock::getID() << getLocation().x << getLocation().y << getOutputName(0) << _value << switchOutput;
}

OutputBlock *OutputBlock::restoreFromFile(arch::ArchiveIn &f, kanvas *parent)
{
    OutputBlock* block = new OutputBlock(parent);
	f >> block->_r.left >> block->_r.top >> block->izlazName;
	f >> block->_value >> block->switchOutput;
    
	return block;
}
