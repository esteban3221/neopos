#pragma onece
#include <gtkmm.h>
#include "sqlite.hpp"

class conf : public Gtk::Window
{
private:
    Gtk::TextView *text_ticket;
    Gtk::ListBox *list_config_visualizacion, *list_config_datos, *list_config_test;
    Gtk::CheckButton *check_config_1, *check_config_2, *check_config_3, *check_config_4, *check_config_5, *check_config_6;
    Gtk::Box *box_main;

    std::stringstream ticket_config;
    Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1 = Gtk::TextBuffer::create();
    std::unique_ptr<Gtk::MessageDialog> dialog;
    void initconf();
    bool on_window_conf_ticket_delete_event();
    void init_conf();
    void edit_ticket();

    std::shared_ptr<SQLite> sql;

public:
    conf(std::shared_ptr<SQLite> db);
    ~conf();

    std::map<short, Gtk::CheckButton *> vec_check{{0, check_config_1}, {1, check_config_2}, {2, check_config_3}, {3, check_config_4}, {4, check_config_5}, {5, check_config_6}};
    Gtk::Entry *ety_conf_razon, *ety_conf_direccion, *ety_conf_rfc, *ety_conf_contacto, *ety_conf_thanks;
};