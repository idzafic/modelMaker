#pragma once
#include "squareBlock.h"
#include "../blockInterface.h"
#include "../canvas.h"


class NLBlock : public squareBlock, public squareBlockSO, public squareBlockMI
{
public:
	class settingsView :
		public BlockBase::settingsView,
		public squareBlockSO::settingsView,
		public squareBlockMI::settingsView
	{

		gui::VerticalLayout vL;
		elementProperty equation;
		NLBlock* thisBlock = nullptr;

	public:
		settingsView():
			vL(5),
			equation("Equation:", td::string8, "Nonlinear equation that combines input variables")
		{
			equation.Action = [this](const td::Variant& v) {thisBlock->setEquation(v.strVal()); };
			vL << equation << BlockBase::settingsView::vL << squareBlockSO::settingsView::vL << squareBlockMI::settingsView::vL;
			vL.appendSpacer();
			setLayout(&vL);
		}
		friend class NLBlock;
	};

private:

	gui::DrawableString eqDraw;

public:

	NLBlock(const gui::Point &position, kanvas *parent);
	int getCnt() override { return canvasParent->getBlockCnt().getCnt<decltype(*this)>(); }

	void setUpBlock() override;
	void drawBlock(const td::ColorID &color) override;
	void writeToModel(ModelNode& model, Nodes& nodes) override;
	void saveToFile(arch::ArchiveOut& f) override;
	static NLBlock* restoreFromFile(arch::ArchiveIn& f, kanvas* parent);
	void updateSettingsView(BlockBase::settingsView* view) override;

	void setEquation(td::String eq);

	static td::String getID() { return "NLBlock"; }
	~NLBlock();
	friend class blockInterface;
};
