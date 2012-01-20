/*
 * custom_menu_tool_button.h
 *
 *  Created on: Nov 10, 2011
 *      Author: vincenzo
 */

#ifndef CUSTOM_MENU_TOOL_BUTTON_H_
#define CUSTOM_MENU_TOOL_BUTTON_H_

#include <gtkmm.h>

class CustomMenuToolButton: public Gtk::MenuToolButton
{
public:
	CustomMenuToolButton();
	CustomMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~CustomMenuToolButton();

	int getImageWidth() const;
	int getImageHeight() const;

protected:
	Glib::RefPtr< Gdk::Pixbuf > image_pixbuf_;
	Gtk::Image* image_;
	Cairo::RefPtr< Cairo::ImageSurface > image_surface_;

protected:
	void createImageWidget();

	virtual void drawImageSurface(const Cairo::RefPtr< Cairo::Context > context) = 0;
};

#endif /* CUSTOM_MENU_TOOL_BUTTON_H_ */
