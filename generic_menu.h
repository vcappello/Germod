/*
 * generic_menu.h
 *
 *  Created on: Nov 22, 2011
 *      Author: vincenzo
 */

#ifndef GENERIC_MENU_H_
#define GENERIC_MENU_H_

#include <gtkmm.h>

#include <iostream>

template< class T >
class GenericItemRenderer
{
public:
	GenericItemRenderer()
	{
	}
	virtual ~GenericItemRenderer()
	{
	}

	virtual int getImageWidth() const = 0;
	virtual int getImageHeight() const = 0;

	virtual void drawImageSurface(const Cairo::RefPtr< Cairo::Context > context, const T& item_value) = 0;
};

template< class T >
class GenericMenuItem : public Gtk::MenuItem
{
public:
	GenericMenuItem(const T& item_value, GenericItemRenderer< T >& renderer) :
		Gtk::MenuItem( *createImageWidget (item_value, renderer ) ),
		active_( false ),
		item_value_( item_value )
	{
	}

	virtual ~GenericMenuItem()
	{
		std::cout << "Destroy" << std::endl;
	}

	int isActive() const { return active_; }
	void setActive(bool value) { active_ = value; }

	T getItemValue() const { return item_value_; }

protected:
	bool active_;
	T item_value_;

protected:
	Gtk::Image* createImageWidget(const T& item_value, GenericItemRenderer< T >& renderer)
	{
		Glib::RefPtr< Gdk::Pixbuf > image_pixbuf;
		Gtk::Image* image;
		Cairo::RefPtr< Cairo::ImageSurface > image_surface;

		// Create cairo context
		image_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, renderer.getImageWidth(), renderer.getImageHeight());

		Cairo::RefPtr< Cairo::Context > image_context = Cairo::Context::create(image_surface);

		renderer.drawImageSurface (image_context, item_value);

		// Create a pixbuf
#ifdef GTKMM_3

		image_pixbuf = Gdk::Pixbuf::create (image_surface, 0, 0, renderer.getImageWidth(), renderer.getImageHeight());
		
		image = Gtk::manage(new Gtk::Image(image_pixbuf));
		
#else

		image_pixbuf = Gdk::Pixbuf::create_from_data (image_surface->get_data(),
			Gdk::COLORSPACE_RGB, true, 8, 
			renderer.getImageWidth(), renderer.getImageHeight(), 
			image_surface->get_stride());

		// Take a copy of the Pixmap (gtkmm bug?)
		image = Gtk::manage(new Gtk::Image(image_pixbuf->copy()));
		
#endif

		return image;
	}
};

template< class T >
class GenericMenu : public Gtk::Menu
{
public:
	typedef sigc::signal< void, T > signal_item_selected_t;

public:
	GenericMenu() :
		active_menu_item_( 0 )
	{
	}

	virtual ~GenericMenu()
	{
	}

	GenericMenuItem< T >* getActiveMenuItem() { return active_menu_item_; }

	void addItem(const T& item_value, GenericItemRenderer< T >& renderer)
	{
		GenericMenuItem< T >* menu_item = Gtk::manage(new GenericMenuItem< T >( item_value, renderer ));
		this->append (*menu_item);

		menu_item->signal_activate().connect(
			sigc::bind< GenericMenuItem< T >* >(
				sigc::mem_fun( *this, &GenericMenu::onMenuItemActivate ),
				menu_item));
	}

	signal_item_selected_t signalItemSelected() { return signal_item_selected_; }

protected:
	GenericMenuItem< T >* active_menu_item_;

	// Signal instance
	signal_item_selected_t signal_item_selected_;

protected:
	void onMenuItemActivate(GenericMenuItem< T >* menu_item)
	{
		active_menu_item_ = menu_item;
		signal_item_selected_.emit (menu_item->getItemValue());
	}
};

#endif /* GENERIC_MENU_H_ */
