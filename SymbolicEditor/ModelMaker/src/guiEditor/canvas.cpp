#include "canvas.h"
#include "blocks/tBlock.h"
#include "blocks/sumBlock.h"
#include "blocks/nlBlock.h"
#include "blocks/ProductBlock.h"
#include "blocks/outputBlock.h"
#include "propertySwitcher.h"
#include "view.h"
#include <gui/Application.h>


#define BLOCK_COLOR td::ColorID::SysText
#define BLOCK_COLOR_SELECTED td::ColorID::DeepSkyBlue


kanvas::kanvas(properties* props, GraphicalEditorView *tabCore):
	gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, \
	gui::InputDevice::Event::PrimaryDblClick, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag, \
	gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorMove }),
	props(props),
	tabCore(tabCore)
{
	this->registerForScrollEvents();
}

properties* kanvas::getProperties()
{
	return props;
}

kanvas* kanvas::getCanvas()
{
	return this;
}


void kanvas::onDraw(const gui::Rect& rect) {

	if (lastAction == Actions::wiring) {
		gui::Shape::drawLine(getFramePoint(tempCurrentBlock.TFBlock->getOutput(tempCurrentBlock.outputPoz)), lastMousePos, BLOCK_COLOR, 2, td::LinePattern::Dash);
	}

	_transformation.appendToContext();

	for (int i = 0; i < _blocks.size(); ++i)
    {
        if (_blocks[i] != tempCurrentBlock.TFBlock)
            _blocks[i]->drawBlock(BLOCK_COLOR);
        else
            _blocks[i]->drawBlock(BLOCK_COLOR_SELECTED);
    }

}


inline void kanvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice) {
	this->setFocus(false);
	tabCore->modelChanged();

	auto point = getModelPoint(inputDevice.getFramePoint());

	for (int i = 0; i < _blocks.size(); ++i)
		if (_blocks[i]->intersectsBlock(point)) {
			lastMousePos = inputDevice.getFramePoint();
			lastAction = Actions::dragging;
			if (_blocks[i] == tempCurrentBlock.TFBlock)
				return;
			tempCurrentBlock.TFBlock = _blocks[i];
			props->showSettings(tempCurrentBlock.TFBlock);
			reDraw();
			return;
		}

	for (int i = 0; i < _blocks.size(); ++i)
		if (_blocks[i]->intersectsOutput(point) != -1) {
			tempCurrentBlock.TFBlock = _blocks[i];
			tempCurrentBlock.outputPoz = _blocks[i]->intersectsOutput(point);
			lastAction = Actions::wiring;
			return;
		}

	tempCurrentBlock.TFBlock = nullptr;
	props->showSettings(nullptr);
	reDraw();

}

inline void kanvas::onCursorDragged(const gui::InputDevice& inputDevice) {
	if (lastAction == Actions::wiring) {
		lastMousePos = inputDevice.getFramePoint();
		reDraw();
		return;
	}
	else if (lastAction == Actions::dragging) {
		gui::CoordType deltaX = (inputDevice.getFramePoint().x - lastMousePos.x) / _scale;
		gui::CoordType deltaY = (inputDevice.getFramePoint().y - lastMousePos.y) / _scale;

		tempCurrentBlock.TFBlock->setPosition({ tempCurrentBlock.TFBlock->getRect().left + deltaX, tempCurrentBlock.TFBlock->getRect().top + deltaY });
		lastMousePos = inputDevice.getFramePoint();
		return;
	}
}

inline void kanvas::onPrimaryButtonReleased(const gui::InputDevice& inputDevice) {
	if (lastAction == Actions::wiring) {
		for (int i = 0; i < _blocks.size(); ++i)
			if (_blocks[i]->intersectsInput(getModelPoint(inputDevice.getFramePoint())) != -1) {
				const int& poz = _blocks[i]->intersectsInput(getModelPoint(inputDevice.getFramePoint()));
				tempCurrentBlock.TFBlock->connectTo(_blocks[i], tempCurrentBlock.outputPoz, poz);
				break;
			}
		lastAction = Actions::none;
		reDraw();
		return;
	}

	lastAction = Actions::none;
}

inline void kanvas::onPrimaryButtonDblClick(const gui::InputDevice& inputDevice) {

}


inline bool kanvas::onZoom(const gui::InputDevice& inputDevice) {
	zoom(inputDevice.getScale());

	return true;
}

