#pragma once
#include "CircleBlock.h"


class OutputBlock : public squareBlockSO, public squareBlockNI, public CircleBlock
{
public:
 class settingsView :
		public squareBlockSO::settingsView,
		public BlockBase::settingsView
	{
		elementProperty _value;
		gui::VerticalLayout _vL;
		OutputBlock* currentBlock = nullptr;
	public:
		settingsView():
			_vL(4),
			_value(tr("output"), td::string8, tr("outputTooltip"))
		{
			_value.Action = [this](const td::Variant& v) {currentBlock->setValue(v.strVal()); };
			_vL << _value << BlockBase::settingsView::vL << squareBlockSO::settingsView::vL;
			_vL.appendSpacer();
			setLayout(&_vL);
		}
		friend class OutputBlock;
	};

private:

    td::String _value;
    gui::DrawableString _valueDraw;
    OutputBlock(kanvas *parent);
    
public:
    OutputBlock(const gui::Point& position, kanvas *parent);
    void setUpBlock() override;
    void drawBlock(const td::ColorID &color) override;
    int getCnt() override { return canvasParent->getBlockCnt().getCnt<decltype(*this)>(); }
    void updateSettingsView(BlockBase::settingsView* view) override;

    void setValue(const td::String &val);
    static td::String getID(){return "OutputBlock";}

    void writeToModel(ModelNode& model, Nodes& nodes) override;
    void saveToFile(arch::ArchiveOut& f) override;
	static OutputBlock *restoreFromFile(arch::ArchiveIn& f, kanvas* parent);

};