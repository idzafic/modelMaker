#include "View.h"
#include "../GlobalEvents.h"
#include "../MainWindow.h"
#include <gui/FileDialog.h>
#include <fstream>

TextEditorView::TextEditorView() :
	_hl(1),
	textMain(this)
{
	_hl << textMain;
	setLayout(&_hl);
}

void TextEditorView::getModel(ModelNode &model)
{
	model.clear();
	model.processCommands(textMain.getText());

	_varsNotRoot.clear();
	_paramsNotRoot.clear();

	for (const auto& node : model.getNodes()) {
		if (std::strcmp(node->getName(), "Init") == 0) {
			for (const auto& subModel : node->getNodes()) {
				if (std::strcmp(subModel->getName(), "Model") == 0) {
					for (const auto& varOrParN : subModel->getNodes()) {
						
						
						if (std::strcmp(varOrParN->getName(), "Vars") == 0)
							for (const auto& vars : varOrParN->getNodes())
								_varsNotRoot.emplace((*vars)["name"]);


						if (std::strcmp(varOrParN->getName(), "Params") == 0)
							for (const auto& vars : varOrParN->getNodes())
								_paramsNotRoot.emplace((*vars)["name"]);


					}
				}
			}
		}

		if (std::strcmp(node->getName(), "Expressions") == 0) {
			td::String name;
			int poz;
			for (const auto& exprs : node->getNodes()) {
				name = (*exprs)["fx"];
				poz = name.find("(");
				if (poz != -1) {
					name = name.subStr(0, poz-1);
					_expressions.emplace(name.trimRight());
				}
			}
		}

	}

}

bool TextEditorView::save(const td::String &path, const td::String &settingsString)
{
	std::ofstream out(path.c_str());
	out << settingsString << "\n";
	out << textMain.getText();
	if(out.fail()){
		showAlert(tr("saveFile"), tr("cantSaveFile"));
		return false;
	}
    setModified(false);
	return true;
}

void TextEditorView::saveAs(const td::String &settingsString, td::String *newPath)
{

	//td::String *settingsStringPtr = new td::String(settingsString);
    td::UINT4 dlgID = 15345;    
    auto* pFD = gui::SaveFileDialog::showWithAttachment(this, tr("saveFile"), "*.modl", settingsString, dlgID, [newPath, this](gui::FileDialog *dialog)
    {
        auto path = dialog->getFileName();
        if(!path.isNull())
        {
            td::String settingsData;
            dialog->getAttachedValue(settingsData);
            if(save(path, settingsData))
                *newPath = path;
        }
    });
//	auto &s = *new gui::SaveFileDialog (this, tr("saveFile"), "*.modl");
//    s.attachValue(settingsString);
//	s.openModal([newPath, this](gui::FileDialog *dialog){
//		auto path = dialog->getFileName();
//		if(!path.isNull())
//        {
//            td::String settingsData;
//            dialog->getAttachedValue(settingsData);
//            if(save(path, settingsData))
//                *newPath = path;
//        }
//	});
}

bool TextEditorView::openFile(const td::String &path, td::String &settingsString)
{
//	modelChanged();
//    setModified(false);
    if(path.endsWith(".modl"))
    {
		std::ifstream in(path.c_str());
		cnt::StringBuilder s;
		char buffer[1000];

		if(!in)
			return false;

		const char *found;
		constexpr const char *modelTag = "Model";
		const int len = std::strlen(modelTag);
		settingsString = "";
		bool firstLoop = true;
		while(in)
        {
            //pronalazenje "model:" tag-a i stavljanje svog sadrzaja prije njega u settingsString
			in.read(buffer + len, sizeof(buffer) - len);
			found = std::search(buffer+len, buffer + len + in.gcount(), modelTag, modelTag+ len);
			if(found)
            {
				if(firstLoop)
					settingsString += td::String(buffer + len, found - buffer - len);
				else
					settingsString += td::String(buffer, found - buffer);

				s.appendString(found, in.gcount()+len-(found - buffer));
				break;
			}
			else
            {
				if(in.gcount() != sizeof(buffer) - len)
                {
					showAlert(tr("error"), tr("noModelTagFound"));
					if(firstLoop)
						settingsString += td::String(buffer + len, in.gcount());
					else
						settingsString += td::String(buffer, in.gcount() + len);

					return settingsString.find("preProc(") != -1;
				}
				if(firstLoop)
                {
					settingsString += td::String(buffer + len, sizeof(buffer) - 2*len);
					firstLoop = false;
				}
                else
					settingsString += td::String(buffer, sizeof(buffer) - len);
			}
			
			memmove(buffer, buffer + sizeof(buffer) - len, len);

		}

		while(in){//model: je pronaden sad se samo ocita ostatak fajla
			in.read(buffer, sizeof(buffer));
			s.appendString(buffer, int(in.gcount()));
		}

		td::String code;
		s.getString(code);

		textMain.setText(code);
		textMain.highlightSyntax();
        textMain.setFontSize(GlobalEvents::settingsVars.textSize);
		
		return true;
	}
	if(path.endsWith(".xml")){
		try {
			td::String s;
			GlobalEvents::getMainWindowPtr()->getModelFromTabOrFile(path).prettyPrint(s);
			textMain.setText(s);
			textMain.setFontSize(GlobalEvents::settingsVars.textSize);
			return true;
		}
		catch (MainWindow::exceptionCantAccessFile&) {
			showAlert(tr("error"), tr("cantLoadXML"));
		}
		catch (ModelNode::exceptionInvalidBlockName& name) {
			cnt::StringBuilderSmall s;
			s << "Unrecognized block \"" << name.message << "\" on line " << name.line << ", cant load model";
			td::String msg;
			s.getString(msg);
			showAlert(tr("error"), msg);
			return false;
		}
		catch (...) {
			return false;
		}
	}
	return false;
}


void TextEditorView::refreshVisuals()
{
    if (GlobalEvents::settingsVars.font.compare("Default", 7))
    {
        textMain.highlightSyntax();
    }
    else
    {
        textMain.setFontName(GlobalEvents::settingsVars.font);
        textMain.highlightSyntax();
    }
}
