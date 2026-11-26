#include "layMenuEntry.h"

namespace lay
{
// -----------------------------------------------------------------------------
//  MenuEntry implementation



// ---------------------------------------------------------------------------------------------------
//  Menu item generators

MenuEntry separator (const std::string &menu_name, const std::string &insert_pos)
{
  MenuEntry e;
  e.menu_name = menu_name;
  e.insert_pos = insert_pos;
  e.separator = true;
  return e;
}

MenuEntry separator (const std::string &insert_pos)
{
    MenuEntry e;
    e.insert_pos = insert_pos;
    e.separator = true;
    return e;
}

MenuEntry menu_item (const std::string &symbol, const std::string &menu_name, const std::string &insert_pos, const std::string &title)
{
  MenuEntry e;
  e.symbol = symbol;
  e.menu_name = menu_name;
  e.insert_pos = insert_pos;
  e.title = title;
  return e;
}


MenuEntry menu_item_copy (const std::string &symbol, const std::string &menu_name, const std::string &insert_pos, const std::string &copy_from)
{
  MenuEntry e;
  e.symbol = symbol;
  e.menu_name = menu_name;
  e.insert_pos = insert_pos;
  e.copy_from = copy_from;
  return e;
}

MenuEntry submenu (const std::string &menu_name, const std::string &insert_pos, const std::string &title)
{
  MenuEntry e;
  e.menu_name = menu_name;
  e.insert_pos = insert_pos;
  e.title = title;
  e.sub_menu = true;
  return e;
}

MenuEntry submenu (const std::string &symbol, const std::string &menu_name, const std::string &insert_pos, const std::string &title)
{
  MenuEntry e;
  e.symbol = symbol;
  e.menu_name = menu_name;
  e.insert_pos = insert_pos;
  e.title = title;
  e.sub_menu = true;
  return e;
}


MenuEntry config_menu_item (const std::string &menu_name, const std::string &insert_pos, const std::string &title, const std::string &cname, const std::string &cvalue)
{
  MenuEntry e;
  e.menu_name = menu_name;
  e.insert_pos = insert_pos;
  e.title = title;
  e.cname = cname;
  e.cvalue = cvalue;
  return e;
}

MenuEntry config_menu_item (const std::string &menu_name, const std::string &title)
{
    MenuEntry e;
    e.menu_name = menu_name;
    e.title = title;
    e.cname = "cname";
    e.cvalue = "?";
    e.sub_menu = true;
    return e;
}

MenuEntry menu(const std::string &menu_name, const std::string &title)
{
    MenuEntry e;
    e.menu_name = menu_name;
    e.title = title;
    return e;
}

MenuEntry menu_item(const std::string &menu_name, const std::string &title)
{
    MenuEntry e;
    e.menu_name = menu_name;
    e.title = title;
    e.sub_menu = true;
    return e;
}

MenuEntry menu_item(const std::string &menu_name, const std::string &symbol, const std::string &title)
{
    MenuEntry e;
    e.symbol = symbol;
    e.menu_name = menu_name;
    e.title = title;
    e.sub_menu = true;
    return e;
}

}
