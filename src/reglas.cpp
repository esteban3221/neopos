#include "reglas.hpp"

reglas::reglas(std::shared_ptr<SQLite> db) : sql(db)
{
    sql->open();
    std::string xml = "<interface>"

                      "<object class=\"GtkBox\" id=\"box_main\">"
                      "<child>"
                      "<object class=\"GtkScrolledWindow\">"
                      "<property name=\"hexpand\">1</property>"
                      "<property name=\"hscrollbar-policy\">never</property>"
                      "<property name=\"min-content-height\">200</property>"
                      "<property name=\"vexpand\">1</property>"
                      "<child>"
                      "<object class=\"GtkViewport\">"
                      "<property name=\"scroll-to-focus\">1</property>"
                      "<child>"
                      "<object class=\"GtkBox\">"
                      "<property name=\"margin-bottom\">30</property>"
                      "<property name=\"margin-end\">60</property>"
                      "<property name=\"margin-start\">60</property>"
                      "<property name=\"margin-top\">30</property>"
                      "<property name=\"orientation\">vertical</property>"

                      "<child>"
                      "<object class=\"GtkLabel\">"
                      "<property name=\"label\">Reglas</property>"
                      "<property name=\"margin-bottom\">10</property>"
                      "<property name=\"xalign\">0</property>"
                      "<style>"
                      "<class name=\"title-2\"/>"
                      "</style>"
                      "</object>"
                      "</child>"

                      "<child>"
                      "<object class=\"GtkListBox\" id=\"list_reglas\">"
                      "<property name=\"selection-mode\">none</property>"
                      "<style>"
                      "<class name=\"rich-list\"/>"
                      "<class name=\"boxed-list\"/>"
                      "</style>"
                      "</object>"
                      "</child>"
                      "</object>"

                      "</child>"
                      "</object>"
                      "</child>"
                      "</object>"
                      "</child>"
                      "</object>"
                      "</interface>";

    auto builder = Gtk::Builder::create();
    try
    {
        builder->add_from_string(xml);
    }
    catch (const Glib::Error &ex)
    {
        std::cerr << "building toolbar failed: " << ex.what();
    }

    box_main = builder->get_widget<Gtk::Box>("box_main");
    rvl_ety_producto.set_child(ety_producto);
    rvl_ety_producto.set_margin(10);
    rvl_ety_producto.set_reveal_child(false);

    ety_producto.set_placeholder_text("Ingresa el codigo de producto");
    list_reglas = builder->get_widget<Gtk::ListBox>("list_reglas");

    this->set_child(*box_main);
    box_main->prepend(rvl_ety_producto);
    box_main->set_orientation(Gtk::Orientation::VERTICAL);
    this->set_titlebar(header_bar);
    add_regla.set_image_from_icon_name("list-add-symbolic");
    header_bar.pack_start(add_regla);
    this->set_title("Reglas de Venta");
    list_reglas->set_activate_on_single_click(false);

    this->sql->command("SELECT * FROM promo");

    for (int i = 0; i < sql->get_result().size(); i++)
    {
        this->list_reglas->prepend(*add_row(sql->get_result()[i][0], sql->get_result()[i][1], sql->get_result()[i][2], std::stoul(sql->get_result()[i][3]), std::stod(sql->get_result()[i][4])));
        this->index[sql->get_result()[i][0]] = std::make_tuple(sql->get_result()[i][2], std::stoul(sql->get_result()[i][3]), std::stod(sql->get_result()[i][4]));
    }
    this->sql->clear_result();

    this->add_regla.signal_clicked().connect([this]()
                                             { 
        if (!rvl_ety_producto.get_reveal_child())
            rvl_ety_producto.set_reveal_child();
        else
            rvl_ety_producto.set_reveal_child(false); });

    this->ety_producto.signal_activate().connect([this]()
                                                 {
                                               this->sql->command("SELECT sku,nombre FROM producto WHERE sku = '" + ety_producto.get_text() + "'");

                                               if (this->sql->get_result().size() < 1)
                                               {
                                                   this->ety_producto.set_css_classes({"error"});
                                                   this->ety_producto.set_placeholder_text("No se encontro ningun producto con ese codigo");
                                               }
                                               else
                                               {
                                                   try
                                                   {
                                                       this->sql->command("INSERT INTO promo VALUES(" + sql->get_result()[0][0] + ", '" + sql->get_result()[0][1] + "', '', 0,0)");
                                                       this->list_reglas->prepend(*add_row(sql->get_result()[0][0],sql->get_result()[0][1], "Editame (Alias)",0,0));
                                                       this->index[ety_producto.get_text()] = std::make_tuple("",0,0);
                                                       this->ety_producto.set_placeholder_text("Ingresa el codigo de producto");
                                                       this->ety_producto.set_css_classes({"entry"});
                                                   }
                                                   catch (const std::exception &e)
                                                   {
                                                       this->ety_producto.set_css_classes({"error"});
                                                       this->ety_producto.set_placeholder_text(e.what());
                                                   }
                                               }
                                               this->ety_producto.set_text("");
                                               this->sql->clear_result(); });
}

