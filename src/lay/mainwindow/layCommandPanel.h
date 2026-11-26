#pragma once

#include <QList>
#include <QWidget>

#include <string>
#include <vector>

class QLineEdit;
class QPlainTextEdit;

namespace lay
{

class MainWindow;
class LayoutView;

/**
 *  @brief The command line panel widget
 *
 *  This widget provides a command line for LayoutViewer. Commands can be typed
 *  into an input field (e.g. "open -o <file>"). The transcript shows all executed
 *  commands as well as external operations which are converted to commands
 *  automatically (see MainWindow::menu_activated).
 *
 *  The command history can be saved as a script and scripts can be loaded
 *  and executed.
 */
class CommandPanel : public QWidget
{
    Q_OBJECT

public:
    CommandPanel(QWidget* parent, MainWindow* mw);
    ~CommandPanel();

    /**
     *  @brief Displays an external operation as a command line
     *
     *  The command is appended to the transcript but not executed.
     */
    void echo(const std::string& command);

    /**
     *  @brief Displays a status or error message
     */
    void message(const std::string& text);

    /**
     *  @brief Executes a command line (as if typed by the user)
     */
    void execute(const std::string& line);

    /**
     *  @brief Clears the transcript
     */
    void clear_transcript();

    /**
     *  @brief Opens the "Save Script" dialog and saves the command history
     */
    void save_script_dialog();

    /**
     *  @brief Opens the "Load Script" dialog and runs the selected script
     */
    void load_script_dialog();

    /**
     *  @brief Shows the list of available commands in the transcript
     */
    void show_help();

    /**
     *  @brief Maps a menu symbol to a command string
     *
     *  Returns an empty string if the symbol has no command equivalent.
     */
    static std::string command_for_symbol(const std::string& symbol);

    /**
     *  @brief Quotes a string for use as a command argument
     */
    static std::string quote(const std::string& s);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
    QSize sizeHint() const override;

private slots:
    void run_clicked();
    void save_script_clicked();
    void load_script_clicked();
    void clear_clicked();
    void help_clicked();
    void input_return_pressed();
    void input_history_back();
    void input_history_forward();

private:
    void setup_ui();
    void append_command(const std::string& line);
    void append_message(const std::string& line);

    //  command dispatch
    bool dispatch(const std::vector<std::string>& argv);
    bool cmd_open(const std::vector<std::string>& args);
    bool cmd_close(const std::vector<std::string>& args);
    bool cmd_zoom(const std::vector<std::string>& args);
    bool cmd_pan(const std::vector<std::string>& args);
    bool cmd_goto(const std::vector<std::string>& args);
    bool cmd_ruler(const std::vector<std::string>& args);
    bool cmd_undo_redo(const std::vector<std::string>& args, bool is_undo);
    bool cmd_help(const std::vector<std::string>& args);
    bool cmd_echo(const std::vector<std::string>& args);
    bool cmd_clear(const std::vector<std::string>& args);
    bool cmd_save_script(const std::vector<std::string>& args);
    bool cmd_load_script(const std::vector<std::string>& args);

    void save_script(const std::string& fn);
    void load_script(const std::string& fn);

    std::vector<std::string> tokenize(const std::string& line) const;
    std::string expand_path(const std::string& p) const;
    double parse_number(const std::string& s) const;
    lay::LayoutView* current_view() const;

    MainWindow* mp_mw;
    QPlainTextEdit* mp_transcript;
    QLineEdit* mp_input;
    QList<QString> m_input_history;   //  input line history
    int m_input_history_pos;          //  -1 means "not navigating"
    std::vector<std::string> m_commands; //  command lines (used for scripts)
};

} // namespace lay
