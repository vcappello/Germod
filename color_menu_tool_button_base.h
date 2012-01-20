/*
 * color_menu_tool_button_base.h
 *
 *  Created on: Oct 11, 2011
 *      Author: vincenzo
 */

#ifndef COLOR_MENU_TOOL_BUTTON_BASE_H_
#define COLOR_MENU_TOOL_BUTTON_BASE_H_

#include "custom_menu_tool_button.h"

#include <color.h>

#include "color_menu.h"

class ColorMenuToolButtonBase : public CustomMenuToolButton
{
public:
	typedef sigc::signal< void, cppgef::Color > signal_color_selected_t;

	typedef sigc::signal< void > signal_no_color_selected_t;

public:
	ColorMenuToolButtonBase();
	ColorMenuToolButtonBase(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~ColorMenuToolButtonBase();

	void initialize(ColorMenu::DISPLAY_NO_COLOR_TYPE display_no_color, ColorMenu::ALLOW_ALPHA_TYPE allow_alpha, const cppgef::Color& default_color, bool deafault_has_color=true);

	// Signal accessor
	signal_color_selected_t signalColorSelected() { return signal_color_selected_; };
	signal_no_color_selected_t signalNoColorSelected() { return signal_no_color_selected_; };

protected:
	ColorMenu* text_color_menu_;

	cppgef::Color selected_color_;
	bool has_color_;

	// Signal instance
	signal_color_selected_t signal_color_selected_;
	signal_no_color_selected_t signal_no_color_selected_;

protected:
	void onColorMenuColorSelected(cppgef::Color color);
	void onColorMenuNoColorSelected();
	void onButtonColorSelected();

	virtual void drawColorRectangle(const Cairo::RefPtr< Cairo::Context > context, double x, double y, double w, double h);
};

#endif /* COLOR_MENU_TOOL_BUTTON_BASE_H_ */
