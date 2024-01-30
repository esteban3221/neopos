#include "Pos.hpp"
#include <iomanip>
#include <fstream>

void Pos::init_reporte()
{
    m_refTreeModel_reporte = Gtk::ListStore::create(m_Columns_reporte);
    tree_repor->set_model(m_refTreeModel_reporte);

    tree_repor->append_column("Folio", m_Columns_reporte.id);
    tree_repor->append_column("Tipo de Venta", m_Columns_reporte.tipo);
    tree_repor->append_column_numeric("Total", m_Columns_reporte.total, "$%.2f");
    tree_repor->append_column_numeric("Ingreso", m_Columns_reporte.ingreso, "$%.2f");
    tree_repor->append_column_numeric("Cambio", m_Columns_reporte.cambio, "$%.2f");
    tree_repor->append_column("Folio Tarjeta", m_Columns_reporte.folio);
    tree_repor->append_column("Fecha", m_Columns_reporte.fecha);

    db->command("SELECT * FROM venta ORDER BY id");

    for (size_t i = 0; i < db->get_result().size(); i++)
    {
        row_reporte = *(m_refTreeModel_reporte->prepend());
        row_reporte[m_Columns_reporte.id] = std::stoll(db->get_result()[i][0]);
        row_reporte[m_Columns_reporte.tipo] = db->get_result()[i][1];
        row_reporte[m_Columns_reporte.total] = std::stof(db->get_result()[i][2]);
        row_reporte[m_Columns_reporte.ingreso] = std::stof(db->get_result()[i][3]);
        row_reporte[m_Columns_reporte.cambio] = std::stof(db->get_result()[i][4]);
        row_reporte[m_Columns_reporte.folio] = db->get_result()[i][5];
        row_reporte[m_Columns_reporte.fecha] = db->get_result()[i][6];
        row_reporte[m_Columns_reporte.datos] = db->get_result()[i][7];
    }
    db->clear_result();
}

void Pos::init_detalle_venta()
{
    Modeldetalle_venta = Gtk::ListStore::create(m_Columns_detalle_venta);
    tree_detalle_venta->set_model(Modeldetalle_venta);

    // tree_detalle_venta->append_column("Código", m_Columns_detalle_venta.sku);
    tree_detalle_venta->append_column_numeric("Cantidad", m_Columns_detalle_venta.cantidad, "%.3f");
    tree_detalle_venta->append_column("Descripción", m_Columns_detalle_venta.nombre);
    tree_detalle_venta->append_column_numeric("Precio", m_Columns_detalle_venta.precio_u, "$%.2f");
    tree_detalle_venta->append_column_numeric("Total", m_Columns_detalle_venta.precio_t, "$%.2f");
}

void Pos::on_tree_detalle_venta_row_activated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
    auto iter = m_refTreeModel_reporte->get_iter(path);
    if (iter)
    {
        if (!Modeldetalle_venta->children().empty())
            Modeldetalle_venta->clear();
        std::istringstream iss((*iter)[m_Columns_reporte.datos].operator Glib::ustring());
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(iss, token, '|'))
        {
            tokens.push_back(token);
            if (tokens.size() == 5)
            {
                row_detalle_venta = *(Modeldetalle_venta->append());
                row_detalle_venta[m_Columns_detalle_venta.sku] = std::stoll(tokens[0]);
                row_detalle_venta[m_Columns_detalle_venta.cantidad] = std::stof(tokens[1]);
                row_detalle_venta[m_Columns_detalle_venta.nombre] = tokens[2];
                row_detalle_venta[m_Columns_detalle_venta.precio_u] = std::stof(tokens[3]);
                row_detalle_venta[m_Columns_detalle_venta.precio_t] = std::stof(tokens[4]);
                tokens.clear();
            }
        }
    }
}

void Pos::on_popup_button_pressed(int /* n_press */, double x, double y)
{
    if (tree_repor->is_focus())
    {
        const Gdk::Rectangle rect(x, y - 50, 0, 0);
        m_MenuPopup.set_pointing_to(rect);
        m_MenuPopup.popup();
    }
}

