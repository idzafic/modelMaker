#include "Results.h"

Results::Results()
: _layout(1)
, _table(td::Ownership::Intern, gui::TableEdit::RowNumberVisibility::VisibleZeroBased)
{
    _layout << _table;
	setLayout(&_layout);
}

dp::IDataSet* Results::getDataSet()
{
    return _table.getDataSet();
}

//void Results::refresh()
//{
//    _table.reload();
//    _table.selectRow(0, true);
//}

void Results::show(dp::IDataSet* pDS)
{
    pDS->onRemoveAll([this](dp::IDataSet* pDS)
    {
        _table.clean();
    });

    dp::IDataSet* currDataSet = getDataSet();
    if (currDataSet == nullptr || currDataSet != pDS)
    {
        _table.init(pDS, 0);
        const auto& userInfo = pDS->getUserInfo();
        if (userInfo.lu8Val() == 0) //non-time domain, firs column is "eps" set it to scientific format
            _table.setColumnNumericFormat(0, td::FormatFloat::Scientific);
    }
    else //if (currDataSet == pDS)
    {
        _table.reload();
    }
    
}
