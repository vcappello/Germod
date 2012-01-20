#ifndef TEXTCOLORMENUTOOLBUTTON_H
#define TEXTCOLORMENUTOOLBUTTON_H

#include "color_menu_tool_button_base.h"

class TextColorMenuToolButton : public ColorMenuToolButtonBase
{
public:
	TextColorMenuToolButton();
	TextColorMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	
	virtual ~TextColorMenuToolButton();
	
	virtual void drawImageSurface(const Cairo::RefPtr< Cairo::Context > context);
	
protected:
	Glib::RefPtr< Gdk::Pixbuf > pixbuf_;
};

#endif // TEXTCOLORMENUTOOLBUTTON_H
