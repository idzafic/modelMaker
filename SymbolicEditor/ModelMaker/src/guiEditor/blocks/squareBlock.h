#pragma once
#include "blockBase.h"
#include <vector>
#include <deque>
#include <gui/Shape.h>
#include <td/String.h>
#include <gui/DrawableString.h>
#include "../canvas.h"

class squareBlock : virtual public BlockBase {

	gui::Rect inputRect, outputRect;

	void createArrow(gui::Shape& arrow, const gui::Point &posBegin, const gui::Point &posEnd, const double &lenght);
	std::vector<gui::Shape> arrows;

	std::vector<gui::Point> inputPoints, outputPoints;
	gui::Shape recShape;

protected:
	std::vector<gui::Shape> connectionLines;
	void drawArrows(const td::ColorID &color);

public:
	squareBlock();
	void setUpWires(bool refreshCanvas) override;
	void drawBlock(const td::ColorID &color) override;
	void setUpBlock() override;

	const gui::Point& getInput(int poz) const override
    {
		return inputPoints.at(poz);
	}

	const gui::Point& getOutput(int poz) const override
    {
		return outputPoints.at(poz);
	}
};


class squareBlockSI : virtual public BlockBase {
public:
	class settingsView{
		gui::plot::ElementProperty inputProp;
	protected:
		gui::VerticalLayout vL;
	public:
		settingsView() :
			vL(1),
			inputProp("input var: ", td::string8, "variable name of this block's output")
		{
			vL << inputProp;
		}
		friend class squareBlockSI;
	};

private:
	gui::DrawableString drawUlaz;
	gui::Rect inputRect;

protected:
	td::String ulazName;
	squareBlockSI(){
		connectedFrom.resize(1);
	}
public:
	squareBlockSI(const td::String& inputName): ulazName(inputName){
		connectedFrom.resize(1);
	}

	int getInputCnt() const override { return 1; }

	void setInputName(const td::String& name);
	const td::String& getInputName(int pos) const override{ return ulazName; }
	void updateSettingsView(BlockBase::settingsView* view) override;

	void drawBlock(const td::ColorID &color) override;
	void setUpBlock() override;

};


class squareBlockSO : virtual public BlockBase {
public:
	class settingsView{
		gui::plot::ElementProperty outputProp;
	protected:
		gui::VerticalLayout vL;
	public:
		settingsView() :
			vL(1),
			outputProp("output var: ", td::string8, "variable name of this block's input")
		{
			vL << outputProp;
		}
		friend class squareBlockSO;
	};
protected:
	td::String izlazName;
	squareBlockSO() { connectedTo.resize(1); }

private:
	gui::DrawableString drawIzlaz;
	gui::Rect outputRect;

public:
	squareBlockSO(const td::String& outputName);
	int getOutputCnt() const override{ return 1; }
	const td::String& getOutputName(int pos) const override { return izlazName; }
	void setOutputName(const td::String& name);
	void updateSettingsView(BlockBase::settingsView* view) override;

	void drawBlock(const td::ColorID &color) override;
	void setUpBlock() override;

};


class squareBlockMInameless : virtual public BlockBase {
	int inputCnt;
public:
	class settingsView {
        gui::plot::ElementProperty cntEdit;
		squareBlockMInameless* currentBlockc = nullptr;
	protected:
		gui::VerticalLayout vL;
	public:
		settingsView() :
			vL(1),
			cntEdit("Number of inputs:", td::DataType::int4, "Number of inputs for this block")
		{
			vL << cntEdit;
			cntEdit.Action = [this](const td::Variant& v) {currentBlockc->changeInputCnt(v.i4Val());};
		}
		friend class squareBlockMInameless;
	};
	friend class settingsView;
protected:
	
public:
	squareBlockMInameless(int inputs_cnt);
	int getInputCnt() const override { return inputCnt; }
	const td::String& getInputName(int pos) const override;
	virtual void changeInputCnt(int cnt);
	void updateSettingsView(BlockBase::settingsView* view) override;

};


class squareBlockMI : virtual public BlockBase, public squareBlockMInameless
{
public:
	class settingsView
	{
        gui::plot::ElementProperty cntEdit;
		gui::VerticalLayout* dynamicVL = nullptr;
		std::deque<gui::plot::ElementProperty> inputs;
		squareBlockMI *currentBlock = nullptr;
	protected:
		gui::View vL;
		
	public:
		settingsView():
			cntEdit(gui::tr("NumberOfInputs"), td::DataType::int4, gui::tr("NumberOFInputTooltip"))
		{
			
		}
		friend class squareBlockMI;
	};

private:
	std::vector<td::String> names;
	int thisBlockCnt;
public:
	squareBlockMI(int inputs_cnt);
	void setUp();

	const td::String& getInputName(int pos) const override;
	void setInputName(const td::String &name, int pos);
	void changeInputCnt(int cnt) override;
	void updateSettingsView(BlockBase::settingsView* view) override;
	void drawBlock(const td::ColorID &color) override;
	void setUpBlock() override;
	void saveToFile(arch::ArchiveOut& f) override;
	void restoreFromFile(arch::ArchiveIn& f);
};

class squareBlockNI : virtual public BlockBase{
	td::String name = "";
public:
	squareBlockNI();
	int getInputCnt() const override { return 0; }
	const td::String& getInputName(int pos) const override{ return name; }
};
