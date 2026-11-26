#pragma once

#include <QStandardItemModel>

#include <map>
#include <vector>

#include "detail/geoPoint.h"

namespace lay{

enum class TreeNodeType
{
    LayoutList
};

/**
 *  @brief A ruler to be created or updated in the "Ruler List" node
 *
 *  The coordinates are given in database (GDS) units - the representation used in the tree.
 */
struct RulerItemInfo
{
    int id;
    geo::Point p1;
    geo::Point p2;
};

class TreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum DataRole
    {
        NodeTypeRole = Qt::UserRole + 1,
        NodeIdRole,
        FilePathRole
    };

    using LayoutMap = std::map<int, QString>;
    using const_iter = LayoutMap::const_iterator;

    const_iter begin() const
    {
        return m_layout_data.begin();
    }

    const_iter end() const
    {
        return m_layout_data.end();
    }

    explicit TreeModel(QObject* parent = nullptr);

    void addLayoutItem(int index, const QString& s, const QString& path = QString());
    QStandardItem* find_layout_item(int index);

    QStandardItem* addRulerListItem(QStandardItem* parent);
    void removeRulerListItem(QStandardItem* parent);

    void addRulerItem(QStandardItem* parent, int id, const geo::Point& p1, const geo::Point& p2);
    void updateRulerItem(QStandardItem* parent, int id, const geo::Point& p1, const geo::Point& p2);

    /**
     *  @brief Make the ruler items below the given node match the given list of rulers
     *
     *  Rulers which are not in the list anymore get their item removed, rulers
     *  which do not have an item yet get one added and existing items get their
     *  coordinates updated.
     *
     *  This keeps the tree in sync with the annotation shapes container - also when
     *  that container is modified by undo/redo, where no plugin code is involved.
     */
    void syncRulerItems(QStandardItem* parent, const std::vector<RulerItemInfo>& rulers);

    QStandardItem* addGotoListItem(QStandardItem* parent);
    void removeGotoListItem(QStandardItem* parent);
    void addGotoItem(QStandardItem* parent, int id, const geo::DPoint& p);


    QModelIndex topIndex() const;

    QStandardItem* addRootNode(const QString& name, TreeNodeType type, const QVariant& nodeId = {});

    QStandardItem* addChildNode(QStandardItem* parentItem, const QString& name, TreeNodeType type,
                                const QVariant& nodeId = {});

    QStandardItem* itemFromModelIndex(const QModelIndex& index) const;

    QStandardItem* findById(const QVariant& nodeId) const;

    bool removeNode(const QModelIndex& index);

    bool renameNode(const QModelIndex& index, const QString& name);

    TreeNodeType nodeType(const QModelIndex& index) const;

    QVariant nodeId(const QModelIndex& index) const;

public slots:
    void removeAllLayoutItem();
    void removeRulerItem(QStandardItem* parent, int index);
    void removeGotoItem(QStandardItem* parent, int index);
    void removeLayoutItem(int index);
    void removeLayoutItem(QStandardItem *item);

private:
    QList<QStandardItem*> createRow(const QString& name, TreeNodeType type,
                                    const QVariant& nodeId) const;

    QStandardItem* findByIdRecursive(QStandardItem* parentItem, const QVariant& nodeId) const;

    static QString nodeTypeText(TreeNodeType type);

    LayoutMap m_layout_data;
    QStandardItem* m_top;
};

}
