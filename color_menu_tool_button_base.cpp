/*
 * color_menu_tool_button_base.cpp
 *
 *  Created on: Oct 11, 2011
 *      Author: vincenzo
 */

#include "color_menu_tool_button_base.h"

ColorMenuToolButtonBase::ColorMenuToolButtonBase()
{
}

ColorMenuToolButtonBase::ColorMenuToolButtonBase(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	CustomMenuToolButton( cobject, builder )
{
}

ColorMenuToolButtonBase::~ColorMenuToolButtonBase()
{
}

void ColorMenuToolButtonBase::initialize(ColorMenu::DISPLAY_NO_COLOR_TYPE display_no_color, ColorMenu::ALLOW_ALPHA_TYPE allow_alpha, const cppgef::Color& default_color, bool deafault_has_color/*=true*/)
{
	selected_color_ = default_color;
	has_color_ = deafault_has_color;

	createImageWidget();

	text_color_menu_ = Gtk::manage(new ColorMenu());
	text_color_menu_->createItems(display_no_color, allow_alpha);

	this->property_menu() = text_color_menu_;

	text_color_menu_->signalColorSelected().connect(
		sigc::mem_fun( *this, &ColorMenuToolButtonBase::onColorMenuColorSelected ));

	text_color_menu_->signalNoColorSelected().connect(
		sigc::mem_fun( *this, &ColorMenuToolButtonBase::onColorMenuNoColorSelected ));

	this->signal_clicked().connect (
		sigc::mem_fun( *this, &ColorMenuToolButtonBase::onButtonColorSelected ));
}

void ColorMenuToolButtonBase::onColorMenuColorSelected(cppgef::Color color)
{
	selected_color_ = color;
	has_color_ = true;

	createImageWidget();

	signal_color_selected_.emit (selected_color_);
}

void ColorMenuToolButtonBase::onColorMenuNoColorSelected()
{
	has_color_ = false;

	createImageWidget();

	signal_no_color_selected_.emit();
}

void ColorMenuToolButtonBase::onButtonColorSelected()
{
	if (has_color_)
		signal_color_selected_.emit (selected_color_);
	else
		signal_no_color_selected_.emit();
}

void ColorMenuToolButtonBase::drawColorRectangle(const Cairo::RefPtr< Cairo::Context > context, double x, double y, double w, double h)
{
	if (has_color_)
	{
		context->rectangle (0, getImageHeight() - 5, getImageWidth(), 5);
#ifdef GTKMM_3
		selected_color_.useInCairo (context);
#else	
		selected_color_.useInCairoRev (context);
#endif	
		context->fill();
	}
	else
	{
		context->set_line_width (1.0);
		context->rectangle (0.5, getImageHeight() - 4.5, getImageWidth() - 0.5, 5 - 0.5);

		context->set_source_rgb (0, 0, 0);
		context->stroke();
	}	
}


