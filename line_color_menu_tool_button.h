#ifndef LINECOLORMENUTOOLBUTTON_H
#define LINECOLORMENUTOOLBUTTON_H

#include <color_menu_tool_button_base.h> // Base class: ColorMenuToolButtonBase

class LineColorMenuToolButton : public ColorMenuToolButtonBase
{

public:
	LineColorMenuToolButton();
	LineColorMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~LineColorMenuToolButton();

public:
	virtual void drawImageSurface(const Cairo::RefPtr<Cairo::Context> context);
	
protected:
	Glib::RefPtr< Gdk::Pixbuf > pixbuf_;
};

#endif // LINECOLORMENUTOOLBUTTON_H
