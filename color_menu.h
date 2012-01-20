/*
 * ColorMenu.h
 *
 *  Created on: Oct 4, 2011
 *      Author: vincenzo
 */

#ifndef COLORMENU_H_
#define COLORMENU_H_

#include <gtkmm.h>

#include <list>

#include <color.h>

#include "color_menu_item.h"

class ColorMenu : public Gtk::Menu
{
public:
	typedef sigc::signal< void, cppgef::Color > signal_color_selected_t;

	typedef sigc::signal< void > signal_no_color_selected_t;

	enum DISPLAY_NO_COLOR_TYPE
	{
		DISPLAY_NO_COLOR_ITEM,
		DO_NOT_DISPLAY_NO_COLOR_ITEM
	};

	enum ALLOW_ALPHA_TYPE
	{
		DO_NOT_ALLOW_ALPHA,
		ALLOW_ALPHA
	};
	
public:
	ColorMenu();
	virtual ~ColorMenu();

	ColorMenuItem* getActiveMenuItem();
	
	void createItems(DISPLAY_NO_COLOR_TYPE display_no_color, ALLOW_ALPHA_TYPE allow_alpha);
	
	// Signal accessor
	signal_color_selected_t signalColorSelected() { return signal_color_selected_; };
	signal_no_color_selected_t signalNoColorSelected() { return signal_no_color_selected_; };

protected:
	ColorMenuItem* active_menu_item_;
	ALLOW_ALPHA_TYPE allow_alpha_;
	
	std::vector< ColorMenuItem* > recents_items_;

	// Signal instance
	signal_color_selected_t signal_color_selected_;
	signal_no_color_selected_t signal_no_color_selected_;
	
protected:
	void onMenuItemActivate(ColorMenuItem* menu_item);
	void onOtherColorMenuItemActivate();
	void onNoColorMenuItemActivate();
};

#endif /* COLORMENU_H_ */