void kanvas::zoom(double factor)
{
	static gui::Size sz;
	getSize(sz);
	gui::Point mousePoint(getModelPoint(gui::Point(sz.width / 2, sz.height / 2)));
	scroll({ -mousePoint.x, -mousePoint.y });
	
	_scale *= factor;
	_transformation.scale(factor);
	_totalScroll.x /= factor;
	_totalScroll.y /= factor;

	scroll(mousePoint);

	reDraw();
}

void kanvas::scroll(gui::Point delta)
{
	delta.x *= -1;
	delta.y *= -1;
	_transformation.translate(delta);
	_totalScroll += delta;
	reDraw();
}

bool kanvas::onScroll(const gui::InputDevice& inputDevice)
{
	auto point = inputDevice.getScrollDelta();
	if (!inputDevice.getKey().isShiftPressed())
		scroll({ point.x * 20, point.y *20});
	else
		scroll({ point.y * 20, point.x *20 });

	return true;
}

gui::Point kanvas::getFramePoint(const gui::Point& framePoint)
{
	return { (framePoint.x + _totalScroll.x) * _scale, (framePoint.y + _totalScroll.y) * _scale };
}

gui::Point kanvas::getModelPoint(const gui::Point& framePoint)
{
	return { framePoint.x / _scale - _totalScroll.x, (framePoint.y) / _scale - _totalScroll.y };
}

void kanvas::getModel(ModelNode& mod)
{
	mod.clear();

	mod.processCommands("Model:\nParams:\nVars:");
	BlockBase::Nodes nodes;
	
	BaseNode& params = *mod.getNodes()[0];

	for (auto& block : _blocks)
		block->writeToModel(mod, nodes);

	td::Variant v;
	props->getModelSettings()->edit.getValue(v);

	params.processCommands("t // time");
	params.processCommands(v.strVal());

	mod._attribs = std::move(params._attribs);
	params._attribs.clear();

	mod["domain"] = "real";
	mod["type"] = "DAE";
	if(auto name = props->getModelSettings()->name.getValue().strVal(); !name.isNull())
		mod["name"] = name;

}

bool kanvas::saveState(const td::String& file, const td::String& settingsString)
{
	arch::FileSerializerOut temp;
	if (!temp.open(file))
		return false;

	arch::ArchiveOut out("TFv3", temp);
	try {
		out << settingsString;
		out << int(_blocks.size());
		for (BlockBase* b : _blocks)
			b->saveToFile(out);

		for (int i = 0; i < _blocks.size(); ++i) {
			auto& vec = _blocks[i]->getConnectedToBlocks();
			out << int(vec.size());

			for  (auto &set : vec) {
				out << int(set.size());
				for (auto& par : set) {
					auto it = std::find(_blocks.begin(), _blocks.end(), par.first);
					out << int(it - _blocks.begin()) << par.second;
				}
			}
		}

		out << props->getModelSettings()->name.getValue().strVal();
		td::Variant v;
		props->getModelSettings()->edit.getValue(v);

		out << v.strVal();

		blockCnts.writeState(out);

	}
	catch (...){
		return false;
	}

	return true;

}

bool kanvas::restoreState(const td::String& file, td::String& settingsString)
{	
	arch::FileSerializerIn temp;
	if (!temp.open(file))
		return false;

	arch::ArchiveIn in(temp);
	in.setSupportedMajorVersion("TFv3");
	std::vector<BlockBase*> kopija;
	td::Variant titl_v, param_v;
	blockCnts.saveState();

	try {
		settingsString.clean();
		in >> settingsString;
		int size = 0;
		in >> size;
		kopija.resize(size, nullptr);

		td::String ID;
		for (int i = 0; i < size; ++i) {
			in >> ID;
			if (ID == TFBlock::getID())
				kopija[i] = TFBlock::restoreFromFile(in, this);
			else if (ID == sumBlock::getID())
				kopija[i] = sumBlock::restoreFromFile(in, this);
			else if (ID == NLBlock::getID())
				kopija[i] = NLBlock::restoreFromFile(in, this);
			else if (ID == ProductBlock::getID())
				kopija[i] = ProductBlock::restoreFromFile(in, this);
			else if (ID == OutputBlock::getID())
				kopija[i] = OutputBlock::restoreFromFile(in, this);
			else
				throw std::logic_error("unknown block");

			kopija[i]->disableLogic(false);
			kopija[i]->setUpBlock();
		}
		for (int i = 0; i < size; ++i) {
			int setCnt, connectionsCnt;
			in >> connectionsCnt;
			for (int j = 0; j < connectionsCnt; ++j) {
				in >> setCnt;
				for (int k = 0; k < setCnt; ++k) {
					int broj, ulazCnt;
					in >> broj >> ulazCnt;
					kopija[i]->connectTo(kopija[broj], j, ulazCnt);
				}
			}

		}


		for (auto& block : kopija)
			block->setUpWires(true);
		

		td::String titl;
		in >> titl;
		titl_v = std::move(titl);

		td::String params;
		in >> params;
		param_v = std::move(params);
		
		blockCnts.reloadState(in);

	}
	catch (...) {
		blockCnts.restoreState();
		for (int i = 0; i < kopija.size(); ++i)
			delete kopija[i];
		return false;
	}


	resetModel(false);


	_blocks = std::move(kopija);
	props->getModelSettings()->edit.setValue(param_v, false);
	props->getModelSettings()->name.setValue(titl_v);

	currentPath = file;

	return true;
}


