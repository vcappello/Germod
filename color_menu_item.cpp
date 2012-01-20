/*
 * ColorMenuItem.cpp
 *
 *  Created on: Oct 4, 2011
 *      Author: vincenzo
 */

#include "color_menu_item.h"

ColorMenuItem::ColorMenuItem(const cppgef::Color& color) :
	Gtk::MenuItem( *Gtk::manage(color_drawing_area_ = new Gtk::DrawingArea()) ),
	active_( false ),
	color_( color )
{
	color_drawing_area_->set_size_request(22, 22);

#ifdef GTKMM_3
	color_drawing_area_->signal_draw().connect(
			sigc::mem_fun(*this, &ColorMenuItem::onColorDrawingAreaDraw));
#else
	color_drawing_area_->signal_expose_event().connect (
			sigc::mem_fun( *this, &ColorMenuItem::onColorDrawingAreaExposeEvent));
#endif
}

ColorMenuItem::~ColorMenuItem()
{
}

int ColorMenuItem::isActive() const
{
	return active_;
}

void ColorMenuItem::setActive(bool value)
{
	active_ = value;
}

cppgef::Color ColorMenuItem::getColor() const
{
	return color_;
}

void ColorMenuItem::setColor(const cppgef::Color& value)
{
	color_ = value;
}

#ifdef GTKMM_3

bool ColorMenuItem::onColorDrawingAreaDraw(const Cairo::RefPtr< Cairo::Context > context)
{
	
#else

bool ColorMenuItem::onColorDrawingAreaExposeEvent(GdkEventExpose* event)
{
	Cairo::RefPtr<Cairo::Context> context = color_drawing_area_->get_window()->create_cairo_context();
	
#endif

	Gtk::Allocation a = color_drawing_area_->get_allocation();
	context->rectangle(0, 0, a.get_width(), a.get_height());
	
	color_.useInCairo (context);

	context->fill();

	if (active_)
	{
		int x = 3;
		int y = 3;
		
		context->move_to (x + 2, y + 3);
		context->line_to (x + 5, y + 6);
		context->line_to (x + 8, y + 0);
		
		context->set_line_width (3.0);
		context->set_source_rgb (1, 1, 1);
		context->stroke_preserve();

		context->set_line_width (1.0);
		context->set_source_rgb (0, 0, 0);
		context->stroke();
	}

	return true;
}


