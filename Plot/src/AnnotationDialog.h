#include <gui/View.h>
//#include <gui/Dialog.h>
#include <vector>
#include <gui/VerticalLayout.h>
#include <gui/Label.h>
#include <gui/plot/Property.h>
#include <gui/NumericEdit.h>



class AnnotDiag : public gui::View {

    gui::VerticalLayout verLay;
    gui::Label horLabel, varLabel;
    std::vector<gui::plot::ElementProperty*> props;

public:

    AnnotDiag(gui::Frame* pFrame, std::vector<gui::CoordType> &verticals, std::vector<gui::CoordType>& horizontals) :
    horLabel("Horizontal annotations:"), varLabel("Vertical annotations:"),
    verLay(verticals.size() + horizontals.size() + 2)
    {	

        verLay.append(varLabel);
        int cnt = 0;
        while (cnt < 2) {
            std::vector<gui::CoordType>& vector = (cnt == 0) ? verticals : horizontals;

            if (cnt == 1)
                verLay.append(horLabel);

            for (size_t i = 0; i < vector.size(); ++i) {
                td::String name;
                name.format("line%d: ", i + 1);
                td::Variant v(vector[i]);
                auto ptr = new gui::plot::ElementProperty(name, v.getType(), "x axis value that the vertical line is crossing", v);
                gui::CoordType& val = vector[i];
                
                ptr->Action = [&val](const td::Variant& v) {
                    v.getValue(&val, td::real8);
                };

                props.push_back(ptr);
                verLay.append(*ptr);
            }
            ++cnt;
        }

        setLayout(&verLay);
        
    }

    ~AnnotDiag() {
        for (size_t i = 0; i < props.size(); ++i) {
            delete props[i];
        }
    }


};
