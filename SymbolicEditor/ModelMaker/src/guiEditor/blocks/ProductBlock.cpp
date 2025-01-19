#include "ProductBlock.h"



ProductBlock::ProductBlock(kanvas *parent): BlockBase({0,0}, parent) {
	disableSetUp = true;
}


ProductBlock::ProductBlock(const gui::Point &position, kanvas *parent, const td::String &gain):
    BlockBase(position, parent)
{

    td::String ul, izl;
	ul.format("g%d_in", getCnt());
	izl.format("g%d_out", getCnt());
	disableSetUp = true;
	setInputName(ul);
    disableSetUp = false;
	setOutputName(izl);
	canvasParent->getBlockCnt().increaseCnt<decltype(*this)>();

    setGain(gain);
}

void ProductBlock::setUpBlock()
{
    if (disableSetUp)
		return;

    _gainDrawable.measure(&blockFont, _StringSize);
    _r.setWidth(_r.height() * (_StringSize.width + 30) /(_r.height() - _StringSize.height - 10));

    TriangleBlock::setUpBlock();
    squareBlockSI::setUpBlock();
    squareBlockSO::setUpBlock();
}

void ProductBlock::drawBlock(const td::ColorID &color)
{
    TriangleBlock::drawBlock(color);
    if(switchOutput)
         _gainDrawable.draw({_r.right - 10 - _StringSize.width, - _StringSize.height/2 + (_r.bottom + _r.top)/2}, &blockFont, color);
    else
        _gainDrawable.draw({_r.left + 10, - _StringSize.height/2 + (_r.bottom + _r.top)/2}, &blockFont, color);

    squareBlockSI::drawBlock(color);
    squareBlockSO::drawBlock(color);
}

void ProductBlock::updateSettingsView(BlockBase::settingsView *view)
{
    BlockBase::updateSettingsView(view);
	squareBlockSI::updateSettingsView(view);
	squareBlockSO::updateSettingsView(view);

    auto viewPr = dynamic_cast<settingsView*>(view);
	if (viewPr == nullptr) return;

	viewPr->currentBlock = this;
	viewPr->_product.setValue(_gain);

}

td::String ProductBlock::getID()
{
    return "GainBlock";
}

void ProductBlock::setGain(const td::String &gain)
{

    if(gain.isNull()){
        _gainDrawable = "1";
        _gain = "1";
    }
    else{
        _gainDrawable = gain;
        _gain = gain;
    }

    setUpAll(true);
}

void ProductBlock::writeToModel(ModelNode &model, Nodes &nodes)
{
    static constexpr Nodes::name pop_nodes[] = {Nodes::name::var, Nodes::name::nl};
	BlockBase::populateNodes(pop_nodes, 2, model, nodes);

	auto& var = *nodes.nodes[(int)Nodes::name::var];
	auto& nle = *nodes.nodes[(int)Nodes::name::nl];

	td::String inputName = getInputName(0);
	td::String outputName = getOutputName(0);
    td::String upstreamOutput;
    if(getIsConnectedFrom())
        upstreamOutput = getConnectedFromBlocks()[0].first->getOutputName(getConnectedFromBlocks()[0].second);
    
    var.processCommands(outputName);
    if (!getIsConnectedTo())
		var.getNodes().back()->_attribs["out"] = "true";

    int poz = inputName.find("=");
    if(upstreamOutput.isNull() || poz != -1){
        var.processCommands(inputName);
        if (upstreamOutput.isNull()){
		    var.getNodes().back()->_attribs["out"] = "true";
            if(poz != -1)
                nle.processCommands(inputName);
        }
    }
    else
        inputName = upstreamOutput;


    cnt::StringBuilder cmnd;
	outputName = outputName.subStr(0, outputName.find("=") - 1);
    inputName = inputName.subStr(0, inputName.find("=") - 1);
	
        
	cmnd << outputName << " = " << inputName << " * " << _gain;
	nle.processCommands(cmnd.toString());


}

void ProductBlock::saveToFile(arch::ArchiveOut &f)
{
    f << ProductBlock::getID() << getLocation().x << getLocation().y << getInputName(0) << getOutputName(0) << _gain << switchOutput;
}

ProductBlock *ProductBlock::restoreFromFile(arch::ArchiveIn &f, kanvas *parent)
{
	ProductBlock* block = new ProductBlock(parent);
	f >> block->_r.left >> block->_r.top >> block->ulazName >> block->izlazName;
	f >> block->_gain >> block->switchOutput;
    block->_gainDrawable = block->_gain;
	return block;
}
