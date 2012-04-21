/*
 * dash_style_menu_tool_button.h
 *
 *  Created on: Nov 22, 2011
 *      Author: vincenzo
 */

#ifndef DASH_STYLE_MENU_TOOL_BUTTON_H_
#define DASH_STYLE_MENU_TOOL_BUTTON_H_

#include "custom_menu_tool_button.h"

#include "generic_menu.h"

class DashStyleRenderer : public GenericItemRenderer< std::vector< double > >
{
public:
	DashStyleRenderer(int width, int height) :
		width_( width ),
		height_( height )
	{
	}

	virtual int getImageWidth() const { return width_; }
	virtual int getImageHeight() const { return height_; }

	virtual void drawImageSurface(const Cairo::RefPtr< Cairo::Context > context, const std::vector< double >& item_value)
	{
		context->set_source_rgb (0, 0, 0);

		context->set_dash (const_cast< std::vector< double >& >( item_value ), 0.0);

		context->move_to (2, height_ / 2);
		context->line_to (width_ - 4, height_ / 2);

		context->stroke();
	}

protected:
	int width_;
	int height_;
};

class DashStyleMenuToolButton: public CustomMenuToolButton
{
public:
	typedef std::vector< double > menu_item_t;
	typedef GenericMenu< menu_item_t > menu_t;
	typedef menu_t::signal_item_selected_t signal_dash_style_selected_t;

public:
	DashStyleMenuToolButton();
	DashStyleMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~DashStyleMenuToolButton();

	void initialize();

	signal_dash_style_selected_t signalDashStyleSelected();

protected:
	menu_t* dash_style_menu_;
	menu_item_t selected_dash_style_;

	signal_dash_style_selected_t signal_dash_style_selected_;

protected:
	void drawImageSurface(const Cairo::RefPtr<Cairo::Context> context);

	void onDashStyleSelected(menu_item_t dash_style);
	void onButtonClicked();
};

#endif /* DASH_STYLE_MENU_TOOL_BUTTON_H_ */
