#pragma once
#include <gui/ActionItemDescriptor.h>
#include <gui/Window.h>
#include <gui/Label.h>
#include <unordered_set>
#include "td/StringConverter.h"


class MainWindow;
class GlobalEvents;


enum class ColorType : td::BYTE {Constant=0, Function, Attribute, Keyword, Variable, Param, Comment, Import, Text, NONE};

enum class DocumentType : td::BYTE {Unknown = 0, ModelTxtEditor, ModelTFEditor, Graph};

struct SettingsVars
{

    static constexpr int colorCnt = (int) ColorType::NONE;
    cnt::Array<td::String, colorCnt> colorNames;


	td::ColorID colorsLight[colorCnt], colorsDark[colorCnt];
    td::ColorID colors[colorCnt]; //used in code to match those in settings

	double textSize;
	td::String font;
	bool embedPlot, restoreTabs, warnBeforeClose, autoPlotFuncs;
#ifdef MU_MACOS
    td::BYTE tabStopSizeInChars = 2;
#else
    td::BYTE tabStopSizeInChars = 2;
#endif
	
	void saveValues();
	void loadSettingsVars(gui::Application* app);
	void loadDefaultSettings(gui::Application* app);
private:
	void setColors();
	static gui::Application *app;
	friend class GlobalEvents;
};


class GlobalEvents {

public:
	static MainWindow *getMainWindowPtr();
	static bool sendActionItem(gui::ActionItemDescriptor& a);
	static SettingsVars settingsVars;
};

enum class UserEvent: td::UINT4 {ReformatTabs=0, SetModifiedFlag, CloseTab};
