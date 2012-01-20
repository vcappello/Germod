/*
 * custom_menu_tool_button.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: vincenzo
 */

#include "custom_menu_tool_button.h"

const int IMAGE_WIDTH = 22;
const int IMAGE_HEIGHT = 22;

CustomMenuToolButton::CustomMenuToolButton()
{
}

CustomMenuToolButton::CustomMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	Gtk::MenuToolButton( cobject )
{
}

CustomMenuToolButton::~CustomMenuToolButton()
{
}

int CustomMenuToolButton::getImageWidth() const
{
	return IMAGE_WIDTH;
}

int CustomMenuToolButton::getImageHeight() const
{
	return IMAGE_HEIGHT;
}

void CustomMenuToolButton::createImageWidget()
{
	// Create cairo context
	image_surface_ = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, IMAGE_WIDTH, IMAGE_HEIGHT);

	Cairo::RefPtr< Cairo::Context > image_context = Cairo::Context::create(image_surface_);

	drawImageSurface (image_context);

	// Create a pixbuf
#ifdef GTKMM_3
	image_pixbuf_ = Gdk::Pixbuf::create (image_surface_, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
#else
	image_pixbuf_ = Gdk::Pixbuf::create_from_data (image_surface_->get_data(),
		Gdk::COLORSPACE_RGB, true, 8, IMAGE_WIDTH, IMAGE_HEIGHT, image_surface_->get_stride());
#endif

	image_ = Gtk::manage(new Gtk::Image(image_pixbuf_));

	image_->show();

	this->set_icon_widget (*image_);
}


