#include "layTreeModel.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"

#include "layTreeItem.h"

namespace lay
{

namespace
{

//  Format a ruler's two points as "x1,y1,x2,y2" in database (GDS) units.
QString ruler_coord_string(const geo::Point& p1, const geo::Point& p2)
{
    return geo::to_qstring(geo::to_string(p1.x()) + "," + geo::to_string(p1.y()) + "," +
                          geo::to_string(p2.x()) + "," + geo::to_string(p2.y()));
}

} // namespace

TreeModel::TreeModel(QObject *parent) : QStandardItemModel(parent)
{
    setColumnCount(2);
    setHorizontalHeaderLabels({"Object", "Description"});

    auto item = new LayoutListItem();
    m_top = item;
    appendRow(m_top);
}

void TreeModel::removeAllLayoutItem()
{
    for (int row = m_top->rowCount()-1; row >=0; --row)
    {
        QStandardItem *item = m_top->child(row, 0);

        if (!item)
        {
            continue;
        }

        m_top->removeRow(row);
    }
}

void TreeModel::addLayoutItem(int index, const QString &s, const QString &path)
{
    auto item = new LayoutItem(s);
    item->setData(index);

    if (path.isEmpty())
    {
        m_top->appendRow(item);
    }
    else
    {
        //  show the file path in the second (Description) column
        auto path_item = new QStandardItem(path);
        path_item->setEditable(false);

        QList<QStandardItem*> row;
        row << item << path_item;

        m_top->appendRow(row);
    }

    m_layout_data.emplace(index, s);

}

QStandardItem *TreeModel::find_layout_item(int index)
{
    QStandardItem *find{0};
    for (int row = 0; row < m_top->rowCount(); ++row)
    {
        QStandardItem *item = m_top->child(row, 0);

        if (!item)
        {
            continue;
        }

        if (item->data().toInt() == index)
        {
            find = item;
        }
    }
    return find;
}

QStandardItem *TreeModel::addRulerListItem(QStandardItem *parent)
{
    for (int row = 0; row < parent->rowCount(); ++row)
    {
        QStandardItem *item = parent->child(row, 0);

        if (dynamic_cast<RulerListItem *>(item))
        {
            return item;
        }
    }

    auto item = new RulerListItem();
    parent->appendRow(item);
    return item;
}

void TreeModel::removeRulerListItem(QStandardItem *parent)
{
    for (int row = parent->rowCount()-1; row >=0; --row)
    {
        QStandardItem *item = parent->child(row, 0);
        parent->removeRow(row);
    }
}

void TreeModel::addRulerItem(QStandardItem *parent, int id, const geo::Point &p1, const geo::Point &p2)
{
    auto item = new RulerItem("R" + QString::number(id));
    item->setData(id);

    auto coord_str = ruler_coord_string(p1, p2);
    auto item1 = new QStandardItem(coord_str);

    QList<QStandardItem *> row;
    row << item << item1;

    parent->appendRow(row);
}

void TreeModel::updateRulerItem(QStandardItem *parent, int id, const geo::Point &p1, const geo::Point &p2)
{
    if (!parent)
    {
        return;
    }
    for (int row = 0; row < parent->rowCount(); ++row)
    {
        QStandardItem *item = parent->child(row, 0);
        if (!item)
        {
            continue;
        }
        if (item->data().toInt() == id)
        {
            QStandardItem *coord = parent->child(row, 1);
            if (coord)
            {
                coord->setText(ruler_coord_string(p1, p2));
            }
            return;
        }
    }
}

void TreeModel::syncRulerItems(QStandardItem *parent, const std::vector<RulerItemInfo> &rulers)
{
    if (!parent)
    {
        return;
    }

    //  first pass: collect the items currently present and update the coordinates of
    //  those which have a ruler (a plain move is handled by this pass alone)
    std::map<int, int> row_of_id;
    for (int row = 0; row < parent->rowCount(); ++row)
    {
        QStandardItem *item = parent->child(row, 0);
        if (item)
        {
            row_of_id[item->data().toInt()] = row;
        }
    }

    bool structure_changed = (row_of_id.size() != rulers.size());

    for (std::vector<RulerItemInfo>::const_iterator r = rulers.begin(); r != rulers.end(); ++r)
    {
        std::map<int, int>::const_iterator row = row_of_id.find(r->id);
        if (row == row_of_id.end())
        {
            structure_changed = true;
        }
        else
        {
            QStandardItem *coord = parent->child(row->second, 1);
            if (coord)
            {
                coord->setText(ruler_coord_string(r->p1, r->p2));
            }
        }
    }

    if (!structure_changed)
    {
        return;
    }

    //  The set of items to be shown changed - i.e. a ruler was added or removed, for
    //  example by an undone/redone "create ruler". Rebuild the list from the given
    //  rulers. This also restores the order of the shapes container, which may differ
    //  from the tree's order after undo/redo. A plain move does not get here, since
    //  its coordinates were already updated above.
    for (int row = parent->rowCount() - 1; row >= 0; --row)
    {
        parent->removeRow(row);
    }

    for (std::vector<RulerItemInfo>::const_iterator r = rulers.begin(); r != rulers.end(); ++r)
    {
        addRulerItem(parent, r->id, r->p1, r->p2);
    }
}

QStandardItem *TreeModel::addGotoListItem(QStandardItem *parent)
{
    for (int row = 0; row < parent->rowCount(); ++row)
    {
        QStandardItem *item = parent->child(row, 0);

        if (dynamic_cast<GotoListItem *>(item))
        {
            return item;
        }
    }

    auto item = new GotoListItem();
    parent->appendRow(item);
    return item;
}

void TreeModel::removeGotoListItem(QStandardItem *parent)
{
    for (int row = parent->rowCount()-1; row >=0; --row)
    {
        QStandardItem *item = parent->child(row, 0);
        parent->removeRow(row);
    }
}

void TreeModel::addGotoItem(QStandardItem *parent, int id, const geo::DPoint &p)
{
    auto item = new GotoItem("G" + QString::number(id));
    item->setData(id);

    auto coord_str = p.to_string();
    auto item1 = new QStandardItem(geo::to_qstring(coord_str));

    QList<QStandardItem *> row;
    row << item << item1;

    parent->appendRow(row);
}

void TreeModel::removeLayoutItem(int index)
{
    for (int row = 0; row < m_top->rowCount(); ++row)
    {
        QStandardItem *item = m_top->child(row, 0);

        if (!item)
        {
            continue;
        }

        if (item->data().toInt() == index)
        {
            m_top->removeRow(row);
            m_layout_data.erase(index);
        }
    }
}

void TreeModel::removeRulerItem(QStandardItem *parent, int index)
{
    for (int row = 0; row < parent->rowCount(); ++row)
    {
        QStandardItem *item = parent->child(row, 0);

        if (!item)
        {
            continue;
        }

        if (item->data().toInt() == index)
        {
            parent->removeRow(row);
        }
    }
}

void TreeModel::removeGotoItem(QStandardItem *parent, int index)
{
    for (int row = 0; row < parent->rowCount(); ++row)
    {
        QStandardItem *item = parent->child(row, 0);

        if (!item)
        {
            continue;
        }

        if (item->data().toInt() == index)
        {
            parent->removeRow(row);
        }
    }
}

void TreeModel::removeLayoutItem(QStandardItem *item)
{
    if (!item)
    {
        return;
    }

    if (QStandardItem *parent = item->parent())
    {
        parent->removeRow(item->row());
    }
    else
    {
        removeRow(item->row());
    }
}

QModelIndex TreeModel::topIndex() const
{
    return m_top->index();
}

QList<QStandardItem *> TreeModel::createRow(const QString &name, TreeNodeType type, const QVariant &nodeId) const
{
    auto *nameItem = new QStandardItem(name);
    auto *typeItem = new QStandardItem(nodeTypeText(type));

    nameItem->setEditable(true);
    typeItem->setEditable(false);

    nameItem->setData(static_cast<int>(type), NodeTypeRole);

    nameItem->setData(nodeId, NodeIdRole);

    typeItem->setData(static_cast<int>(type), NodeTypeRole);

    return {nameItem, typeItem};
}

QStandardItem *TreeModel::addRootNode(const QString &name, TreeNodeType type, const QVariant &nodeId)
{
    const QList<QStandardItem *> row = createRow(name, type, nodeId);

    appendRow(row);

    return row.first();
}

QStandardItem *TreeModel::addChildNode(QStandardItem *parentItem, const QString &name, TreeNodeType type,
                                       const QVariant &nodeId)
{
    if (!parentItem)
    {
        return nullptr;
    }

    if (parentItem->column() != 0)
    {
        parentItem = itemFromIndex(parentItem->index().siblingAtColumn(0));
    }

    const QList<QStandardItem *> row = createRow(name, type, nodeId);

    parentItem->appendRow(row);

    return row.first();
}

QStandardItem *TreeModel::itemFromModelIndex(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return nullptr;
    }

