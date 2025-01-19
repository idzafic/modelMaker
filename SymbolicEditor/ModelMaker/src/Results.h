#pragma once
#include <gui/View.h>
#include <gui/TableEdit.h>
#include <dp/IDataSet.h>
#include <gui/VerticalLayout.h>

class Results : public gui::View {
	gui::VerticalLayout _layout;
//	dp::IDataSetPtr _pDS;
	gui::TableEdit _table;
public:
	Results();
	dp::IDataSet* getDataSet();
//	void refresh();
	void show(dp::IDataSet* pDS);
};
