#include "layCommandPanel.h"
#include "detail/utilsInternational.h"
#include "detail/utilsString.h"
#include "detail/utilsExpression.h"

#include "layLayoutView.h"
#include "layGotoPlugin.h"
#include "layMainWindow.h"
#include "layResourceTree.h"
#include "layRulerPlugin.h"

#include "detail/geoManager.h"
#include "detail/geoPoint.h"
#include "detail/geoTechnology.h"
#include "tlException.h"
#include "tlExpression.h"
#include "tlInternational.h"
#include "tlString.h"

#include <QEvent>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextCursor>
#include <QTextStream>
#include <QVBoxLayout>

#include <cctype>
#include <cstdlib>

namespace lay
{

// ---------------------------------------------------------------------------------------------
//  local helpers

static bool is_integer(const std::string& s)
{
    if (s.empty())
    {
        return false;
    }
    size_t i = 0;
    if (s[0] == '-')
    {
        i = 1;
        if (s.size() == 1)
        {
            return false;
        }
    }
    for (; i < s.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
        {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------
//  CommandPanel implementation

// -----------------------------------------------------------------------------
//  CommandPanel implementation

CommandPanel::CommandPanel(QWidget* parent, MainWindow* mw)
    : QWidget(parent), mp_mw(mw), mp_transcript(0), mp_input(0), m_input_history_pos(-1)
{
    setup_ui();
}

CommandPanel::~CommandPanel()
{
    //  .. nothing yet ..
}

QSize CommandPanel::sizeHint() const
{
    //  keep the panel compact (input row + a few transcript lines)
    return QSize(360, 120);
}

void CommandPanel::setup_ui()
{
    setObjectName(QString::fromUtf8("command_panel"));

    QVBoxLayout* vbl = new QVBoxLayout(this);
    vbl->setContentsMargins(2, 2, 2, 2);
    vbl->setSpacing(2);

    //  transcript (command history / output)
    mp_transcript = new QPlainTextEdit(this);
    mp_transcript->setObjectName(QString::fromUtf8("command_transcript"));
    mp_transcript->setReadOnly(true);
    mp_transcript->setMaximumBlockCount(2000);
    mp_transcript->setMinimumHeight(56);
    mp_transcript->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    vbl->addWidget(mp_transcript, 1);

    //  input line with buttons on the right
    QHBoxLayout* input_ly = new QHBoxLayout();
    input_ly->setContentsMargins(0, 0, 0, 0);
    input_ly->setSpacing(2);

    mp_input = new QLineEdit(this);
    mp_input->setObjectName(QString::fromUtf8("command_input"));
    mp_input->setPlaceholderText(tr("Enter command, e.g. open -o \"~/test.oas\""));
    mp_input->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    mp_input->installEventFilter(this);
    connect(mp_input, &QLineEdit::returnPressed, this, &CommandPanel::input_return_pressed);
    input_ly->addWidget(mp_input, 1);

    QPushButton* run_btn = new QPushButton(QIcon(QString::fromUtf8(":/run.png")), QString(), this);
    run_btn->setToolTip(tr("Run the command in the input field"));
    run_btn->setIconSize(QSize(16, 16));
    input_ly->addWidget(run_btn);
    connect(run_btn, &QPushButton::clicked, this, &CommandPanel::run_clicked);

    QPushButton* clear_btn = new QPushButton(QIcon(QString::fromUtf8(":/clear.png")), QString(), this);
    clear_btn->setToolTip(tr("Clear the transcript"));
    clear_btn->setIconSize(QSize(16, 16));
    input_ly->addWidget(clear_btn);
    connect(clear_btn, &QPushButton::clicked, this, &CommandPanel::clear_clicked);

    vbl->addLayout(input_ly);
}

bool CommandPanel::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == mp_input && ev->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(ev);
        if (ke->key() == Qt::Key_Up)
        {
            input_history_back();
            return true;
        }
        else if (ke->key() == Qt::Key_Down)
        {
            input_history_forward();
            return true;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void CommandPanel::append_command(const std::string& line)
{
    mp_transcript->appendPlainText(QString::fromUtf8(">> ") + geo::to_qstring(line));

    QTextCursor cur = mp_transcript->textCursor();
    cur.movePosition(QTextCursor::End);
    mp_transcript->setTextCursor(cur);
}

void CommandPanel::append_message(const std::string& line)
{
    mp_transcript->appendPlainText(geo::to_qstring(line));

    QTextCursor cur = mp_transcript->textCursor();
    cur.movePosition(QTextCursor::End);
    mp_transcript->setTextCursor(cur);
}

void CommandPanel::echo(const std::string& command)
{
    std::string c = tl::trim(command);
    if (c.empty())
    {
        return;
    }

    append_command(c);
    m_commands.push_back(c);
}

void CommandPanel::message(const std::string& text)
{
    append_message(text);
}

void CommandPanel::clear_transcript()
{
    mp_transcript->clear();
}

void CommandPanel::execute(const std::string& line)
{
    std::string l = tl::trim(line);
    if (l.empty() || l[0] == '#')
    {
        return;
    }

    append_command(l);
    m_commands.push_back(l);

    dispatch(tokenize(l));
}

void CommandPanel::run_clicked()
{
    input_return_pressed();
}

void CommandPanel::save_script_clicked()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save Script"),
                                              QDir::homePath() + QString::fromUtf8("/layout_script.cmd"),
                                              tr("Command Scripts (*.cmd *.txt);;All Files (*)"));
    if (!fn.isEmpty())
    {
        save_script(geo::to_string(fn));
    }
}

void CommandPanel::load_script_clicked()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Load Script"), QDir::homePath(),
                                              tr("Command Scripts (*.cmd *.txt);;All Files (*)"));
    if (!fn.isEmpty())
    {
        load_script(geo::to_string(fn));
    }
}

void CommandPanel::clear_clicked()
{
    clear_transcript();
}

void CommandPanel::help_clicked()
{
    cmd_help(std::vector<std::string>());
}

void CommandPanel::save_script_dialog()
{
    save_script_clicked();
}

void CommandPanel::load_script_dialog()
{
    load_script_clicked();
}

void CommandPanel::show_help()
{
    help_clicked();
}

void CommandPanel::input_return_pressed()
{
    QString text = mp_input->text().trimmed();
    if (text.isEmpty())
    {
        return;
    }

    m_input_history.append(text);
    m_input_history_pos = -1;
    mp_input->clear();

    execute(geo::to_string(text));
}

void CommandPanel::input_history_back()
{
    if (m_input_history.isEmpty())
    {
        return;
    }

    if (m_input_history_pos < 0)
    {
        m_input_history_pos = int(m_input_history.size());
    }
    if (m_input_history_pos > 0)
    {
        --m_input_history_pos;
        mp_input->setText(m_input_history[m_input_history_pos]);
    }
}

void CommandPanel::input_history_forward()
{
    if (m_input_history.isEmpty() || m_input_history_pos < 0)
    {
        return;
    }

    ++m_input_history_pos;
    if (m_input_history_pos >= int(m_input_history.size()))
    {
        m_input_history_pos = -1;
        mp_input->clear();
    }
    else
    {
        mp_input->setText(m_input_history[m_input_history_pos]);
    }
}

// ---------------------------------------------------------------------------------------------
//  Command dispatch

bool CommandPanel::dispatch(const std::vector<std::string>& argv)
{
    if (argv.empty())
    {
        return true;
    }

    const std::string& cmd = argv[0];
    std::vector<std::string> args(argv.begin() + 1, argv.end());

    if (cmd == "open" || cmd == "o")
    {
        return cmd_open(args);
    }
    else if (cmd == "close")
    {
        return cmd_close(args);
    }
    else if (cmd == "zoom" || cmd == "z")
    {
        return cmd_zoom(args);
    }
    else if (cmd == "pan")
    {
        return cmd_pan(args);
    }
    else if (cmd == "goto" || cmd == "g")
    {
        return cmd_goto(args);
    }
    else if (cmd == "ruler")
    {
        return cmd_ruler(args);
    }
    else if (cmd == "undo")
    {
        return cmd_undo_redo(args, true);
    }
    else if (cmd == "redo")
    {
        return cmd_undo_redo(args, false);
    }
    else if (cmd == "help" || cmd == "?")
    {
        return cmd_help(args);
    }
    else if (cmd == "echo")
    {
        return cmd_echo(args);
    }
    else if (cmd == "clear")
    {
        return cmd_clear(args);
    }
    else if (cmd == "save_script" || cmd == "save")
    {
        return cmd_save_script(args);
    }
    else if (cmd == "load_script" || cmd == "source")
    {
        return cmd_load_script(args);
    }

    append_message("Unknown command: " + cmd + "  (type 'help' for a list of commands)");
    return true;
}

bool CommandPanel::cmd_open(const std::vector<std::string>& args)
{
    std::string tech;
    int mode = 0;
    std::vector<std::string> files;

    for (size_t i = 0; i < args.size(); ++i)
    {
        const std::string& a = args[i];
        if (a == "-o" || a == "--open")
        {
            //  flag for "open files" - this is the default behavior
        }
        else if (a == "-t" || a == "--technology" || a == "--tech")
        {
            if (i + 1 < args.size())
            {
                tech = args[++i];
            }
        }
        else if (a == "-m" || a == "--mode")
        {
            if (i + 1 < args.size())
            {
                mode = std::atoi(args[++i].c_str());
            }
        }
        else
        {
            files.push_back(expand_path(a));
        }
    }

    if (files.empty())
    {
        append_message("Usage: open [-o] [-t <technology>] [-m <mode>] <file> [file ...]");
        return true;
    }

    if (!tech.empty() && !geo::Technologies::instance()->has_technology(tech))
    {
        append_message("Unknown technology: " + tech + " - using default technology");
        tech.clear();
    }

    for (std::vector<std::string>::const_iterator f = files.begin(); f != files.end(); ++f)
    {
        std::string fn = *f;
        if (mp_mw->check_exist_file(fn))
        {
            append_message("Already open: " + fn);
            continue;
        }

        try
        {
            if (tech.empty())
            {
                mp_mw->load_layout(fn, mode);
            }
            else
            {
                mp_mw->load_layout(fn, tech, mode);
            }
            append_message("Opened: " + fn);
        }
        catch (tl::Exception& ex)
        {
            append_message("Error: " + ex.msg());
        }
        catch (...)
        {
            append_message("Error opening: " + fn);
        }
    }

    return true;
}

bool CommandPanel::cmd_close(const std::vector<std::string>& args)
{
    if (args.empty())
    {
        append_message("Usage: close <index> | close <file> | close all | close current");
        return true;
    }

    if (args[0] == "all")
    {
        //  suppress the echo, close_all() would otherwise echo "close all" again
        mp_mw->set_command_echo(false);
        mp_mw->close_all();
        mp_mw->set_command_echo(true);
        return true;
    }

    int index = -1;
    if (args[0] == "current")
    {
        lay::LayoutView* v = current_view();
        if (v)
        {
            index = mp_mw->index_of(v);
        }
    }
    else if (is_integer(args[0]))
    {
        //  numeric view index
        index = std::atoi(args[0].c_str());
    }
    else
    {
        //  treat the argument as a file name
        std::string fn = expand_path(args[0]);
        index = mp_mw->view_index_for_file(fn);
        if (index < 0)
        {
            append_message("No open file matches: " + args[0]);
            return true;
        }
    }

    if (index < 0 || index >= int(mp_mw->views()))
    {
        append_message("Invalid view index: " + args[0]);
        return true;
    }

    //  suppress the echo, close_view() would otherwise echo "close <file>" again
    mp_mw->set_command_echo(false);
    mp_mw->interactive_close_view(index, true);
    mp_mw->set_command_echo(true);
    return true;
}

bool CommandPanel::cmd_zoom(const std::vector<std::string>& args)
{
    lay::LayoutView* v = current_view();
    if (!v)
    {
        append_message("No layout view");
        return true;
    }

    std::string sub = args.empty() ? std::string() : args[0];
    if (sub == "fit")
    {
        if (args.size() > 1 && args[1] == "sel")
        {
            v->zoom_fit_sel();
        }
        else
        {
            v->zoom_fit();
        }
    }
    else if (sub == "sel")
    {
        v->zoom_fit_sel();
    }
    else if (sub == "in")
    {
        v->zoom_in();
    }
    else if (sub == "out")
    {
        v->zoom_out();
    }
    else
    {
        append_message("Usage: zoom fit | fit sel | in | out");
    }

    return true;
}

bool CommandPanel::cmd_pan(const std::vector<std::string>& args)
{
    lay::LayoutView* v = current_view();
    if (!v)
    {
        append_message("No layout view");
        return true;
    }

    std::string sub = args.empty() ? std::string() : args[0];
    bool fast = args.size() > 1 && (args[1] == "fast" || args[1] == "f");

    if (sub == "left")
    {
        fast ? v->pan_left_fast() : v->pan_left();
    }
    else if (sub == "right")
    {
        fast ? v->pan_right_fast() : v->pan_right();
    }
    else if (sub == "up")
    {
        fast ? v->pan_up_fast() : v->pan_up();
    }
    else if (sub == "down")
    {
        fast ? v->pan_down_fast() : v->pan_down();
    }
    else
    {
        append_message("Usage: pan left | right | up | down [fast]");
    }

    return true;
}

bool CommandPanel::cmd_goto(const std::vector<std::string>& args)
{
    lay::LayoutView* v = current_view();
    if (!v)
    {
        append_message("No layout view");
        return true;
    }

    lay::GotoPlugin* gotos = v->get_plugin<lay::GotoPlugin>();
    if (!gotos)
    {
        append_message("Goto plugin not available");
        return true;
    }

    if (args.empty())
    {
        append_message("Usage: goto <x> <y> [<size>] | goto clear | goto remove <id> | goto list");
        return true;
    }

    const std::string& sub = args[0];

    if (sub == "clear" || sub == "remove_all" || sub == "delete_all")
    {
        gotos->remove_all_gotos();
        append_message("All goto markers removed");
        return true;
    }
    else if (sub == "remove" || sub == "delete" || sub == "rm")
    {
        if (args.size() < 2)
        {
            append_message("Usage: goto remove <id>");
            return true;
        }
        int id = std::atoi(args[1].c_str());
        gotos->remove_goto(id);
        append_message("Goto marker " + geo::to_string(id) + " removed");
        return true;
    }
    else if (sub == "list")
    {
        const std::vector<lay::GotoView*>& markers = gotos->markers();
        for (std::vector<lay::GotoView*>::const_iterator m = markers.begin(); m != markers.end(); ++m)
        {
            append_message(geo::to_string((*m)->id()) + ": " + (*m)->position().to_string());
        }
        append_message(geo::to_string(markers.size()) + " goto marker(s)");
        return true;
    }
    else if (args.size() >= 2)
    {
        //  add a goto marker at the given position (like the goto tool)
        try
        {
            double x = parse_number(args[0]);
            double y = parse_number(args[1]);
            double s = args.size() > 2 ? parse_number(args[2]) : -1.0;
            //  suppress the echo, goto_position() would otherwise echo the same command again
            mp_mw->set_command_echo(false);
            gotos->goto_position(geo::DPoint(x, y), s);
            mp_mw->set_command_echo(true);
            append_message("Goto " + geo::to_string(x) + "," + geo::to_string(y));
        }
        catch (tl::Exception& ex)
        {
            mp_mw->set_command_echo(true);
            append_message("Error: " + ex.msg());
        }
        return true;
    }

    append_message("Usage: goto <x> <y> [<size>] | goto clear | goto remove <id> | goto list");
    return true;
}

bool CommandPanel::cmd_ruler(const std::vector<std::string>& args)
{
    lay::LayoutView* v = current_view();
    if (!v)
    {
        append_message("No layout view");
        return true;
    }

    lay::RulerPlugin* rulers = v->get_plugin<lay::RulerPlugin>();
    if (!rulers)
    {
        append_message("Ruler plugin not available");
        return true;
    }

    if (args.empty())
    {
        append_message("Usage: ruler <x1> <y1> <x2> <y2> | ruler clear | ruler remove <id> | ruler list");
        return true;
    }

    const std::string& sub = args[0];

    if (sub == "clear" || sub == "remove_all" || sub == "delete_all")
    {
        v->remove_all_rulers();
        append_message("All rulers removed");
        return true;
    }
    else if (sub == "remove" || sub == "delete" || sub == "rm")
    {
        if (args.size() < 2)
        {
            append_message("Usage: ruler remove <id>");
            return true;
        }
        int id = std::atoi(args[1].c_str());
        v->remove_ruler(id);
        append_message("Ruler " + geo::to_string(id) + " removed");
        return true;
    }
    else if (sub == "list")
    {
        int count = 0;
        lay::AnnotationShapes& shapes = v->annotation_shapes();
        for (lay::AnnotationShapes::iterator r = shapes.begin(); r != shapes.end(); ++r)
        {
            const lay::RulerObject* robj = dynamic_cast<const lay::RulerObject*>(r->ptr());
            if (robj)
            {
                ++count;
                append_message(geo::to_string(robj->id()) + ": " + robj->p1().to_string() + " -> "
                               + robj->p2().to_string());
            }
        }
        append_message(geo::to_string(count) + " ruler(s)");
        return true;
    }
    else if (args.size() >= 4)
    {
        //  add a ruler between two points (default template)
        try
        {
            double x1 = parse_number(args[0]);
            double y1 = parse_number(args[1]);
            double x2 = parse_number(args[2]);
            double y2 = parse_number(args[3]);
            lay::RulerObject ro(geo::DPoint(x1, y1), geo::DPoint(x2, y2), 0, lay::Template());
            //  suppress the echo, insert_ruler() would otherwise echo the same command again
            mp_mw->set_command_echo(false);
            int id = rulers->insert_ruler(ro, true);
            mp_mw->set_command_echo(true);
            append_message("Ruler added (id " + geo::to_string(id) + ")");
        }
        catch (tl::Exception& ex)
        {
            mp_mw->set_command_echo(true);
            append_message("Error: " + ex.msg());
        }
        return true;
    }

    append_message("Usage: ruler <x1> <y1> <x2> <y2> | ruler clear | ruler remove <id> | ruler list");
    return true;
}

bool CommandPanel::cmd_undo_redo(const std::vector<std::string>& /*args*/, bool is_undo)
{
    lay::LayoutView* v = current_view();
    geo::Manager& m = mp_mw->manager();

    if (is_undo)
    {
        if (!v || !m.available_undo().first)
        {
            append_message("Nothing to undo");
            return true;
        }
        m.undo();
    }
    else
    {
        if (!v || !m.available_redo().first)
        {
            append_message("Nothing to redo");
            return true;
        }
        m.redo();
    }

    return true;
}

bool CommandPanel::cmd_help(const std::vector<std::string>& /*args*/)
{
    append_message("Available commands:");
    append_message("  open [-o] [-t <tech>] [-m <mode>] <file> [file ...]  open layout file(s)");
    append_message("  close <index> | <file> | all | current               close view(s)");
    append_message("  zoom fit | fit sel | in | out                        zoom operations");
    append_message("  pan left | right | up | down [fast]                  pan operations");
    append_message("  goto <x> <y> [<size>] | clear | remove <id> | list   goto operations");
    append_message("  ruler <x1> <y1> <x2> <y2> | clear | remove <id> | list   ruler operations");
    append_message("  undo | redo                                          undo / redo");
    append_message("  save_script <file>                                   save commands as a script");
    append_message("  load_script <file>                                   load and run a script");
    append_message("  clear                                               clear the transcript");
    append_message("  echo <text>                                          print text");
    return true;
}

bool CommandPanel::cmd_echo(const std::vector<std::string>& args)
{
    std::string t;
    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i)
        {
            t += " ";
        }
        t += args[i];
    }
    append_message(t);
    return true;
}

