#include "layResourceTree.h"
#include "detail/utilsInternational.h"

#include <QFileInfo>
#include <QHeaderView>
#include <QVBoxLayout>

#include "layTreeItem.h"

#include "layLayoutView.h"
#include "layMainWindow.h"
#include "tlInternational.h"
#include "layRulerObject.h"

namespace lay{

static ResourceTree* s_inst=0;

// -----------------------------------------------------------------------------
//  ResourceTree implementation

ResourceTree::ResourceTree(QWidget* parent)
    : QWidget(parent), m_view(new QTreeView(this)), m_model(new TreeModel(this))
{
    m_view->setModel(m_model);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_view->setColumnWidth(0, 160);
    m_view->setColumnWidth(1, 100);

    connect(m_view, &QTreeView::customContextMenuRequested, this, &ResourceTree::showContextMenu);
    connect(m_view, &QTreeView::doubleClicked, this, &ResourceTree::onItemDoubleClicked);

    m_view->setUniformRowHeights(true);
    m_view->setAnimated(true);
    m_view->setIndentation(16);
    m_view->setIconSize(QSize(18, 18));
    // m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    //  the double-click is handled in onItemDoubleClicked - disable the built-in
    //  toggle (default is true) so it cannot collapse the item again afterwards
    m_view->setExpandsOnDoubleClick(false);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_view->header()->setStretchLastSection(true);
    m_view->header()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_view->setStyleSheet(
        "QTreeView {"
        "    background-color: #ffffff;"
        "    border: none;"
        "    outline: 0;"
        "    font-size: 10pt;"
        "    show-decoration-selected: 1;"
        "}"
        "QTreeView::item {"
        "    height: 28px;"
        "    margin: 1px 3px;"
        "    padding-left: 4px;"
        "    padding-right: 8px;"
        "    border-radius: 6px;"
        "    color: #2c3e50;"
        "    background-color: transparent;"
        "}"
        "QTreeView::item:hover {"
        "    background-color: #eaf0f8;"
        "}"
        "QTreeView::item:selected {"
        "    background-color: #cce4ff;"
        "    color: #123c74;"
        "}"
        "QTreeView::item:selected:active {"
        "    background-color: #cce4ff;"
        "}"
        "QHeaderView::section {"
        "    background-color: #ffffff;"
        "    color: #5a6572;"
        "    border: none;"
        "    border-bottom: 1px solid #d8dfe8;"
        "    border-right: 1px solid #e3e8ef;"
        "    padding: 6px 8px;"
        "    font-size: 9pt;"
        "    font-weight: 600;"
        "}"
        "QScrollBar:vertical {"
        "    background: transparent;"
        "    width: 10px;"
        "    margin: 0;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #c3ccd8;"
        "    min-height: 30px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background: #a9b4c2;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: transparent;"
        "}"
        );

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);

    // connect(m_controller, &ResourceTreeController::resourceActivated, this,
    //         &ResourceTree::resourceActivated);

    // connect(m_controller, &ResourceTreeController::resourceRemoved, this,
    //         &ResourceTree::resourceRemoved);

    // connect(m_controller, &ResourceTreeController::resourceRenamed, this,
    //         &ResourceTree::resourceRenamed);

    // connect(m_view->selectionModel(), &QItemSelectionModel::currentChanged, this,
    //         [this](const QModelIndex& current, const QModelIndex&) {
    //           if (!current.isValid()) {
    //             emit currentResourceChanged({});
    //             return;
    //           }

    //           emit currentResourceChanged(
    //               m_model->nodeId(current.siblingAtColumn(0)));
    //         });
    s_inst = this;
}

ResourceTree *ResourceTree::instance()
{
    return s_inst;
}

void ResourceTree::remove_layout_item(int index)
{
    m_model->removeLayoutItem(index);
}

void ResourceTree::remove_all_layout_items()
{
    m_model->removeAllLayoutItem();
}

