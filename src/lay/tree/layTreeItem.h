#pragma once

#include <QTreeWidget>
#include <QStandardItem>

namespace lay{

class ITreeItemService {
public:
    virtual ~ITreeItemService() = default;

    virtual void contextMenu(QMenu& menu){}
    virtual void onItemPress() {}
    virtual void onItemClick(int column) {}
    virtual void onDoubleClick(int column) {}
};

class TreeItemBase : public QStandardItem,
                     public ITreeItemService
{
public:
    TreeItemBase(const QString& s):
        QStandardItem(s),
        ITreeItemService()
    {

    }
    virtual ~TreeItemBase() = default;

};


class LayoutListItem : public TreeItemBase{
public:
    LayoutListItem();
    void contextMenu(QMenu& menu) override;
};

class RulerListItem : public TreeItemBase{
public:
    RulerListItem();
    void contextMenu(QMenu& menu) override;
};

class GotoListItem : public TreeItemBase{
public:
    GotoListItem();
    void contextMenu(QMenu& menu) override;
};

class LayoutItem : public TreeItemBase{
public:
    LayoutItem(const QString& name);
    void onItemClick(int column) override;
    void contextMenu(QMenu& menu) override;
};

class RulerItem : public TreeItemBase{
public:
    RulerItem(const QString& name);
    void onItemClick(int column) override;
    void onDoubleClick(int column) override;
    void contextMenu(QMenu& menu) override;
};

class GotoItem : public TreeItemBase{
public:
    GotoItem(const QString& name);
    void onItemClick(int column) override;
    void onDoubleClick(int column) override;
    void contextMenu(QMenu& menu) override;
};

}
