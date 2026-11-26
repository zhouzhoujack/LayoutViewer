#include "layTreeItem.h"
#include "detail/utilsString.h"

#include <QIcon>
#include <QObject>

#include "layGotoPlugin.h"
#include "layMainWindow.h"
#include "layResourceTree.h"
#include "layRulerObject.h"
#include "tlString.h"

namespace lay{

// -----------------------------------------------------------------------------
//  TreeItem implementation

LayoutListItem::LayoutListItem()
    :TreeItemBase("Layout Files")
{
    setIcon(QIcon(":/textdocumenticon.png"));
    setEditable(false);
}

void LayoutListItem::contextMenu(QMenu &menu)
{
    menu.addAction("Close All ", [this](){
        lay::MainWindow::instance()->close_all();
    });
}

RulerListItem::RulerListItem()
    :TreeItemBase("Ruler List")
{
    setIcon(QIcon(":/ruler.png"));
    setEditable(false);
}

void RulerListItem::contextMenu(QMenu &menu)
{
    auto p = QStandardItem::parent();
    auto layout_index = p->data().toInt();

    menu.addAction("Delete All", [layout_index, this](){
        lay::MainWindow::instance()->view(layout_index)->remove_all_rulers();
        lay::MainWindow::instance()->echo_command("ruler clear");
    });
}

GotoListItem::GotoListItem()
    :TreeItemBase("Goto List")
{
    setIcon(QIcon(":/green_flag.png"));
    setEditable(false);
}

void GotoListItem::contextMenu(QMenu &menu)
{
    auto p = QStandardItem::parent();
    auto layout_index = p->data().toInt();

    menu.addAction("Delete All", [layout_index, this](){
        lay::MainWindow::instance()->view(layout_index)->remove_all_gotos();
    });
}


LayoutItem::LayoutItem(const QString& name)
    :TreeItemBase(name)
{
    setIcon(QIcon(":/polygon.png"));
}

void LayoutItem::contextMenu(QMenu &menu)
{
    const int index = data().toInt();
    menu.addAction("Close", [index, this](){
        lay::MainWindow::instance()->close_view(index);
    });
}

void LayoutItem::onItemClick(int column){

}


RulerItem::RulerItem(const QString& name)
    :TreeItemBase(name)
{

}

void RulerItem::contextMenu(QMenu &menu)
{
    auto p = QStandardItem::parent();
    auto pp = p->parent();
    auto layout_index = pp->data().toInt();

    const int index = data().toInt();
    menu.addAction("Delete", [index, layout_index, this](){
        lay::MainWindow::instance()->view(layout_index)->remove_ruler(index);
        lay::MainWindow::instance()->echo_command("ruler remove " + geo::to_string(index));
    });
}

void RulerItem::onDoubleClick(int column)
{
    auto p = QStandardItem::parent();
    auto pp = p->parent();
    const int layout_index = pp->data().toInt();

    lay::LayoutView* view = lay::MainWindow::instance()->view(layout_index);
    const int ruler_id = data().toInt();

    //  delegate the "locate ruler" (pan / zoom) to the ruler plugin
    view->locate_ruler(ruler_id);
}

void RulerItem::onItemClick(int column){

}

GotoItem::GotoItem(const QString& name)
    :TreeItemBase(name)
{

}

void GotoItem::contextMenu(QMenu &menu)
{
    auto p = QStandardItem::parent();
    auto pp = p->parent();
    auto layout_index = pp->data().toInt();

    const int index = data().toInt();
    menu.addAction("Delete", [index, layout_index, this](){
        lay::MainWindow::instance()->view(layout_index)->remove_goto(index);
    });
}

void GotoItem::onItemClick(int column){

}

void GotoItem::onDoubleClick(int column)
{
    auto p = QStandardItem::parent();
    if (!p)
    {
        return;
    }

    auto pp = p->parent();
    if (!pp)
    {
        return;
    }

    const int layout_index = pp->data().toInt();
    lay::LayoutView* view = lay::MainWindow::instance()->view(layout_index);
    if (!view)
    {
        return;
    }

    const int goto_id = data().toInt();
    lay::GotoPlugin* plugin = view->get_plugin<lay::GotoPlugin>();
    if (!plugin)
    {
        return;
    }

    for (auto it = plugin->markers().begin(); it != plugin->markers().end(); ++it)
    {
        const lay::GotoView* marker = *it;
        if (!marker || marker->id() != goto_id)
        {
            continue;
        }

        //  center on the marker and fit a window around it so the marker is observable
        const geo::DBox box = view->box();
        const double s = std::min(box.width(), box.height());
        view->goto_window(marker->position(), s);
        return;
    }
}

}