Gtk::ListBoxRow *reglas::add_row(const std::string &sku, const std::string &title, const std::string &subtitle, uint16_t mul, double precio)
{
    Gtk::ListBoxRow *test_row = new Gtk::ListBoxRow;
    

    auto box_label = new Gtk::Box;
    auto box_control = new Gtk::Box;
    auto box_row = new Gtk::Box;
    auto btn_del = new Gtk::Button;

    Gtk::SpinButton *spin_cantidad = new Gtk::SpinButton(Gtk::Adjustment::create(mul, 1.0, 10000000.0, 1.0, 5.0, 0.0));
    Gtk::SpinButton *spin_precio = new Gtk::SpinButton(Gtk::Adjustment::create(precio, 1.0, 10000000.0, 1.0, 5.0, 0.0));
    Gtk::Label label_title(title);
    Gtk::Label label_x("x");
    Gtk::EditableLabel *label_subti = new Gtk::EditableLabel(subtitle);
    spin_precio->set_digits(2);

    box_control->append(*spin_cantidad);
    box_control->append(label_x);
    box_control->append(*spin_precio);
    box_control->set_spacing(5);

    box_label->set_orientation(Gtk::Orientation::VERTICAL);
    box_row->set_orientation(Gtk::Orientation::HORIZONTAL);

    spin_cantidad->set_orientation(Gtk::Orientation::HORIZONTAL);

    btn_del->set_image_from_icon_name("list-remove-symbolic");
    btn_del->set_css_classes({"destructive-action"});
    btn_del->set_hexpand();
    btn_del->set_halign(Gtk::Align::END);
    btn_del->set_tooltip_text("Elimina Regla de Venta.");

    label_title.add_css_class("heading");
    label_subti->add_css_class("dim-label");
    label_title.set_halign(Gtk::Align::START);
    label_subti->set_halign(Gtk::Align::START);

    label_subti->set_editable();

    box_label->append(label_title);
    box_label->append(*label_subti);

    box_label->set_spacing(2);
    box_label->set_hexpand();

    box_row->append(*box_label);
    box_row->append(*box_control);
    box_row->append(*btn_del);

    test_row->set_child(*box_row);

    btn_del->signal_clicked().connect([this, test_row, sku]()
                                      {
                                        this->sql->command("DELETE FROM promo WHERE sku = '" + sku + "'");
                                        this->list_reglas->remove(*test_row);
                                        this->index.erase(sku); 
                                        });

    label_subti->signal_changed().connect([this, label_subti, sku]()
                                          { 
                                            sql->command("UPDATE promo SET alias = '" + label_subti->get_text() + "' WHERE sku = '" + sku + "'"); 
                                            index[sku] = std::make_tuple(label_subti->get_text(), std::get<1>(index[sku]), std::get<2>(index[sku]));
                                          });
    spin_cantidad->signal_changed().connect([this, spin_cantidad, sku]()
                                            { 
                                                sql->command("UPDATE promo SET mul = '" + spin_cantidad->get_text() + "' WHERE sku = '" + sku + "'"); 
                                                index[sku] = std::make_tuple(std::get<0>(index[sku]), spin_cantidad->get_value_as_int(), std::get<2>(index[sku]));
                                                });
    spin_precio->signal_changed().connect([this, spin_precio, sku]()
                                          { 
                                            sql->command("UPDATE promo SET precio = '" + spin_precio->get_text() + "' WHERE sku = '" + sku + "'"); 
                                            index[sku] = std::make_tuple(std::get<0>(index[sku]), std::get<1>(index[sku]), spin_precio->get_value());
                                            });

    return test_row;
}

reglas::~reglas()
{
    this->list_reglas->remove_all();
    delete list_reglas;
    delete box_main;
}

const std::tuple<std::string,uint16_t,double> reglas::obj_promo(const std::string &sku) const
{
    if(auto it = index.find(sku); it != index.end())
    {
        return it->second;
    }else
    {
        return std::make_tuple("", 0, 0.0);;
    }
}