void ResourceTree::remove_ruler_item(int index)
{
    auto current = current_rulerlist_item();
    m_model->removeRulerItem(current, index);
}

void ResourceTree::remove_all_ruler_items()
{
    //  remove the whole "Ruler List" node (and all rulers within it)
    auto current = current_rulerlist_item();
    if (current && current->parent())
    {
        m_model->removeRulerListItem(current);
    }
}

void ResourceTree::add_ruler_item(int id, const geo::Point& p1, const geo::Point& p2)
{
    auto current = current_layout_item();
    auto rule_list = m_model->addRulerListItem(current);
    m_model->addRulerItem(rule_list, id, p1, p2);
    m_view->expand(current->index());
}

void ResourceTree::sync_ruler_items(const std::vector<RulerItemInfo>& rulers)
{
    if (lay::MainWindow::instance() == 0)
    {
        return;
    }

    auto layout = current_layout_item();
    if (!layout)
    {
        return;
    }

    auto ruler_list = current_rulerlist_item();

    if (rulers.empty())
    {
        //  don't create an empty "Ruler List" node - just drop the items
        if (ruler_list)
        {
            m_model->removeRulerListItem(ruler_list);
        }
        return;
    }

    if (!ruler_list)
    {
        ruler_list = m_model->addRulerListItem(layout);
        m_view->expand(layout->index());
    }

    m_model->syncRulerItems(ruler_list, rulers);
}

void ResourceTree::update_ruler_item(int id, const geo::Point& p1, const geo::Point& p2)
{
    auto current = current_rulerlist_item();
    m_model->updateRulerItem(current, id, p1, p2);
}

void ResourceTree::remove_goto_item(int index)
{
    auto current = current_gotolist_item();
    m_model->removeGotoItem(current, index);
}

void ResourceTree::add_goto_item(int id, const geo::DPoint &p)
{
    auto current = current_layout_item();
    if (!current)
    {
        return;
    }

    auto goto_list = m_model->addGotoListItem(current);
    m_model->addGotoItem(goto_list, id, p);
    m_view->expand(current->index());
}

void ResourceTree::remove_all_goto_items()
{
    auto current = current_gotolist_item();
    if (!current)
    {
        return;
    }

    m_model->removeGotoListItem(current);
}

void ResourceTree::addLayoutItem(int index)
{
    lay::LayoutView* view = lay::MainWindow::instance()->view(index);

    //  first column: the file name
    std::string title = view->title();

    //  second column: the path without the file name (directory)
    std::string path;
    if (view->cellviews() > 0)
    {
        int cv_index = view->active_cellview_index();
        if (cv_index < 0 || cv_index >= int(view->cellviews()))
        {
            cv_index = 0;
        }
        path = view->cellview(cv_index)->filename();
    }
    QString dir = QFileInfo(geo::to_qstring(path)).path();

    m_model->addLayoutItem(index, geo::to_qstring(title), dir);
    m_view->expand(m_model->topIndex());
}

void ResourceTree::showContextMenu(const QPoint& pos)
{
    const QModelIndex index = m_view->indexAt(pos);

    if (index.isValid())
    {
        m_view->setCurrentIndex(index);
    }

    QModelIndex nodeIndex = index.siblingAtColumn(0);
    QStandardItem* item = m_model->itemFromIndex(nodeIndex);

    TreeItemBase* base = dynamic_cast<TreeItemBase*>(item);
    if (!base)
        return;

    QMenu menu;
    base->contextMenu(menu);
    menu.exec(m_view->viewport()->mapToGlobal(pos));
}

void ResourceTree::onItemDoubleClicked(const QModelIndex& index)
{
    QModelIndex nodeIndex = index.siblingAtColumn(0);
    QStandardItem* item = m_model->itemFromIndex(nodeIndex);

    //  the layout item is handled by the tree itself: activate the layout,
    //  expand it and collapse the other layout items.
    if (LayoutItem* layout_item = dynamic_cast<LayoutItem*>(item))
    {
        const int layout_index = layout_item->data().toInt();
        lay::MainWindow::instance()->select_view(layout_index);
        expandItem(nodeIndex);
        collapse_other_layout_items(layout_item);
        return;
    }

    //  other item types get the chance to react through their virtual handler
    TreeItemBase* base = dynamic_cast<TreeItemBase*>(item);
    if (base)
    {
        base->onDoubleClick(nodeIndex.column());
    }
}

