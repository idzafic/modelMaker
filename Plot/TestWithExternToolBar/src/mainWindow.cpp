#include <gui/Window.h>
#include "mainWindow.h"
#include <gui/plot/View.h>

#include <gui/FileDialog.h>

static gui::Font *getFont(gui::Font &font){
    if(!font.isOk()){
        auto fonts = gui::Font::getSystemFamilyNames();
        font.create(fonts[0], 10, gui::Font::Style::Normal, gui::Font::Unit::LogicalPixel);
    }
    return &font;
}

MainWindow::MainWindow()
    : gui::Window(gui::Geometry(600, 100, 1500, 1500)), 
    _graph(getFont(font), getFont(font), td::ColorID::SysCtrlBack, false), 
    splitter(_graph)
{
    
    setTitle("Graph");
    _mainMenuBar.setAsMain(this);
    setToolBar(_toolBar);

    int broj = 1001;
    gui::CoordType* x = new gui::CoordType[broj];
    gui::CoordType* y = new gui::CoordType[broj];

    for (int i = 0; i < broj; ++i) {
        x[i] = i;
        y[i] = i * i;
    }

    

    gui::CoordType* xx = new gui::CoordType[6];
    gui::CoordType* yy = new gui::CoordType[6];

    xx[0] = 0; xx[1] = 100; xx[2] = 200; xx[3] = 300; xx[4] = 300; xx[5] = 200;
    yy[0] = 0; yy[1] = -100; yy[2] = 50; yy[3] = 50; yy[4] = 400; yy[5] = 400;

    _graph.addFunction(xx, yy, 6);

    _graph.addFunction(x, y, broj);

  
    setCentralView(&splitter);

    
    splitter.refreshPicks();


}


bool MainWindow::onActionItem(gui::ActionItemDescriptor& aiDesc) {

    if(aiDesc._menuID == 100){ //toolbar
        _graph.handleAction((gui::plot::View::Action) aiDesc._actionItemID);
        return true;
    }

    if (aiDesc._menuID == 1) {
        if (aiDesc._actionItemID > 2)
            return false;

        resetGraph = false;
        if (aiDesc._actionItemID == 1) {
            _graph.reset();
            resetGraph = true;
        }

        
        gui::OpenFileDialog::show(this, "Read data", { {"","*.txt"},{"", "*.xml"} }, 953, 
            [this](gui::FileDialog* pDlg) {
                open(pDlg->getFileName());
            },
            "plot.txt"
            );
            
    }
    
    if (aiDesc._menuID == 2) {
       

        if (aiDesc._actionItemID == 11) {
            
            gui::SaveFileDialog::show(this, "export data to xml", "*.xml", 953, 
            [this](gui::FileDialog* pDlg) {
                _graph.saveXML(pDlg->getFileName());
            },
            "plot.xml"
            );

            return true;
        }

        if (aiDesc._actionItemID == 10) {

            gui::SaveFileDialog::show(this, "export data to txt", "*.txt", 953, 
            [this](gui::FileDialog* pDlg) {
                _graph.saveTXT(pDlg->getFileName());
            },
            "plot.txt"
            );
        
            return true;
        }

    }

    return false;
}


bool MainWindow::open(const td::String& path){

    if (resetGraph && path.endsWith(".xml"))
        resetGraph = false;
    else
        resetGraph = true;

    bool succes = _graph.openFile(path, resetGraph);
    if (!succes)
        return false;

    if(resetGraph)
        _graph.fitToWindow();

    splitter.refreshPicks();
    return true;
}


void MainWindow::loadFromTerminal(int argc, char** argv){

    if (argc < 2)
        return;

    open(argv[1]);

    for (size_t i = 2; i < argc; ++i){
        open(argv[i]);
    }

}


MainWindow::~MainWindow(){


}







