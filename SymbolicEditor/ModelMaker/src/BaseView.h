#pragma once
#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/SplitterLayout.h>
#include <unordered_set>
#include <td/StringConverter.h>
#include <gui/StandardTabView.h>
#include <gui/Image.h>
#include <memory>
#include <gui/Application.h>
#include "GlobalEvents.h"

#include "../../EquationToXML/inc/nodes.h"
#include "ModelSettings.h"
#include "Results.h"

class ViewForTab;

class LogView : private gui::View {
	mutable gui::TextEdit textMain;
	gui::VerticalLayout _vl;

	static int errorLen, infoLen, warLen;

	friend class ViewForTab;

public:

	enum class LogType : td::BYTE { Info=0, Warning, Error };
	LogView();
	void appendLog(const td::String text, LogType type, bool discardThisLog = false) const;
	void measure(gui::CellInfo &c) override;
};

class ViewForTab : public gui::View
{
public:
	class BaseClass : public gui::View
    {
        //ViewForTab _parent = nullptr;
		unsigned int _version = 1;
		std::unordered_set<td::String> vars, params;
		void setVariabesAndParams(std::unordered_set<td::String>&& vars, std::unordered_set<td::String>&& params);
		friend class ViewForTab;
	protected:
		void modelRolledBack() {--_version; }
		void setVersion(unsigned int newVersion) {_version = newVersion;}
	public:
		void modelChanged()
        {
            ++_version;
            setModified(true);
        }
        void setModified(bool bModified)
        {
            
            gui::Application* pApp = getApplication();
            gui::Window* pMainWnd = pApp->getInitialWindow();
            td::Variant var(bModified);
            pMainWnd->handleUserEvent((td::UINT4) UserEvent::SetModifiedFlag, var);
            
        }
		virtual bool save(const td::String& path, const td::String& settingsString) = 0;
		virtual void saveAs(const td::String& settingsString, td::String* newPath) = 0;
		virtual void getModel(ModelNode& model) = 0;
		virtual void refreshVisuals() = 0;
		virtual bool openFile(const td::String& path, td::String& settingsString) = 0;
		unsigned int getVersion() const { return _version; }
		const std::unordered_set<td::String>& getVars();
		const std::unordered_set<td::String>& getParams();
	};
private:
	std::shared_ptr<LogView> logView;
	ModelSettings settings;
    std::shared_ptr<Results> _results;
	std::vector<ModelSettings::PlotDesc> funcionsDesc;
	std::vector<ModelSettings::DependencyDesc> depenends;

	gui::SplitterLayout tabAndLogSplitter;
	gui::StandardTabView _tabView;
	gui::Image _logImg, _settingsImg, _resultsImg;
    
	td::String _path, _name;
	unsigned int _lastSavedModel = 0, _lastSavedSettings = 0, lastModelExtract = 0, lastSettingsVer = 0;
	ModelNode model, modelTab, emptyModel;
	bool includeGuard = false;

	void updateModelNode();
	void updateSettings();

	BaseClass* _simView = nullptr;
protected:
	ViewForTab();
	void init(BaseClass* simView, const td::String &defaultName, const td::String &path);
public:
	const std::shared_ptr<LogView> getLog();
    std::shared_ptr<Results> getResults();
    
	const BaseClass& getMainView();
    BaseClass* getContentView();
    void handleColorSettings();
	const td::String &getName();
	void setName(const td::String &name);
	bool loadFile(const td::String& path);
	void save();
	void saveAs();
	void exportToXML(td::String path);
	ModelSettings::SimulationSettings getSimulationSettings();
	void setPath(const td::String &path);
	const td::String& getPath();
	bool promptSaveIfNecessary(bool exitProgram);
	const ModelNode& getModelNode(bool &error, bool supressLogs = false);
	const std::vector<ModelSettings::PlotDesc>& getPlots();
	
    bool isModified() const
    {
        //IDz: ova metoda vise nije potrebna, moze se ici preko gui::TabView->isModified
        if (!_simView)
            return false;
        
        if ((_lastSavedSettings == settings.getVersion() && _lastSavedModel == _simView->getVersion()) || _simView->getVersion() == 0)
            return false;
        return true;
    }
    
	bool shouldClose() override;
	~ViewForTab();

};
