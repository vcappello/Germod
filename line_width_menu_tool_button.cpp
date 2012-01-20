/*
 * line_width_menu_tool_button.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: vincenzo
 */

#include "line_width_menu_tool_button.h"

#include <settings.h>

LineWidthMenuToolButton::LineWidthMenuToolButton()
{
}

LineWidthMenuToolButton::LineWidthMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	CustomMenuToolButton( cobject, builder )
{
}

LineWidthMenuToolButton::~LineWidthMenuToolButton()
{
}

void LineWidthMenuToolButton::initialize()
{
	selected_line_width_ = cppgef::Settings::getInstance()->getLineWidth();

	createImageWidget();

	line_width_menu_ = Gtk::manage(new menu_t());
	this->property_menu() = line_width_menu_;
	
	LineWidthRenderer line_width_renderer( 48, 22 );
	
	line_width_menu_->addItem (0.5, line_width_renderer);
	line_width_menu_->addItem (1, line_width_renderer);
	line_width_menu_->addItem (1.5, line_width_renderer);
	line_width_menu_->addItem (2, line_width_renderer);
	line_width_menu_->addItem (3, line_width_renderer);
	line_width_menu_->addItem (4, line_width_renderer);
	line_width_menu_->addItem (5, line_width_renderer);
	line_width_menu_->addItem (8, line_width_renderer);
	line_width_menu_->addItem (10, line_width_renderer);
	line_width_menu_->show_all();
	
	line_width_menu_->signalItemSelected().connect (
		sigc::mem_fun( *this, &LineWidthMenuToolButton::onLineWidthSelected ));
}

LineWidthMenuToolButton::signal_line_width_selected_t LineWidthMenuToolButton::signalLineWidthSelected()
{
	return signal_line_width_selected_;
}

void LineWidthMenuToolButton::drawImageSurface(const Cairo::RefPtr<Cairo::Context> context)
{
	context->set_source_rgb (0, 0, 0);

	context->set_line_width (selected_line_width_);

	context->move_to (2, 11);
	context->line_to (20, 11);

	context->stroke();
}

void LineWidthMenuToolButton::onLineWidthSelected(menu_item_t width)
{
	selected_line_width_ = width;

	createImageWidget();

	signal_line_width_selected_.emit (selected_line_width_);
}


