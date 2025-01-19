#include "tBlock.h"
#include "../../GlobalEvents.h"


void TFBlock::setNumerator(const td::String& nominator){
	nom = nominator.isNull() ? "1" : nominator;
	drawNom = nom;
	setUpAll();
}


void TFBlock::setDenominator(const td::String& denominator){
	dem = denominator.isNull() ? "1" : denominator;
	drawDem = dem;
	setUpAll();
}






void TFBlock::setUpBlock()
{

	if (disableSetUp)
		return;

	if (drawNom.isInitialized() && drawDem.isInitialized()) {
		gui::Size s1, s2;
		drawNom.measure(&blockFont, s1);
		drawDem.measure(&blockFont, s2);

		gui::CoordType& bigWidth = (s1.width > s2.width) ? s1.width : s2.width;

		_r.setWidth(100 + bigWidth);
		squareBlock::setUpBlock();

		constexpr int offset = 6;

		rectangleNominator.setOrigin({ _r.left, _r.center().y - offset - s1.height });
		rectangleNominator.setSize({ _r.width(), 0 });

		rectangleDenominator.setOrigin({ _r.left, _r.center().y + offset });
		rectangleDenominator.setSize({ _r.width(), 0 });

		gui::Point p[] = { {_r.center().x - bigWidth / 2 - offset, _r.center().y}, {_r.center().x + bigWidth / 2 + offset, _r.center().y} };
		fracLine.createLines(p, 2, 1);

	}
	else {
		drawNom = nom;
		drawDem = dem;
		setUpBlock();
		return;
	}



	squareBlockSO::setUpBlock();
	squareBlockSI::setUpBlock();



	
}



void TFBlock::drawBlock(const td::ColorID &color) {
	
	if (drawNom.isInitialized() && drawDem.isInitialized()) {
		drawNom.draw(rectangleNominator, &blockFont, color, td::TextAlignment::Center);
		drawDem.draw(rectangleDenominator, &blockFont, color, td::TextAlignment::Center);
		fracLine.drawWire(color, 2);
	}



	squareBlock::drawBlock(color);
	squareBlockSO::drawBlock(color);
	squareBlockSI::drawBlock(color);
}


bool TFBlock::hasLaplaceOperator(const td::String& s)
{
	int poz = 0;

	while (true) {
		poz = s.find("s", poz);
		if (poz == -1)
			break;

		char c = (s.length() > poz + 1) ? s.getAt(poz + 1) : '*';
		if (std::isdigit(c) || c == '*' || c == '/' || c == '+' || c == '-' || c == ' ' || c == '^') {
			c = (poz > 0) ? s.getAt(poz - 1) : c = '*';
			if (c == '*' || c == '/' || c == '+' || c == '-' || c == ' ')
				return true;
		}
		++poz;
	}
	return false;
}

TFBlock::TFBlock(kanvas *parent): BlockBase({0,0}, parent) {
	disableSetUp = true;
}

TFBlock::TFBlock(const gui::Point& position, const td::String& inputName, const td::String& outputName, kanvas *parent):
	squareBlockSI(inputName),
	squareBlockSO(outputName),
	BlockBase::BlockBase(position, parent)
{
	setNumerator("1");
	setDenominator("s");

	setInputName(inputName);
	setOutputName(outputName);

	setUpAll();
	canvasParent->reDraw();
}

TFBlock::TFBlock(const gui::Point& position, kanvas *parent):
	BlockBase::BlockBase(position, parent)
{
	td::String ul, izl;
	ul.format("tf%d_in", getCnt());
	izl.format("tf%d_out", getCnt());
	disableSetUp = true;
	setInputName(ul);
	disableSetUp = false;
	setOutputName(izl);
	canvasParent->getBlockCnt().increaseCnt<decltype(*this)>();


	setNumerator("1");
	setDenominator("s");
	setUpAll();
	canvasParent->reDraw();
}


TFBlock::~TFBlock(){
	disableSetUp = true;
	removeConnections();
}

td::String TFBlock::getID()
{
	return "TFBlock";
}

void TFBlock::saveToFile(arch::ArchiveOut& f)
{
	f << TFBlock::getID() << getLocation().x << getLocation().y << getInputName(0) << getOutputName(0) << nom << dem << switchOutput;
}

TFBlock* TFBlock::restoreFromFile(arch::ArchiveIn& f, kanvas* parent)
{
	TFBlock* block = new TFBlock(parent);
	f >> block->_r.left >> block->_r.top >> block->ulazName >> block->izlazName;
	f >> block->nom >> block->dem >> block->switchOutput;
	return block;
}

void TFBlock::updateSettingsView(BlockBase::settingsView* view)
{
	BlockBase::updateSettingsView(view);
	squareBlockSI::updateSettingsView(view);
	squareBlockSO::updateSettingsView(view);

	auto viewTf = dynamic_cast<settingsView*>(view);
	if (viewTf == nullptr) return;

	viewTf->currentBlock = this;
	viewTf->num.setValue(nom);
	viewTf->dem.setValue(dem);
}

void TFBlock::writeToModel(ModelNode& model, Nodes& nodes)
{
	static constexpr Nodes::name pop_nodes[] = {Nodes::name::var, Nodes::name::nl, Nodes::name::tf};
	BlockBase::populateNodes(pop_nodes, 3, model, nodes);
	
	auto& var = *nodes.nodes[(int)Nodes::name::var];
	auto& tfs = *nodes.nodes[(int)Nodes::name::tf];
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



	if (hasLaplaceOperator(nom) || hasLaplaceOperator(dem)) {
		cmnd << outputName << "/" << inputName << " = " << "(" << nom << ")" << "/(" << dem << ")";
		tfs.processCommands(cmnd.toString());
	}
	else {
		cmnd << outputName << " = " << "(" << nom << " * " << inputName << ")/(" << dem << ")";
		nle.processCommands(cmnd.toString());
	}



}

