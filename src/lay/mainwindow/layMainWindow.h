#pragma once

#include <QAction>
#include "detail/utilsObject.h"
#include "detail/utilsDeferredExecution.h"
#include "detail/utilsProgress.h"
#include "detail/utilsEvents.h"
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolBar>
#include <QInputDialog>

#include "detail/geoManager.h"

#include "layCommandPanel.h"
#include "layLayoutView.h"
#include "layTextProgressDelegate.h"
#include "layDispatcher.h"

#include "tlDeferredExecution.h"
#include "tlObject.h"

namespace lay
{

class SettingsForm;
class ViewWidgetStack;
class ControlWidgetStack;
class ProgressReporter;
class ProgressWidget;
class ProgressDialog;
class FileDialog;
class AbstractMenu;
class SaveLayoutAsOptionsDialog;
class SaveLayoutOptionsDialog;
class LoadLayoutOptionsDialog;
class LogViewerDialog;
class CellView;
class Navigator;
class ResourceTree;
class LayerToolbox;
class HelpDialog;
class HelpAboutDialog;
class ViewWidgetStack;
class ProgressWidget;

class MainWindow : public QMainWindow, public lay::DispatcherDelegate, public geo::RefCounted
{
    Q_OBJECT

public:
    MainWindow(QApplication* app, const char* name = "main_window", bool undo_enabled = true);

    virtual ~MainWindow()
    {
    }

    static MainWindow* instance();

    geo::Manager& manager()
    {
        return m_manager;
    }

    lay::Dispatcher* dispatcher() const
    {
        return const_cast<lay::Dispatcher*>(&m_dispatcher);
    }

    ResourceTree* resource_tree() const { return m_tree; }

    lay::CommandPanel* command_panel() const
    {
        return mp_command_panel;
    }

    /**
     *  @brief Enable or disable echoing of external operations into the command panel
     *
     *  The command panel temporarily disables echoing while it executes a command
     *  that funnels through menu_activated to avoid duplicate display.
     */
    void set_command_echo(bool on)
    {
        m_command_echo = on;
    }

    /**
     *  @brief Echo an external operation into the command panel
     *
     *  The command is appended to the transcript if echoing is enabled (i.e. the
     *  operation was not initiated from the command panel itself).
     */
    void echo_command(const std::string& command)
    {
        if (m_command_echo && mp_command_panel)
        {
            mp_command_panel->echo(command);
        }
    }

    AbstractMenu* menu()
    {
        // return m_dispatcher.menu();
        return 0;
    }

    void menu_activated (const std::string &symbol);

    void enter_busy_mode(bool bm)
    {
        m_busy = bm;
    }

    bool is_single_cv_layer_properties_file(const std::string& fn);


    int index_of (const lay::LayoutView *view) const;
    lay::LayoutView* current_view() const;
    lay::LayoutView* view(int index);
    unsigned int views() const
    {
        return (unsigned int)mp_views.size();
    }

    bool configure (const std::string &name, const std::string &value);

    void set_synchronous(bool sync_mode);
    void add_mru(const std::string& fn);
    void add_mru(const std::string& fn, const std::string& tech);

    int create_view();
    void about_to_exec();

    bool is_busy() const
    {
        return m_busy;
    }

    void restore_session(const std::string& fn);

    lay::CellViewRef load_layout(const std::string& filename, int mode = 0)
    {
        return load_layout(filename, m_initial_technology, mode);
    }
    lay::CellViewRef load_layout(const std::string& filename, const std::string& technology,
                                 int mode = 0)
    {
        return load_layout(
            filename,
            geo::Technologies::instance()->technology_by_name(technology)->load_layout_options(),
            technology, mode);
    }

    void load_layer_properties(const std::string& fn, int cv_index, bool all_views,
                               bool add_default);
    void load_layer_properties(const std::string& fn, bool all_views, bool add_default);
    lay::CellViewRef load_layout(const std::string& filename, const geo::LoadLayoutOptions& options,
                                 const std::string& technology, int mode = 0);

