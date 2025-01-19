#pragma once
#include <gui/Dialog.h>
#include <gui/VerticalLayout.h>

#include <gui/Label.h>
#include <cnt/StringBuilder.h>



class SaveDlg: public gui::Dialog{
	gui::View _mainView;
	gui::Label _lblSave;
	gui::VerticalLayout _vl;
public:
	SaveDlg(ViewForTab *parent):
		gui::Dialog(parent->getParentFrame(), {
	{gui::Dialog::Button::ID::OK, tr("Save"), gui::Button::Type::Default},
	{gui::Dialog::Button::ID::Cancel, tr("Cancel"), gui::Button::Type::Normal},
	{gui::Dialog::Button::ID::Delete, tr("DontSave"), gui::Button::Type::Destructive}
	}, gui::Size(600,1000), (td::UINT4)MainWindow::DialogID::SaveData)
    , _lblSave("")
    , _vl(2)
	{
		cnt::StringBuilderSmall str;
		str << "\t" << tr("Model") << " \"" << parent->getName() << "\" " << tr("NotSavedAlert") << "\t";
		td::String text;
		str.getString(text);
        _lblSave.setTitle(text);

		gui::Size size;
		_lblSave.getMinSize(size);
		size.height *= 2.2;
		_lblSave.setPreferedContentSize(size);

		_vl << _lblSave;
		_mainView.setLayout(&_vl);
		setTitle(tr("saveDialogTitle"));
		setCentralView(&_mainView);
	}

	SaveDlg(Window *parent)
    : gui::Dialog(parent, {{gui::Dialog::Button::ID::OK, tr("Save"), gui::Button::Type::Default},
                              {gui::Dialog::Button::ID::Cancel, tr("Cancel"), gui::Button::Type::Normal},
                              {gui::Dialog::Button::ID::Delete, tr("DontSave"), gui::Button::Type::Destructive}},
                    gui::Size(600,1000), (td::UINT4)MainWindow::DialogID::SaveData)
    , _lblSave(tr("SomeUnsaved"))
    , _vl(2)
	{
		gui::Size size;
		_lblSave.getMinSize(size);
		size.height *= 2.2;
		_lblSave.setPreferedContentSize(size);

		_vl << _lblSave;
		_mainView.setLayout(&_vl);
		setTitle(tr("saveDialogTitle"));
		setCentralView(&_mainView);
	}
};
