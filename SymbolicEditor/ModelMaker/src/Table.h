#pragma once
#include <gui/View.h>
#include <vector>
#include <gui/GridLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/TextEdit.h>
#include <gui/ViewScroller.h>
#include <gui/GridComposer.h>
#include <gui/DrawableString.h>
#include <gui/Label.h>
#include <cnt/StringBuilder.h>
#include <gui/Canvas.h>

#include "DataDrawer.h"


/*


class NumberColumn : public gui::Canvas{
	gui::DrawableString numbersStr;
	gui::Font::ID font = gui::Font::ID::SystemLarger;
	gui::Size sz;
	bool textSet = false;
public:
	NumberColumn(){

	}

	void setText(const td::String &text){
		numbersStr = text;
		numbersStr.measure(font, sz);
		textSet = true;
		setSizeLimits(sz.width, gui::Control::Limit::Fixed);
		setBackgroundColor(td::ColorID::LightGray);
	}

	void setNumbers(double *numbers, unsigned int size){
		cnt::StringBuilder str;
		for(int i = 0; i<size; ++i){
			str << numbers[i] << "\n";
		}
		td::String temp;
		str.getString(temp);
		setText(temp);
	}


	void onDraw(const gui::Rect& rect){
		if(textSet)
			numbersStr.draw({0,0}, font, td::ColorID::Black);
	}

	gui::Size getSize(){
		return sz;
	}

};

static inline void setSizeToBigger(gui::Size &initialSize, const gui::Size comparedSize){
	initialSize.width += comparedSize.width;
	initialSize.height = (initialSize.height > initialSize.height) ? initialSize.height : comparedSize.height;
}

class viewWithSize : public gui::View {
public:

	gui::Size size;

	viewWithSize() {
		size = { 0,0 };
	}

	bool getModelSize(gui::Size& modelSize) const override {
		modelSize = size;
		return true;
	}

};

class Table : public gui::ViewScroller {
	std::vector<gui::GridLayout *> layouts;
	std::vector<gui::Label *> labels;
	std::vector<NumberColumn *> numberCols;
	gui::HorizontalLayout _hLayout;
	gui::GridLayout _gridVertLayout;
	gui::VerticalLayout _vLayout;
	viewWithSize mainView, paramsView;
	gui::Size sz = {0,0};

public:
	Table(const std::vector<DataDraw::FunctionDesc> &funcs):
		gui::ViewScroller(gui::ViewScroller::Type::ScrollerAlwaysVisible, gui::ViewScroller::Type::ScrollerAlwaysVisible),
		_hLayout(funcs.size()),
		_gridVertLayout(funcs.size(),3),
		_vLayout(3)
	{
		int verticalCnt = 0;
		


		const auto addParamLine = [&](const td::String& functionName, const td::String& paramName, double* value) {
			td::String result;
			if (functionName != paramName)
				result = functionName;
			else
				result = "";

			result += "\t";

			labels.push_back(new gui::Label(result));

			_gridVertLayout.insert(verticalCnt, 0, *labels.end()[-1]);

			result = paramName;
			result += " = ";

			labels.push_back(new gui::Label(result));
			_gridVertLayout.insert(verticalCnt, 1, *labels.end()[-1], td::HAlignment::Right);
			result.fromNumber(*value);

			labels.push_back(new gui::Label(result));;
			_gridVertLayout.insert(verticalCnt, 2, *labels.end()[-1]);

			verticalCnt += 1;
		};


		for (int i = 0; i < funcs.size(); ++i) {

			if (funcs[i].x == nullptr && funcs[i].y == nullptr) {

				td::String niceName = funcs[i].name;
				niceName += " =\t";

				labels.push_back(new gui::Label(niceName));
				labels.push_back(new gui::Label(funcs[i].yname));
				_gridVertLayout.insert(verticalCnt, 0, *labels.end()[-2]);
				_gridVertLayout.insert(verticalCnt, 2, *labels.end()[-1]);

				verticalCnt += 1;
				continue;
			}


			if (funcs[i].x != nullptr && funcs[i].y == nullptr && funcs[i].size == 1) { //param u x
				addParamLine(funcs[i].name, funcs[i].xname, funcs[i].x);
				continue;
			}

			if (funcs[i].y != nullptr && funcs[i].x == nullptr && funcs[i].size == 1) { //param u y
				addParamLine(funcs[i].name, funcs[i].yname, funcs[i].y);
				continue;
			}



			if (funcs[i].x != nullptr && funcs[i].y != nullptr) { //dva niza
				layouts.push_back(new gui::GridLayout(3, 2));
				auto& layout = layouts.back();

				
				labels.push_back(new gui::Label(funcs[i].yname));
				labels.push_back(new gui::Label(funcs[i].xname));
				layout->insert(1, 0, *labels.end()[-2], td::HAlignment::Center);
				layout->insert(1, 1, *labels.end()[-1], td::HAlignment::Center);


				if (funcs[i].name != funcs[i].yname)
					labels.push_back(new gui::Label(funcs[i].name));
				else
					labels.push_back(new gui::Label(""));

				layout->insert(0, 0, *labels.end()[-1], 2, td::HAlignment::Center);
				

				numberCols.push_back(new NumberColumn());
				numberCols.push_back(new NumberColumn());

				numberCols.end()[-2]->setNumbers(funcs[i].y, funcs[i].size);
				numberCols.end()[-1]->setNumbers(funcs[i].x, funcs[i].size);

				setSizeToBigger(sz, numberCols.end()[-2]->getSize());
				setSizeToBigger(sz, numberCols.end()[-1]->getSize());

				layout->insert(2, 0, *numberCols.end()[-2], td::HAlignment::Center);
				layout->insert(2, 1, *numberCols.end()[-1], td::HAlignment::Center);



				_hLayout << *layout;
				continue;
			}

			if (funcs[i].x == nullptr && funcs[i].y != nullptr) { //jedan niz
				layouts.push_back(new gui::GridLayout(3, 2));
				auto& layout = layouts.back();

				labels.push_back(new gui::Label(funcs[i].yname));
				layout->insert(1, 0, *labels.end()[-1], td::HAlignment::Center);

				if (funcs[i].name != funcs[i].yname) 
					labels.push_back(new gui::Label(funcs[i].name));
				else
					labels.push_back(new gui::Label(""));
				
				layout->insert(0, 0, *labels.end()[-1], 2, td::HAlignment::Center);

				
				numberCols.push_back(new NumberColumn());
				numberCols.end()[-1]->setNumbers(funcs[i].y, funcs[i].size);

				setSizeToBigger(sz, numberCols.end()[-1]->getSize());

				layout->insert(2, 0, *numberCols.end()[-1], td::HAlignment::Center);

				_hLayout << *layout;
				continue;

			}


		}



		if (verticalCnt > 0) {
			paramsView.setLayout(&_gridVertLayout);
			_vLayout << paramsView;
			_vLayout.appendSpace(15);
		}

		_vLayout << _hLayout;
		mainView.size = gui::Size(sz.width * 1.1, sz.height + verticalCnt * 35 + 180);
		mainView.setLayout(&_vLayout);
		setContentView(&mainView);

	}

	void measure(gui::CellInfo& c) override {
		gui::ViewScroller::measure(c);
		c.minHor = 0;
		c.nResHor = 1;
	}


	~Table() {
		for (const auto& ptr : labels)
			delete ptr;
		for (const auto& ptr : layouts)
			delete ptr;
		for (const auto& ptr : numberCols)
			delete ptr;

	}
};
*/