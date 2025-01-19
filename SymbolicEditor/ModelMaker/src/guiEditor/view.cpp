#include "view.h"
#include <gui/FileDialog.h>





GraphicalEditorView::GraphicalEditorView() : 
	spliter(gui::SplitterLayout::Orientation::Horizontal),
	_canvas(&props, this)
{
	spliter.setContent(_canvas, props);
	setLayout(&spliter);

}

bool GraphicalEditorView::save(const td::String& path, const td::String& settingsString)
{
	return _canvas.saveState(path, settingsString);
}

void GraphicalEditorView::saveAs(const td::String& settingsString, td::String* newPath)
{
	auto ptrSettings = new td::String(settingsString);
    td::UINT4 dlgID = 12345;
    auto pFD = gui::SaveFileDialog::show(this, tr("saveTF"), { {tr("tfFile"), "*.tfstate"}}, dlgID, [this, newPath, ptrSettings](gui::FileDialog *d) {
        if (td::String path = d->getFileName(); !path.isNull()) {
            if(_canvas.saveState(path, *ptrSettings))
                *newPath = path;
        }
        delete ptrSettings;
    });
    
//	gui::SaveFileDialog& saveDiag = *new gui::SaveFileDialog(this, tr("saveTF"), { {tr("tfFile"), "*.tfstate"}});
//	saveDiag.open([this, newPath, ptrSettings](gui::FileDialog *d) {
//		if (td::String path = d->getFileName(); !path.isNull()) {
//			if(_canvas.saveState(path, *ptrSettings))
//				*newPath = path;
//		}
//		delete ptrSettings;
//	});

}

void GraphicalEditorView::getModel(ModelNode& model)
{
	_canvas.getModel(model);
}

void GraphicalEditorView::refreshVisuals()
{
	props.showView(0, false);
	
}

bool GraphicalEditorView::openFile(const td::String& path, td::String& settingsString)
{
	this->modelChanged();
	return _canvas.restoreState(path, settingsString);
}





