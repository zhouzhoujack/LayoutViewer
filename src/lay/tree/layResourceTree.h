#pragma once

#include <QObject>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
#include <QWidget>

#include "layTreeModel.h"

namespace lay
{

class LayoutView;
class RulerObject;

class ResourceTree : public QWidget
{
    Q_OBJECT
public:
    using const_iter = TreeModel::const_iter;

    explicit ResourceTree(QWidget* parent = nullptr);

    static ResourceTree* instance();

    void remove_layout_item(int index);
    void remove_all_layout_items();

    void remove_ruler_item(int index);
    void add_ruler_item(int id, const geo::Point& p1, const geo::Point& p2);
    void update_ruler_item(int id, const geo::Point& p1, const geo::Point& p2);
    void remove_all_ruler_items();

    /**
     *  @brief Make the "Ruler List" node of the current layout match the given rulers
     *
     *  Used to keep the tree in sync with the annotations container, in particular
     *  after undo/redo operations, which modify the container without any plugin
     *  interaction.
     */
    void sync_ruler_items(const std::vector<RulerItemInfo>& rulers);

    void remove_goto_item(int index);
    void add_goto_item(int id, const geo::DPoint& p);
    void remove_all_goto_items();

    //  Expand an item (show only its direct children, child expand state is preserved)
    void expandItem(const QModelIndex& index);

    //  Collapse an item (and all its children) in the tree view
    void collapseItem(const QModelIndex& index);

    TreeModel* tree_model() { return m_model; }

    const_iter begin() const
    {
        return m_model->begin();
    }

    const_iter end() const
    {
        return m_model->end();
    }

    // void clear();
    // QVariant currentNodeId() const;

signals:

public slots:
    void addLayoutItem(int index);
    void showContextMenu(const QPoint& pos);

private:
    void onItemDoubleClicked(const QModelIndex& index);
    void collapse_other_layout_items(QStandardItem* layout_item);

    QStandardItem* current_layout_item();
    QStandardItem* current_rulerlist_item();
    QStandardItem* current_gotolist_item();

    QTreeView* m_view = nullptr;
    TreeModel* m_model = nullptr;
};


}
