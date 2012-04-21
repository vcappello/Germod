/*
 * line_width_menu_tool_button.h
 *
 *  Created on: Nov 10, 2011
 *      Author: vincenzo
 */

#ifndef LINE_WIDTH_MENU_TOOL_BUTTON_H_
#define LINE_WIDTH_MENU_TOOL_BUTTON_H_

#include "custom_menu_tool_button.h"

#include "generic_menu.h"

class LineWidthRenderer : public GenericItemRenderer< double >
{
public:
	LineWidthRenderer(int width, int height) :
		width_( width ),
		height_( height )
	{
	}

	virtual int getImageWidth() const { return width_; }
	virtual int getImageHeight() const { return height_; }

	virtual void drawImageSurface(const Cairo::RefPtr< Cairo::Context > context, const double& item_value)
	{
		context->set_source_rgb (0, 0, 0);

		context->set_line_width (item_value);

		context->move_to (2, height_ / 2);
		context->line_to (width_ - 4, height_ / 2);

		context->stroke();
	}

protected:
	int width_;
	int height_;
};

class LineWidthMenuToolButton: public CustomMenuToolButton
{
public:
	typedef double menu_item_t;
	typedef GenericMenu< menu_item_t > menu_t;
	typedef menu_t::signal_item_selected_t signal_line_width_selected_t;

public:
	LineWidthMenuToolButton();
	LineWidthMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~LineWidthMenuToolButton();

	void initialize();

	signal_line_width_selected_t signalLineWidthSelected();

protected:
	menu_t* line_width_menu_;
	menu_item_t selected_line_width_;

	signal_line_width_selected_t signal_line_width_selected_;

protected:
	void drawImageSurface(const Cairo::RefPtr<Cairo::Context> context);

	void onLineWidthSelected(menu_item_t width);
	void onButtonClicked();
};

#endif /* LINE_WIDTH_MENU_TOOL_BUTTON_H_ */
