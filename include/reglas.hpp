#pragma onece
#include <gtkmm.h>
#include "sqlite.hpp"
#include <iostream>
#include <map>
#include <tuple>

class reglas : public Gtk::Window
{
private:
    /* data */
    Gtk::Box *box_main;
    Gtk::HeaderBar header_bar;
    std::shared_ptr<SQLite> sql;
    Gtk::Button add_regla;

    Gtk::ListBox *list_reglas;
    Gtk::Revealer rvl_ety_producto;
    Gtk::Entry ety_producto;
    std::map<std::string,std::tuple<std::string,uint16_t,double>> index;
    
    Gtk::ListBoxRow* add_row(const std::string &,const std::string &title,const std::string &subtitle, uint16_t mul, double precio);
public:
    reglas(std::shared_ptr<SQLite> db);
    ~reglas();

    const std::tuple<std::string,uint16_t,double> obj_promo(const std::string &)const ;
};