#include "DataDrawer.h"
#include "Table.h"
#include "GlobalEvents.h"


DataDraw::PlotDesc::FunctionDesc::FunctionDesc(const ModelSettings::PlotDesc::FunctionDesc &&fun, unsigned int size, double *data):
    size(size),
    y(data),
    name(fun.name),
    attribs(std::move(fun.attribs))
{
    
}


DataDraw::DataDraw(gui::TabView* tabView):
	imgGraph(":graphData"),
	imgTable(":tableData"),
	_hl(1),
	_tabViewOwnership(tabView == nullptr),
	_tabView((tabView == nullptr) ? new gui::TabView(gui::TabHeader::Type::Dynamic, 10, 50) : tabView)
{
	if (_tabViewOwnership) {
		_hl << *_tabView;
		setLayout(&_hl);
	}
}

void DataDraw::measure(gui::CellInfo& cell)
{
	gui::View::measure(cell);
	//cell.nResHor = 1;
	//cell.nResVer = 1;
	cell.minHor = 500;
}





void DataDraw::addData(const td::String &tabName, const PlotDesc &plot)
{
    using Type = ModelSettings::PlotDesc::Type;
    gui::BaseView* tab = nullptr;
    gui::Image *img = nullptr;
    

    for (int i = 0; i < _tabView->getNumberOfViews(); ++i){
        if(_tabView->getView(i)->gui::Frame::getTagID().getType() == td::string8 &&\
           _tabView->getView(i)->gui::Frame::getTagID().strVal() == tabName)
        {
            _tabView->removeView(i); 
            break;
        }
    }



    if (plot.type == Type::graph || plot.type == Type::scatter)
    {
        static gui::Font *f = new gui::Font();
        static bool init = false;
        if(!init){
            init = true;
            f->create(GlobalEvents::settingsVars.font, 10, gui::Font::Style::Normal, gui::Font::Unit::Point);
        }



        auto g = new gui::plot::View(f, f, td::ColorID::SysCtrlBack, false);
        td::String name;
        bool empty = true;
        for (const PlotDesc::FunctionDesc& f : plot.yAxis) {
            
            if(f.size == 0)
                continue;
            if(f.size == 1){
                if(f.attribs.contains("inverse"))
                    g->verticals.emplace_back(*f.y);
                else
                    g->horizontals.emplace_back(*f.y);
                continue;
            }
            double lineWidth = 2;
            if(f.attribs.contains("width")){
                lineWidth = f.attribs.find("width")->second.toDouble();
            }
            using Pattern = gui::plot::Function::Pattern;
            Pattern p = (plot.type == Type::graph) ? Pattern::LinePattern::DefaultLine : Pattern::LinePattern::DefaultDot;
            if(f.attribs.contains("pattern")){
                auto found = Pattern::toLinePattern(f.attribs.find("pattern")->second.c_str());
                if(found.pattern != Pattern::LinePattern::DefaultLine)
                    p = found;
            }

            name = f.name;
            if(f.attribs.contains("name"))
                name = f.attribs.find("name")->second;

            g->addFunction(plot.x, f.y, f.size, lineWidth, p, name);
            empty = false;
        }

        g->setxAxisName(plot.xName);
        if(plot.yAxis.size() == 1)
            g->setyAxisName(name);
        g->fitToWindow();
        tab = g;
        img = &imgGraph;
        
        if(empty){
            delete tab;
            tab = nullptr;
        }
    }
    
    if(tab != nullptr){
        _tabView->addView(tab, tabName, img, (td::BYTE) DocumentType::Graph);
        tab->setTagID(tabName);
    }
}

void DataDraw::removeTabs()
{
    if(_tabViewOwnership)
	    _tabView->removeAll();
}

DataDraw::~DataDraw()
{
	if(_tabViewOwnership)
		delete _tabView;
}


