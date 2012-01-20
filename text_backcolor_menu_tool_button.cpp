/*
 * text_backcolor_menu_tool_button.cpp
 *
 *  Created on: Oct 11, 2011
 *      Author: vincenzo
 */

#include "text_backcolor_menu_tool_button.h"

const std::string PIXBUF_FILE_NAME("color-text-background.png");

TextBackcolorMenuToolButton::TextBackcolorMenuToolButton() :
	ColorMenuToolButtonBase()
{
	pixbuf_ = Gdk::Pixbuf::create_from_file (PIXBUF_FILE_NAME);
}

TextBackcolorMenuToolButton::TextBackcolorMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	ColorMenuToolButtonBase( cobject, builder)
{
	pixbuf_ = Gdk::Pixbuf::create_from_file (PIXBUF_FILE_NAME);
}

TextBackcolorMenuToolButton::~TextBackcolorMenuToolButton()
{
}

void TextBackcolorMenuToolButton::drawImageSurface(const Cairo::RefPtr< Cairo::Context > context)
{
	Gdk::Cairo::set_source_pixbuf (context, pixbuf_, 3.0, 0.0);
    context->paint();

	drawColorRectangle (context, 0.5, getImageHeight() - 4.5, getImageWidth() - 0.5, 5 - 0.5);
}

