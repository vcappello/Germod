/*
 * ColorMenuItem.h
 *
 *  Created on: Oct 4, 2011
 *      Author: vincenzo
 */

#ifndef COLORMENUITEM_H_
#define COLORMENUITEM_H_

#include <gtkmm.h>

#include <color.h>

class ColorMenuItem : public Gtk::MenuItem
{
public:
	ColorMenuItem(const cppgef::Color& color);

	virtual ~ColorMenuItem();

	int isActive() const;
	void setActive(bool value);
	
	cppgef::Color getColor() const;
	void setColor(const cppgef::Color& value);
	
protected:
	Gtk::DrawingArea* color_drawing_area_;
	bool active_;
	
	cppgef::Color color_;

protected:

#ifdef GTKMM_3
	bool onColorDrawingAreaDraw(const Cairo::RefPtr< Cairo::Context > context);
#else
	bool onColorDrawingAreaExposeEvent(GdkEventExpose* event);	
#endif

};

#endif /* COLORMENUITEM_H_ */
