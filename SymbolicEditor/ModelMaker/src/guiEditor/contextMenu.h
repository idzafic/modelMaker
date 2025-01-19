#include <gui/ContextMenu.h>
#include <gui/ContextMenus.h>


class contextMenuModel : public gui::ContextMenu{
private:
	gui::SubMenu sub;
public:
	contextMenuModel(): sub(100, "cm1", 5)
	{
		auto& items = sub.getItems();
		items[0].initAsActionItem(tr("newTFBlock"), 10);
		items[1].initAsActionItem(tr("newSumBlock"), 11);
		items[2].initAsActionItem(tr("newNonlinearBlock"), 12);
		items[3].initAsActionItem(tr("newGainBlock"), 13);
		items[4].initAsActionItem(tr("newOutputBlock"), 14);

		setItem(&sub);
	}
};


class ContextMenus : public gui::ContextMenus{
protected:
	contextMenuModel m1;
public:
	ContextMenus(): gui::ContextMenus(1){
		setItem(0, &m1);
	}
};