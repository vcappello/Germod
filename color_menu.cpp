/*
 * ColorMenu.cpp
 *
 *  Created on: Oct 4, 2011
 *      Author: vincenzo
 */

#include "color_menu.h"

#include <glibmm/i18n.h>

ColorMenu::ColorMenu() :
	active_menu_item_( 0 ),
	allow_alpha_( ColorMenu::DO_NOT_ALLOW_ALPHA )
{
}

ColorMenu::~ColorMenu()
{
}

void ColorMenu::createItems(DISPLAY_NO_COLOR_TYPE display_no_color, ALLOW_ALPHA_TYPE allow_alpha)
{
	allow_alpha_ = allow_alpha;
	
	int palette_row = 0;
	int columns = 3;
	int rows = 9;
	int menu_item_row = 0;

	if (display_no_color == DISPLAY_NO_COLOR_ITEM)
	{
		Gtk::MenuItem* no_color_menu_item = Gtk::manage(new Gtk::MenuItem(_("No color")));
		no_color_menu_item->signal_activate().connect(
				sigc::mem_fun( *this, &ColorMenu::onNoColorMenuItemActivate ));

		this->attach(*no_color_menu_item, 0, columns, menu_item_row, menu_item_row + 1);
		menu_item_row++;
	}

	double palette[27][3] = {
		{252, 233, 79},
		{237, 212, 0},
		{196, 160, 0},
		{138, 226, 52},
		{115, 210, 22},
		{78, 154, 6},
		{252, 175, 62},
		{245, 121, 0},
		{206, 92, 0},
		{114, 159, 207},
		{52, 101, 164},
		{32, 74, 135},
		{173, 127, 168},
		{117, 80, 123},
		{92, 53, 102},
		{233, 185, 110},
		{193, 125, 17},
		{143, 89, 2},
		{239, 41, 41},
		{204, 0, 0},
		{164, 0, 0},
		{238, 238, 236},
		{211, 215, 207},
		{186, 189, 182},
		{136, 138, 133},
		{85, 87, 83},
		{46, 52, 54} };

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			cppgef::Color palette_color;
			
			palette_color.setRGB (
				palette[palette_row][0] / 255.0,
				palette[palette_row][1] / 255.0,
				palette[palette_row][2] / 255.0);

			ColorMenuItem* menu_item = Gtk::manage(new ColorMenuItem(palette_color));
			this->attach(*menu_item, x, x + 1, menu_item_row, menu_item_row + 1);
			
			menu_item->signal_activate().connect(
				sigc::bind< ColorMenuItem* >(
					sigc::mem_fun( *this, &ColorMenu::onMenuItemActivate ),
					menu_item));
				
			palette_row++;
		}
		menu_item_row++;
	}

	// Recents
	this->attach(*Gtk::manage(new Gtk::SeparatorMenuItem()), 0, columns, menu_item_row, menu_item_row + 1);
	menu_item_row++;

	for (int x = 0; x < columns; x++)
	{
		ColorMenuItem* menu_item = Gtk::manage(new ColorMenuItem(cppgef::Color("white")));
		this->attach(*menu_item, x, x + 1, menu_item_row, menu_item_row + 1);

		recents_items_.push_back (menu_item);

		menu_item->signal_activate().connect (
			sigc::bind< ColorMenuItem* >(
				sigc::mem_fun( *this, &ColorMenu::onMenuItemActivate ),
				menu_item));
	}
	menu_item_row++;

	// Other
	this->attach(*Gtk::manage(new Gtk::SeparatorMenuItem()), 0, columns, menu_item_row, menu_item_row + 1);
	menu_item_row++;

	Gtk::MenuItem* choose_other_color_menu_item = Gtk::manage(new Gtk::MenuItem(_("Choose other color...")));
	choose_other_color_menu_item->signal_activate().connect(
			sigc::mem_fun( *this, &ColorMenu::onOtherColorMenuItemActivate ));

	this->attach(*choose_other_color_menu_item, 0, columns, menu_item_row, menu_item_row + 1);

	this->show_all();
}

ColorMenuItem* ColorMenu::getActiveMenuItem()
{
	return active_menu_item_;
}

void ColorMenu::onMenuItemActivate(ColorMenuItem* menu_item)
{
	if (active_menu_item_)
		active_menu_item_->setActive (false);
		
	menu_item->setActive (true);
	
	active_menu_item_ = menu_item;
	
	signal_color_selected_.emit (menu_item->getColor());
}

void ColorMenu::onOtherColorMenuItemActivate()
{
    Gtk::ColorSelectionDialog color_dialog;
	
	if (allow_alpha_)
		color_dialog.get_color_selection()->set_has_opacity_control (true);
	else
		color_dialog.get_color_selection()->set_has_opacity_control (false);
		
    int dialog_return = color_dialog.run();
    if (dialog_return == Gtk::RESPONSE_OK)
	{
    	if (active_menu_item_)
    		active_menu_item_->setActive (false);

    	// Update recents
    	for(size_t x = 0; x < recents_items_.size() - 1; x++)
    		recents_items_[x]->setColor(recents_items_[x + 1]->getColor());

    	cppgef::Color selected_color;
    	selected_color.setFromDialog (color_dialog);

    	recents_items_[recents_items_.size() - 1]->setColor (selected_color);

    	active_menu_item_ = recents_items_[recents_items_.size() - 1];
    	active_menu_item_->setActive (true);

    	// Notify
    	signal_color_selected_.emit (selected_color);
	}
}

void ColorMenu::onNoColorMenuItemActivate()
{
	if (active_menu_item_)
		active_menu_item_->setActive (false);

	signal_no_color_selected_.emit();
}

