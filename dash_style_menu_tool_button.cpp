/*
 * dash_style_menu_tool_button.cpp
 *
 *  Created on: Nov 22, 2011
 *      Author: vincenzo
 */

#include "dash_style_menu_tool_button.h"

#include <settings.h>

DashStyleMenuToolButton::DashStyleMenuToolButton()
{
}

DashStyleMenuToolButton::DashStyleMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	CustomMenuToolButton( cobject, builder )
{
}

DashStyleMenuToolButton::~DashStyleMenuToolButton()
{
}

void DashStyleMenuToolButton::initialize()
{
	selected_dash_style_ = cppgef::Settings::getInstance()->getLineDashStyle();

	createImageWidget();

	dash_style_menu_ = Gtk::manage(new menu_t());
	this->property_menu() = dash_style_menu_;

	DashStyleRenderer dash_style_renderer( 48, 22 );

	std::vector< double > no_dash;
	dash_style_menu_->addItem (no_dash, dash_style_renderer);

	std::vector< double > small_dashes; small_dashes.push_back (5); small_dashes.push_back (5);
	dash_style_menu_->addItem (small_dashes, dash_style_renderer);

	std::vector< double > dashes; dashes.push_back (10); dashes.push_back (10);
	dash_style_menu_->addItem (dashes, dash_style_renderer);

	std::vector< double > dot_dashes; dot_dashes.push_back (5); dot_dashes.push_back (3); dot_dashes.push_back (1); dot_dashes.push_back (3);
	dash_style_menu_->addItem (dot_dashes, dash_style_renderer);

	std::vector< double > dotted; dotted.push_back (1); dotted.push_back (5);
	dash_style_menu_->addItem (dotted, dash_style_renderer);

	dash_style_menu_->show_all();

	dash_style_menu_->signalItemSelected().connect (
		sigc::mem_fun( *this, &DashStyleMenuToolButton::onDashStyleSelected ));
}

DashStyleMenuToolButton::signal_dash_style_selected_t DashStyleMenuToolButton::signalDashStyleSelected()
{
	return signal_dash_style_selected_;
}

void DashStyleMenuToolButton::drawImageSurface(const Cairo::RefPtr<Cairo::Context> context)
{
	context->set_source_rgb (0, 0, 0);

	context->set_dash (selected_dash_style_, 0.0);

	context->move_to (2, 11);
	context->line_to (20, 11);

	context->stroke();
}

void DashStyleMenuToolButton::onDashStyleSelected(menu_item_t dash_style)
{
	selected_dash_style_ = dash_style;

	createImageWidget();

	signal_dash_style_selected_.emit (selected_dash_style_);
}