bool CommandPanel::cmd_clear(const std::vector<std::string>& /*args*/)
{
    clear_transcript();
    return true;
}

bool CommandPanel::cmd_save_script(const std::vector<std::string>& args)
{
    if (args.empty())
    {
        append_message("Usage: save_script <file>");
        return true;
    }
    save_script(expand_path(args[0]));
    return true;
}

bool CommandPanel::cmd_load_script(const std::vector<std::string>& args)
{
    if (args.empty())
    {
        append_message("Usage: load_script <file>");
        return true;
    }
    load_script(expand_path(args[0]));
    return true;
}

// ---------------------------------------------------------------------------------------------
//  Script save / load

void CommandPanel::save_script(const std::string& fn)
{
    QString path = geo::to_qstring(fn);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        append_message("Cannot write script: " + fn);
        return;
    }

    QTextStream ts(&f);
    for (size_t i = 0; i < m_commands.size(); ++i)
    {
        ts << geo::to_qstring(m_commands[i]) << "\n";
    }
    f.close();

    append_message("Script saved: " + fn);
}

void CommandPanel::load_script(const std::string& fn)
{
    QString path = geo::to_qstring(fn);
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        append_message("Script not found: " + fn);
        return;
    }

    QTextStream ts(&f);
    while (!ts.atEnd())
    {
        QString line = ts.readLine();
        execute(geo::to_string(line));
    }
    f.close();

    append_message("Script loaded: " + fn);
}

