#pragma once
#include <gui/View.h>
#include <gui/Window.h>
#include <gui/TabView.h>
#include <gui/TableEdit.h>
#include <gui/HorizontalLayout.h>
#include <gui/Application.h>
#include <td/String.h>
#include <gui/Image.h>
#include <gui/GridLayout.h>
#include <map>

#include <gui/plot/View.h>
#include "ModelSettings.h"

class DataDraw : public gui::View 
{
	gui::TabView *_tabView;
	gui::HorizontalLayout _hl;
	gui::Image imgGraph, imgTable;
	bool _tabViewOwnership;


public:

	struct PlotDesc {
		ModelSettings::PlotDesc::Type type;
		td::String xName;
		double* x;
		
		struct FunctionDesc{
			unsigned int size;
			double *y;
			td::String name;
			std::map<td::String, td::String> attribs;
			FunctionDesc(const ModelSettings::PlotDesc::FunctionDesc &&fun, unsigned int size, double *data);
		};
		std::vector<FunctionDesc> yAxis;
	};

	

	DataDraw(gui::TabView* tabView = nullptr);
	void measure(gui::CellInfo& cell) override;
	void addData(const td::String &tabName, const std::vector<PlotDesc>& functions);
	void addData(const td::String &tabName, const PlotDesc& functions);
	void removeTabs();
	~DataDraw();
};
