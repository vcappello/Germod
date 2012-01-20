#ifndef LINEFILLCOLORMENUTOOLBUTTON_H
#define LINEFILLCOLORMENUTOOLBUTTON_H

#include <color_menu_tool_button_base.h> // Base class: ColorMenuToolButtonBase

class FillColorMenuToolButton : public ColorMenuToolButtonBase
{

public:
	FillColorMenuToolButton();
	FillColorMenuToolButton(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~FillColorMenuToolButton();

public:
	virtual void drawImageSurface(const Cairo::RefPtr<Cairo::Context> context);
	
protected:
	Glib::RefPtr< Gdk::Pixbuf > pixbuf_;
};

#endif // LINEFILLCOLORMENUTOOLBUTTON_H