// ---------------------------------------------------------------------------------------------
//  Helpers

std::vector<std::string> CommandPanel::tokenize(const std::string& line) const
{
    std::vector<std::string> tokens;
    std::string cur;
    bool in_single = false;
    bool in_double = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '\\' && i + 1 < line.size())
        {
            cur += line[++i];
        }
        else if (c == '"' && !in_single)
        {
            in_double = !in_double;
        }
        else if (c == '\'' && !in_double)
        {
            in_single = !in_single;
        }
        else if (std::isspace(static_cast<unsigned char>(c)) && !in_single && !in_double)
        {
            if (!cur.empty())
            {
                tokens.push_back(cur);
                cur.clear();
            }
        }
        else
        {
            cur += c;
        }
    }

    if (!cur.empty())
    {
        tokens.push_back(cur);
    }

    return tokens;
}

std::string CommandPanel::expand_path(const std::string& p) const
{
    if (p == "~")
    {
        return geo::to_string(QDir::homePath());
    }
    if (p.size() > 1 && p[0] == '~' && (p[1] == '/' || p[1] == '\\'))
    {
        return geo::to_string(QDir::homePath()) + p.substr(1);
    }
    return p;
}

double CommandPanel::parse_number(const std::string& s) const
{
    geo::Extractor ex(s.c_str());
    double d = geo::Eval().parse(ex).execute().to_double();
    if (!ex.at_end())
    {
        throw tl::Exception("Invalid number: " + s);
    }
    return d;
}