void ResourceTree::collapse_other_layout_items(QStandardItem* layout_item)
{
    QStandardItem* parent_item = layout_item->parent();
    if (!parent_item)
    {
        return;
    }

    for (int row = 0; row < parent_item->rowCount(); ++row)
    {
        QStandardItem* item = parent_item->child(row);
        if (item && item != layout_item)
        {
            collapseItem(item->index());
        }
    }
}

void ResourceTree::expandItem(const QModelIndex& index)
{
    if (index.isValid())
    {
        //  only reveal the direct children - do not expand the children,
        //  their own expand/collapse state is preserved.
        m_view->expand(index);
    }
}

void ResourceTree::collapseItem(const QModelIndex& index)
{
    if (index.isValid())
    {
        m_view->collapse(index);
    }
}

QStandardItem *ResourceTree::current_layout_item()
{
    lay::LayoutView* view = lay::MainWindow::instance()->current_view();
    int index = lay::MainWindow::instance()->index_of(view);
    return m_model->find_layout_item(index);
}

QStandardItem *ResourceTree::current_rulerlist_item()
{
    QStandardItem * rulerlist{0};
    auto current = current_layout_item();
    for(int i=0; i < current->rowCount(); ++i){
        QStandardItem * item = current->child(i);
        if (dynamic_cast<RulerListItem *>(item))
        {
            rulerlist = item;
            break;
        }
    }
    return rulerlist;
}

QStandardItem *ResourceTree::current_gotolist_item()
{
    QStandardItem * gotolist{0};
    auto current = current_layout_item();
    for(int i=0; i < current->rowCount(); ++i){
        QStandardItem * item = current->child(i);
        if (dynamic_cast<GotoListItem *>(item))
        {
            gotolist = item;
            break;
        }
    }
    return gotolist;
}

// void ResourceTree::clear() { m_model->clearResources(); }

// QModelIndex ResourceTree::currentNodeIndex() const {
//   const QModelIndex index = m_view->currentIndex();

//   if (!index.isValid()) {
//     return {};
//   }

//   return index.siblingAtColumn(0);
// }

// QVariant ResourceTree::currentNodeId() const {
//   return m_model->nodeId(currentNodeIndex());
// }

// QString ResourceTree::currentResourcePath() const {
//   return m_model->resourcePath(currentNodeIndex());
// }

// ResourceNodeType ResourceTree::currentNodeType() const {
//   return m_model->nodeType(currentNodeIndex());
// }

// bool ResourceTree::selectNode(const QVariant& id) {
//   QStandardItem* item = m_model->findById(id);

//   if (!item) {
//     return false;
//   }

//   QModelIndex index = item->index();

//   QModelIndex parentIndex = index.parent();

//   while (parentIndex.isValid()) {
//     m_view->expand(parentIndex);
//     parentIndex = parentIndex.parent();
//   }

//   m_view->setCurrentIndex(index);
//   m_view->scrollTo(index);

//   return true;
// }

// bool ResourceTree::addResource(const QVariant& parentId,
//                                const ResourceData& resource) {
//   QStandardItem* parentItem = m_model->findById(parentId);

//   if (!parentItem) {
//     return false;
//   }

//   QStandardItem* item = m_model->addChildResource(parentItem, resource);

//   if (!item) {
//     return false;
//   }

//   m_view->expand(parentItem->index());

//   return true;
// }

// bool ResourceTree::removeResource(const QVariant& id) {
//   QStandardItem* item = m_model->findById(id);

//   if (!item) {
//     return false;
//   }

//   return m_model->removeResource(item->index());
// }


}
