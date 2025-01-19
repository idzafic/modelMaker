#pragma once
#include <vector>
#include <gui/View.h>
#include "../../../../EquationToXML/inc/nodes.h"
#include <utility>
#include <gui/CheckBox.h>
#include <gui/Button.h>
#include <gui/Label.h>
#include "./../../../../common/Property.h"
#include <gui/VerticalLayout.h>
#include <arch/ArchiveIn.h>
#include <arch/ArchiveOut.h>
#include <gui/Font.h>

#define FONT_ID gui::Font::ID::SystemNormal

class kanvas;

class BlockBase {
public:

	class settingsView : public gui::View {
		BlockBase* currentBlock = nullptr;
		gui::CheckBox checkBoxSwitch;
		gui::Button buttonDisconnect;
	protected:
		gui::VerticalLayout vL;
	public:
		settingsView() :
			checkBoxSwitch("switch input/output"),
			buttonDisconnect("disconnect wires", "remove all incomming and outgoing connections from the selected block"),
			vL(2)
		{
			buttonDisconnect.onClick([this]() {currentBlock->removeConnections(); });
			checkBoxSwitch.onClick([this]() {currentBlock->switchInput(); });
			vL << checkBoxSwitch << buttonDisconnect;
		}

		friend class BlockBase;
	};

	struct Nodes {
		enum class name{tf, nl, var, par};
		BaseNode* nodes[4] = { nullptr, nullptr, nullptr, nullptr };
	};

private:


protected:

	gui::Rect _r;
	bool switchOutput = false;
	std::vector<std::set<std::pair<BlockBase*, int>>> connectedTo;
	std::vector<std::pair<BlockBase*, int>> connectedFrom;

	bool disableSetUp = false;
	static void populateNodes(const Nodes::name *id, int array_size, ModelNode& model, Nodes &nodes);

	static gui::Font blockFont;
	static bool fontInit;
	static inline double _textHeight = 10;

	kanvas* canvasParent;


public:
	BlockBase(const gui::Point& position, kanvas *parent);
	virtual int getCnt() = 0;

	const gui::Rect& getRect() const;
	const bool getInputSwitched() const;
	virtual const gui::Point& getOutput(int poz) const = 0;
	virtual const gui::Point& getInput(int poz) const = 0;
	virtual int getOutputCnt() const = 0;
	virtual int getInputCnt() const = 0;
	virtual const td::String& getOutputName(int pos) const = 0;
	virtual const td::String& getInputName(int pos) const = 0;
	const gui::Point getLocation() const;
	const std::vector<std::set<std::pair<BlockBase*, int>>>& getConnectedToBlocks() const;
	const std::vector<std::pair<BlockBase*, int>>& getConnectedFromBlocks() const;
	bool getIsConnectedFrom() const;
	bool getIsConnectedTo() const;


	virtual void setUpAll(bool ignoreRelatedBlocks = false);
	virtual void setUpBlock() = 0;
	virtual void setUpWires(bool refreshCanvas) = 0;
	void switchInput();
	void setPosition(const gui::Point& position);
	void removeConnections();
	void disconnectInput(int poz);
	void disconnectOutput(int poz);

	bool intersectsBlock(const gui::Point&);
	int intersectsInput(const gui::Point&) const;
	int intersectsOutput(const gui::Point&) const;


	void connectTo(BlockBase* block, int pozFrom, int pozTo);

	virtual void drawBlock(const td::ColorID &color) = 0;
	void disableLogic(bool disable);


	virtual void updateSettingsView(settingsView* view);

	virtual void writeToModel(ModelNode& model, Nodes &nodes) = 0;

	virtual void saveToFile(arch::ArchiveOut& f) = 0;
	

	virtual ~BlockBase();

};