lay::LayoutView* CommandPanel::current_view() const
{
    return mp_mw ? mp_mw->current_view() : 0;
}

// ---------------------------------------------------------------------------------------------
//  Symbol -> command translation (used to echo external operations)

std::string CommandPanel::command_for_symbol(const std::string& symbol)
{
    if (symbol == "cm_zoom_fit")
    {
        return "zoom fit";
    }
    else if (symbol == "cm_zoom_fit_sel")
    {
        return "zoom fit sel";
    }
    else if (symbol == "cm_zoom_in")
    {
        return "zoom in";
    }
    else if (symbol == "cm_zoom_out")
    {
        return "zoom out";
    }
    else if (symbol == "cm_pan_left")
    {
        return "pan left";
    }
    else if (symbol == "cm_pan_right")
    {
        return "pan right";
    }
    else if (symbol == "cm_pan_up")
    {
        return "pan up";
    }
    else if (symbol == "cm_pan_down")
    {
        return "pan down";
    }
    else if (symbol == "cm_undo")
    {
        return "undo";
    }
    else if (symbol == "cm_redo")
    {
        return "redo";
    }
    else if (symbol == "cm_close_all")
    {
        return "close all";
    }
    return std::string();
}

std::string CommandPanel::quote(const std::string& s)
{
    std::string r = "\"";
    for (std::string::const_iterator c = s.begin(); c != s.end(); ++c)
    {
        if (*c == '"' || *c == '\\')
        {
            r += '\\';
        }
        r += *c;
    }
    r += '"';
    return r;
}

} // namespace lay