void Pos::act_dashboard()
{
    db->command("SELECT sum(total) FROM venta WHERE strftime('%Y-%m', fecha) = strftime('%Y-%m', 'now');");
    lbl_venta_mes->set_markup("$<span font_desc='50'>" + db->get_result()[0][0] + "</span>");
    db->clear_result();

    db->command("SELECT sum(total) FROM venta WHERE strftime('%Y-%m-%d', fecha) = strftime('%Y-%m-%d', 'now');");
    lbl_venta_dia->set_markup("$<span font_desc='50'>" + db->get_result()[0][0] + "</span>");
    db->clear_result();

    db->command("SELECT count(*) FROM venta WHERE strftime('%Y-%m-%d', fecha) = strftime('%Y-%m-%d', 'now');");
    lbl_clientes_dia->set_markup("<span font_desc='50'>" + db->get_result()[0][0] + "</span>");
    db->clear_result();

    db->command("SELECT min(total) FROM venta WHERE strftime('%Y-%m', fecha) = strftime('%Y-%m', 'now')");
    lbl_venta_min_mes->set_markup("$<span font_desc='50'>" + db->get_result()[0][0] + "</span>");
    db->clear_result();

    db->command("SELECT max(total) FROM venta WHERE strftime('%Y-%m', fecha) = strftime('%Y-%m', 'now')");
    lbl_venta_max_mes->set_markup("$<span font_desc='50'>" + db->get_result()[0][0] + "</span>");
    db->clear_result();

    db->command("SELECT SUM(total) AS TotalVentas FROM venta WHERE strftime('%Y', fecha) = strftime('%Y', 'now')");
    lbl_venta_anio->set_markup("$<span font_desc='50'>" + db->get_result()[0][0] + "</span>");
    db->clear_result();

    db->command("SELECT min(total) AS TotalVentas FROM venta WHERE strftime('%Y-%m', fecha) = strftime('%Y-%m', 'now')");
    lbl_venta_min_anio->set_markup("$<span font_desc='50'>" + db->get_result()[0][0] + "</span>");
    db->clear_result();

    db->command("SELECT max(total) AS TotalVentas FROM venta WHERE strftime('%Y-%m', fecha) = strftime('%Y-%m', 'now')");
    lbl_venta_max_anio->set_markup("$<span font_desc='50'>" + db->get_result()[0][0] + "</span>");
    db->clear_result();

    db->command("SELECT SUM(total) AS TotalVentas FROM venta WHERE strftime('%Y', Fecha) = strftime('%Y', 'now', '-1 year')");
    lbl_venta_anio_anterior->set_markup("$<span font_desc='50'>" + (db->get_result()[0][0] == "NULL" ? "0" : db->get_result()[0][0]) + "</span>");
    db->clear_result();
}