    const QModelIndex firstColumnIndex = index.siblingAtColumn(0);

    return itemFromIndex(firstColumnIndex);
}

bool TreeModel::removeNode(const QModelIndex &index)
{
    // QStandardItem* item = itemFromModelIndex(index);

    // if (!item) {
    //   return false;
    // }

    // QStandardItem* parentItem = item->parent();

    // if (parentItem) {
    //   return parentItem->removeRow(item->row());
    // }

    // return removeRow(item->row());

    return false;
}

bool TreeModel::renameNode(const QModelIndex &index, const QString &name)
{
    QStandardItem *item = itemFromModelIndex(index);

    if (!item)
    {
        return false;
    }

    item->setText(name);
    return true;
}

TreeNodeType TreeModel::nodeType(const QModelIndex &index) const
{
    QStandardItem *item = itemFromModelIndex(index);

    if (!item)
    {
        // return TreeNodeType::Folder;
    }

    return static_cast<TreeNodeType>(item->data(NodeTypeRole).toInt());
}

QVariant TreeModel::nodeId(const QModelIndex &index) const
{
    QStandardItem *item = itemFromModelIndex(index);

    return item ? item->data(NodeIdRole) : QVariant();
}

QStandardItem *TreeModel::findById(const QVariant &nodeId) const
{
    QStandardItem *root = invisibleRootItem();

    return findByIdRecursive(root, nodeId);
}

QStandardItem *TreeModel::findByIdRecursive(QStandardItem *parentItem, const QVariant &nodeId) const
{
    if (!parentItem)
    {
        return nullptr;
    }

    for (int row = 0; row < parentItem->rowCount(); ++row)
    {
        QStandardItem *child = parentItem->child(row, 0);

        if (!child)
        {
            continue;
        }

        if (child->data(NodeIdRole) == nodeId)
        {
            return child;
        }

        QStandardItem *result = findByIdRecursive(child, nodeId);

        if (result)
        {
            return result;
        }
    }

    return nullptr;
}

QString TreeModel::nodeTypeText(TreeNodeType type)
{
    // switch (type) {
    //   case TreeNodeType::Project:
    //     return tr("工程");

    //   case TreeNodeType::Folder:
    //     return tr("文件夹");

    //   case TreeNodeType::File:
    //     return tr("文件");
    // }

    return {};
}

} // namespace lay
