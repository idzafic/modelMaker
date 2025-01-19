#pragma once
#include "squareBlock.h"
#include "blockBase.h"
#include "../blockInterface.h"
#include <gui/ComboBox.h>
#include <gui/DrawableString.h>
#include "../canvas.h"


class sumBlock : public squareBlockMInameless, public squareBlockSO, public squareBlock 
{
public:
	class settingsView: 
		public squareBlockMInameless::settingsView,
		public squareBlockSO::settingsView,
		public BlockBase::settingsView
	{
		gui::ComboBox izbor;
		sumBlock* currentBlock = nullptr;
	protected:
		gui::VerticalLayout vL;
	public:
		settingsView() :
			vL(6)
		{
			izbor.addItem("-");
			izbor.addItem("+");
			izbor.onChangedSelection([this]() {currentBlock->changeSign(izbor.getSelectedIndex()); });
			vL << izbor << BlockBase::settingsView::vL << squareBlockSO::settingsView::vL << squareBlockMInameless::settingsView::vL;
			vL.appendSpacer();
			setLayout(&vL);
		}
		friend class sumBlock;
	};
	friend class settingsView;

private:

	bool sumOperator;
	gui::DrawableString znak;

public:
	sumBlock(gui::Point position, kanvas *parent, bool addition_operator, int cnt = 2);
	int getCnt() override { return canvasParent->getBlockCnt().getCnt<decltype(*this)>(); }

	void setUpBlock() override;
	void drawBlock(const td::ColorID &color) override;
	void writeToModel(ModelNode& model, Nodes& nodes) override;
	void saveToFile(arch::ArchiveOut& f) override;
	static sumBlock* restoreFromFile(arch::ArchiveIn& f, kanvas* parent);
	void updateSettingsView(BlockBase::settingsView* view) override;

	void changeSign(bool addition);

	static td::String getID() { return "sumBlock"; }
	~sumBlock();
	friend class blockInterface;
};