void Pos::on_menu_file_popup_reimprime()
{
    auto refSelection = tree_repor->get_selection();

    if (refSelection)
    {
        auto iter = refSelection->get_selected();
        
        std::stringstream ticket;

        ticket << "****** TICKET DE COMPRA ******\n"
               << "--------------------------------\n"
               << "-----------REIMPRESION----------\n"
               << "--------------------------------\n\n"
               << std::left << std::setw(20)
               << Conf->ety_conf_razon->get_text() << "\n\n";

        if (Conf->vec_check[2]->get_active())
        {
            ticket << "Dirección: " << Conf->ety_conf_direccion->get_text()
                   << "\n"
                   << "--------------------------------\n";
        }

        if (Conf->vec_check[3]->get_active())
        {
            ticket << "RFC: " << Conf->ety_conf_rfc->get_text() << "\n"
                   << "--------------------------------\n";
        }
        if (Conf->vec_check[1]->get_active())
            ticket << "Fecha: "
                   << (*iter)[m_Columns_reporte.fecha]
                   << "\n";
        ticket << "No. Ticket: " << (*iter)[m_Columns_reporte.id] << "\n\n";
        // if (Conf->vec_check[4]->get_active())
        //  true;
        //  ticket << std::left << std::setw (10) << "Le atendió: "
        //                << "Juan Perez"
        //                << "\n\n"
        //                << "--------------------------------\n";
        ticket << "Articulo\n"
               << std::setw(10) << "Cnt." << std::setw(10) << "P.U."
               << "T.\n"
               << "--------------------------------\n";

        std::istringstream iss((*iter)[m_Columns_reporte.datos].operator Glib::ustring());
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(iss, token, '|'))
        {
            tokens.push_back(token);
            if (tokens.size() == 5)
            {

                ticket << tokens[2] << "\n"
                   << std::setw(10) << tokens[1]
                   << "$" << std::setw(10) << tokens[3]
                   << "$" << tokens[4] << std::endl;

                tokens.clear();
            }
        }

        ticket << "--------------------------------\n";
        ticket << std::left << std::setw(20) << "Total:"
               << "$" << std::fixed << std::setprecision(2) << (*iter)[m_Columns_reporte.total] << std::endl;
        ticket << std::left << std::setw(20) << "Tipo de Pago:"
               << (*iter)[m_Columns_reporte.tipo] << "\n"
               << "--------------------------------\n";
        ticket << std::left << std::setw(20) << "Ingreso:"
               << "$" << (*iter)[m_Columns_reporte.ingreso] << "\n";
        ticket << std::left << std::setw(20) << "Cambio:"
               << "$" << std::fixed << (*iter)[m_Columns_reporte.cambio] << std::endl
               << "--------------------------------\n";
        ticket << "Folio Tarjeta:  " << (*iter)[m_Columns_reporte.folio] << std::endl
               << "--------------------------------\n";
        if (Conf->vec_check[5]->get_active())
            ticket << "**" << Conf->ety_conf_contacto->get_text() << "**"
                   << "\n"
                   << "--------------------------------\n";
        if (Conf->vec_check[0]->get_active())
            ticket << "**" << Conf->ety_conf_thanks->get_text() << "**"
                   << "\n"
                   << "--------------------------------\n";

        std::ofstream archivoTemp("temp.txt");
        archivoTemp << ticket.str();
        archivoTemp.close();

#ifdef __linux__
            std::system("lp temp.txt");
#endif

#ifdef _WIN32
            std::system("notepad /p temp.txt");
#endif

        remove("temp.txt");
    }
}

void Pos::on_menu_file_popup_generic()
{

    auto refSelection = tree_repor->get_selection();
    if (refSelection)
    {
        auto iter = refSelection->get_selected();
        if (iter)
        {
            dialog.reset(new Gtk::MessageDialog(*this, "Cancelacion de Venta.", false, Gtk::MessageType::INFO, Gtk::ButtonsType::CANCEL, true));
            dialog->set_secondary_text("Escriba el motivo de la cancelacion.");
            dialog->get_content_area()->append(ety_folio);
            dialog->add_button("Cancelar Venta", Gtk::ResponseType::OK)->set_css_classes({"destructive-action"});
            ety_folio.show();
            dialog->signal_response().connect([=](int response)
                                              {
                if(response == Gtk::ResponseType::OK){
                    if(!ety_folio.get_text().empty()){

                        std::istringstream iss((*iter)[m_Columns_reporte.datos].operator Glib::ustring());
                        std::string token;
                        std::vector<std::string> tokens;
                        while (std::getline(iss, token, '|'))
                        {
                            tokens.push_back(token);
                            if (tokens.size() == 5)
                            {   
                                std::cout << std::stoll(tokens[0]) << std::endl;
                                std::cout << tokens[0] << std::endl;
                                for (auto i : m_refTreeModel_prod->children()){
                                    if(i[m_Columns_prod.sku] == std::stoll(tokens[0])){
                                        auto a = i[m_Columns_prod.piezas].operator float() + std::stof(tokens[1]);
                                        db->command("UPDATE producto SET piezas = " + std::to_string(a) + " WHERE sku = " + std::to_string(std::stoll(tokens[0])));
                                        (i)[m_Columns_prod.piezas] = a;
                                        break;
                                    }
                                }
                                Modeldetalle_venta->clear();
                                tokens.clear();
                            }
                        }
                    db->command("DELETE FROM venta WHERE id = " + std::to_string((*iter)[m_Columns_reporte.id]));
                    m_refTreeModel_reporte->erase(iter);
                    dialog->hide();
                    }
                }
                else
                    dialog->hide();
                act_dashboard(); });
            ety_folio.grab_focus();
            dialog->show();
        }
    }
}