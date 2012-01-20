/*
 * text_backcolor_menu_tool_button.h
 *
 *  Created on: Oct 11, 2011
 *      Author: vincenzo
 */

#ifndef TEXT_BACKCOLOR_MENU_TOOL_BUTTON_H_
#define TEXT_BACKCOLOR_MENU_TOOL_BUTTON_H_

#include "color_menu_tool_button_base.h"

class TextBackcolorMenuToolButton: public ColorMenuToolButtonBase
{
public:
	TextBackcolorMenuToolButton();
	TextBackcolorMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

	virtual ~TextBackcolorMenuToolButton();

	virtual void drawImageSurface(const Cairo::RefPtr< Cairo::Context > context);
	
protected:
	Glib::RefPtr< Gdk::Pixbuf > pixbuf_;
};

#endif /* TEXT_BACKCOLOR_MENU_TOOL_BUTTON_H_ */