inline bool kanvas::onKeyPressed(const gui::Key& key)
{
	if (key.getVirtual() == gui::Key::Virtual::Delete) {
		if (tempCurrentBlock.TFBlock != nullptr) {
			tempCurrentBlock.TFBlock->removeConnections();
			_blocks.erase(std::find(_blocks.begin(), _blocks.end(), tempCurrentBlock.TFBlock));
			props->showSettings(nullptr);
			tempCurrentBlock.TFBlock = nullptr;
			lastAction = Actions::none;
			reDraw();

			if (_blocks.size() == 0)
				tabCore->setVersion(0);

		}
		return true;
	}

	if (key.getChar() == 'h' || key.getChar() == 'H') {
		_totalScroll.toZero();
		_scale = 1;
		_transformation.identity();
		zoom(0.8);
	}

	if(key.getChar() == '+' && key.isCtrlPressed())
		zoom(1.1);

	if(key.getChar() == '-' && key.isCtrlPressed())
		zoom(0.9);


	return false;
}


void kanvas::onSecondaryButtonPressed(const gui::InputDevice& inputDevice) {
	lastMousePos = inputDevice.getFramePoint();
	if(inputDevice.getKey().isCtrlPressed())
		lastAction = Actions::secondary;	
	else
		Frame::openContextMenu(100, inputDevice);
}


void kanvas::onSecondaryButtonReleased(const gui::InputDevice& inputDevice)
{
	lastAction = Actions::none;
}

void kanvas::onCursorMoved(const gui::InputDevice& inputDevice)
{
	if (lastAction == Actions::secondary) {
		scroll({(- inputDevice.getFramePoint().x + lastMousePos.x) / _scale, (- inputDevice.getFramePoint().y + lastMousePos.y) / _scale});
		lastMousePos = inputDevice.getFramePoint();
		reDraw();
	}
}


void kanvas::resetModel(bool resetCnt)
{
	for (int i = 0; i < _blocks.size(); ++i)
		delete _blocks[i];
	_blocks.clear();
	props->showView(0);
	if (resetCnt) {
		blockCnts.resetAll();
	}
	props->getModelSettings()->edit.clean();
}

void kanvas::reDraw()
{
	gui::Frame::reDraw();
}

blockInterface& kanvas::getBlockCnt()
{
	return blockCnts;
}

inline bool kanvas::onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu)
{

	return false;
}

inline bool kanvas::onActionItem(gui::ActionItemDescriptor& aiDesc) {
	tabCore->modelChanged();

	if (aiDesc._menuID == 100) {
		if (aiDesc._actionItemID == 10) {
			_blocks.push_back(new TFBlock(getModelPoint(lastMousePos), this));
			return true;
		}
		else if (aiDesc._actionItemID == 11) {
			_blocks.push_back(new sumBlock(getModelPoint(lastMousePos), this, true));
			return true;
		}
		else if (aiDesc._actionItemID == 12) {
			_blocks.push_back(new NLBlock(getModelPoint(lastMousePos), this));
			return true;
		}
		else if (aiDesc._actionItemID == 13) {
			_blocks.push_back(new ProductBlock(getModelPoint(lastMousePos), this));
			return true;
		}
		else if (aiDesc._actionItemID == 14) {
			_blocks.push_back(new OutputBlock(getModelPoint(lastMousePos), this));
			return true;
		}
	}

	return false;
}





bool kanvas::exportToXML(const td::String &path){

	ModelNode mod;
	getModel(mod);
	return mod.printNode(path);

}


kanvas::~kanvas() {
	for (int i = 0; i < _blocks.size(); ++i)
		delete _blocks[i];
}
