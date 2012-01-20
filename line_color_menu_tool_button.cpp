#include "line_color_menu_tool_button.h"

const std::string PIXBUF_FILE_NAME("color-line.png");

LineColorMenuToolButton::LineColorMenuToolButton()
{
	pixbuf_ = Gdk::Pixbuf::create_from_file (PIXBUF_FILE_NAME);
}

LineColorMenuToolButton::LineColorMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	ColorMenuToolButtonBase( cobject, builder)
{
	pixbuf_ = Gdk::Pixbuf::create_from_file (PIXBUF_FILE_NAME);
}

LineColorMenuToolButton::~LineColorMenuToolButton()
{
}

void LineColorMenuToolButton::drawImageSurface(const Cairo::RefPtr<Cairo::Context> context)
{
	Gdk::Cairo::set_source_pixbuf (context, pixbuf_, 3.0, 0.0);
    context->paint();
	
	drawColorRectangle (context, 0.5, getImageHeight() - 4.5, getImageWidth() - 0.5, 5 - 0.5);
}
