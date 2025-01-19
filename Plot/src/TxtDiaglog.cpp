#include "TxtDlg.h"
#include "gui/View.h"
#include "gui/ComboBox.h"
#include "gui/TextEdit.h"

namespace gui
{
namespace plot
{

class diagView : public gui::View {

    gui::ComboBox picker;
    gui::VerticalLayout v;
    gui::TextEdit text;
    bool& horizontal;

    const char* infoHor = "Write data horiontaly. Format is : \n\nname[xVal1,  xVal2...]\n\nFirst data row is x value and the second one is y value.The name of the function is the name of the y data row(both names can be empty)."
            "if more than one pair of data is present then additional pairs are added as seperate functions.";

    const char* infoVer = "Write data vertically. Format is: \n\nname\nxVal    yVal\nxVal2    yVal2\n...\n\nIf there are more than 2 numbers per line then additional pairs are regarded as seperate functions."
        "\nIf function dont have the same lenght the longer ones have to be printed out first";

public:

    diagView(bool &horizontal) :v(2), horizontal(horizontal)
    {
        horizontal = true;
        picker.addItem("Horizontal data");
        picker.addItem("Vertical data");

        text.setAsReadOnly(true);
        text.setText(infoHor);
        

        v << picker << text;
        picker.selectIndex(0);

        setLayout(&v);


    }

    bool onChangedSelection(gui::ComboBox* pCmb);

    

};

bool diagView::onChangedSelection(gui::ComboBox* pCmb){

    if (pCmb->getSelectedIndex() == 0) {
        text.setText(infoHor);
        horizontal = true;
        
    }

    if (pCmb->getSelectedIndex() == 1) {
        text.setText(infoVer);
        horizontal = false;
    }

    return true;
}

diagView* viewPtr;

TxtDlg::TxtDlg(gui::Frame* pFrame, td::UINT4 wndID) : gui::Dialog(pFrame, { {gui::Dialog::Button::ID::OK, "Save"} }, gui::Size(1000, 400), wndID)
{

    viewPtr = new diagView(horizontal);

    setCentralView(viewPtr);

}

TxtDlg::~TxtDlg(){
    delete viewPtr;
}


} //namepsace plot
} //namespace gui
