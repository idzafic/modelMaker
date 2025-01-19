#include "gui/View.h"
#include "gui/Dialog.h"
#include "gui/ComboBox.h"

namespace gui
{
namespace plot
{

class TxtDlg : public gui::Dialog {
    
public:
    bool horizontal = true;
    TxtDlg(gui::Frame* pFrame, td::UINT4 wndID = 0);
    ~TxtDlg();
};

} //namepsace plot
} //namespace gui
