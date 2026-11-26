#pragma once


#include <string>

namespace lay
{

struct MenuEntry
{
  MenuEntry () : sub_menu (false), checkable (false), separator (false) { }

  std::string menu_name;
  std::string symbol;
  std::string insert_pos;
  std::string title;
  std::string copy_from;
  std::string cname;
  std::string cvalue;
  std::string exclusive_group;
  bool sub_menu;
  bool checkable;
  bool separator;
};

MenuEntry separator (const std::string &insert_pos);
MenuEntry menu (const std::string &menu_name, const std::string &title);
MenuEntry menu_item (const std::string &menu_name, const std::string &title);
MenuEntry menu_item (const std::string &menu_name, const std::string &symbol, const std::string &title);
MenuEntry config_menu_item (const std::string &menu_name, const std::string &title);

MenuEntry separator (const std::string &menu_name, const std::string &insert_pos);
MenuEntry menu_item (const std::string &symbol, const std::string &menu_name, const std::string &insert_pos, const std::string &title);
MenuEntry menu_item_copy (const std::string &symbol, const std::string &menu_name, const std::string &insert_pos, const std::string &copy_from);
MenuEntry submenu (const std::string &menu_name, const std::string &insert_pos, const std::string &title);
MenuEntry submenu (const std::string &symbol, const std::string &menu_name, const std::string &insert_pos, const std::string &title);
MenuEntry config_menu_item (const std::string &menu_name, const std::string &insert_pos, const std::string &title, const std::string &cname, const std::string &cvalue = std::string ());


}
