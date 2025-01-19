#include <gui/WinMain.h>
#include <gui/Application.h>
#include <gui/Window.h>
#include <td/Variant.h>
#include <td/String.h>
#include "td/StringConverter.h"
#include "MainWindow.h"
#include "GlobalEvents.h"


MainWindow* mainWindow = nullptr;

MainWindow *GlobalEvents::getMainWindowPtr()
{
	if(mainWindow == nullptr)
		throw "mainWindow was nullptr";
    return mainWindow;
}

bool GlobalEvents::sendActionItem(gui::ActionItemDescriptor &a)
{
	return GlobalEvents::getMainWindowPtr()->onActionItem(a);
}


class application : public gui::Application {
public:
	application(int argc, const char** argv) : gui::Application(argc, argv) {

	}

	gui::Window* createInitialWindow() override{
		return mainWindow = new MainWindow();
	}

	

};

//td::StringBase<char, (td::_StringEncoding)0, 4, false>::calcHashNo;

int main(int argc, const char** argv) {
	auto app = new application(argc, argv);
	
	app->init(app->getProperties()->getKeyValue("Laungage", "BA").toString());
	return app->run();

}
