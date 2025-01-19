#include"gui/Application.h"
#include"mainWindow.h"
#include <gui/WinMain.h>

class Application : public gui::Application
{


protected:

    gui::Window* createInitialWindow() override
    {
        return new MainWindow();
    }

public:
    Application(int argc, const char** argv)
        : gui::Application(argc, argv)
    {
    }
};





int main(int argc, const char** argv) {

    Application app(argc, argv);
    app.init("BA");
    return app.run();

}
