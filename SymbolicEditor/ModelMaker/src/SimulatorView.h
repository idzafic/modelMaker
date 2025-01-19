#include "BaseView.h"

template<typename simType>
class SimulatorView : public ViewForTab{
    simType simulator;
    public:
    SimulatorView(const td::String &defaultName, const td::String &path = td::String()):
        ViewForTab()
    {
        init(&simulator, defaultName, path);
    }
};