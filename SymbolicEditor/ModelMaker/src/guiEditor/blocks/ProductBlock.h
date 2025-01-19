#pragma once
#include "squareBlock.h"
#include "TriangleBlock.h"
#include "../canvas.h"

class ProductBlock : public squareBlockSI, public squareBlockSO, public TriangleBlock{
public:
    class settingsView :
		public squareBlockSO::settingsView,
		public BlockBase::settingsView,
		public squareBlockSI::settingsView
	{

		elementProperty _product;
		gui::VerticalLayout vL;
		ProductBlock* currentBlock = nullptr;
	public:
		settingsView():
			vL(5),
			_product(tr("gain"), td::string8, tr("gainTooltip"))
		{
			_product.Action = [this](const td::Variant& v) {currentBlock->setGain(v.strVal()); };
			vL << _product << BlockBase::settingsView::vL << squareBlockSI::settingsView::vL << squareBlockSO::settingsView::vL;
			vL.appendSpacer();
			setLayout(&vL);
		}
		friend class ProductBlock;
	};
private:

    td::String _gain;
    gui::DrawableString _gainDrawable;
    gui::Size _StringSize;

    ProductBlock(kanvas *parent);

public:
    ProductBlock(const gui::Point& position, kanvas *parent, const td::String &gain = "1");
    void setUpBlock() override;
    void drawBlock(const td::ColorID &color) override;
    void updateSettingsView(BlockBase::settingsView* view) override;
    int getCnt() override { return canvasParent->getBlockCnt().getCnt<decltype(*this)>(); }
    static td::String getID();

    void setGain(const td::String &gain);

    void writeToModel(ModelNode& model, Nodes& nodes) override;
    void saveToFile(arch::ArchiveOut& f) override;
	static ProductBlock *restoreFromFile(arch::ArchiveIn& f, kanvas* parent);

};