    void select_view(int index);
    void close_view(int index);
    void close_all();

    void tech_message(const std::string& s);

    bool update_progress(geo::Progress* progress);
    bool progress_wants_widget() const;
    void progress_add_widget(QWidget* widget);
    QWidget* progress_get_widget() const;
    void progress_remove_widget();
    bool show_progress_bar(bool show);

    void interactive_close_view(int index, bool all_cellviews);

    geo::Event current_view_changed_event;
    geo::event<int> view_closed_event;
    geo::event<int> view_created_event;

    bool check_exist_file(std::string& f);
    //  find the tab index of the view holding the given file, or -1 if none
    int view_index_for_file(const std::string& fn) const;
    //  get the file name associated with a view (empty if none / invalid)
    std::string view_filename(int index) const;
    void open_recent (size_t n);
    void do_update_mru_menus ();

signals:
    void file_open_signal(int index);

public slots:
    void dock_widget_visibility_changed (bool visible);
    void current_pos (double x, double y, bool is_dbu);
    void update_window_title ();
    void select_mode(int m);
    void view_selected(int index);
    void tab_close_requested(int);

private:
    void cm_setup();
    void cm_open();
    void cm_goto_position();
    void cm_undo ();
    void cm_redo ();

    void init_menu ();
    void build_menu_item (const std::vector<lay::MenuEntry>& menu_entries);

    void update_tab_title(int i);
    void add_view(LayoutView* view);
    int do_create_view();

    void setup_panel_menus();
    void setup_dock_toggle(const std::string& menu_path, QDockWidget* dock);

    void setup_status_bar();
    void update_status_bar();
    void on_cellview_changed(int);
    void setup_dock();
    void setup_tool_bar();
    void setup_resource_tree();
    void setup_central_widget();
    lay::CellViewRef create_or_load_layout(const std::string* filename,
                                           const geo::LoadLayoutOptions* options,
                                           const std::string& tech, const int mode);

    // center view widgets
    ViewWidgetStack* mp_view_stack;
    TextProgressDelegate m_text_progress;

    // progress reporting
    lay::ProgressReporter* mp_pr;
    lay::ProgressDialog* mp_progress_dialog;
    QFrame* mp_main_frame;
    QTabBar* mp_tab_bar;
    ProgressWidget* mp_progress_widget;
    QStackedWidget* mp_main_stack_widget;

    // layers and cells
    QDockWidget* mp_layer_cell_dock;
    QDockWidget *mp_hp_dock_widget;
    ControlWidgetStack *mp_hp_stack;
    QDockWidget* mp_lp_dock_widget;
    QDockWidget *mp_layer_toolbox_dock_widget;
    lay::LayerToolbox *mp_layer_toolbox;
    ControlWidgetStack* mp_lp_stack;

    // command dock
    QDockWidget* mp_command_dock;
    lay::CommandPanel* mp_command_panel{0};
    bool m_command_echo{true};

    // status bar
    QStatusBar* mp_status_bar;
    QFrame* mp_dbu_frame;
    QFrame* mp_cell_frame;
    QFrame* mp_coord_frame;
    QLabel* mp_cpx_label;
    QLabel* mp_cpy_label;
    QLabel* mp_grid_size_label;
    QLabel* mp_cur_cell_label;

    SettingsForm* mp_setup_form;

    lay::Dispatcher m_dispatcher;
    std::vector<std::pair<std::string, std::string>> m_mru;
    std::vector<lay::LayoutView*> mp_views;
    std::map<int, lay::CellViewRef> mp_cell_views;

    int m_mode{-1};
    bool m_busy;
    bool m_synchronous;

    AbstractMenu m_menu;
    geo::DeferredMethod<MainWindow> dm_do_update_mru_menus;

    geo::Manager m_manager;
    std::string m_initial_technology;

    QDockWidget* m_tree_dock{0};
    ResourceTree* m_tree{0};

    FileDialog* mp_file_dlg;
    QToolBar* mp_tool_bar{0};
};

} // namespace lay
