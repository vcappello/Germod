#include <gtkmm.h>
#include <iostream>

#include "main_window.h"

int main(int argc, char **argv)
{
	Gtk::Main kit(argc, argv);

	//Load the GtkBuilder file and instantiate its widgets:
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	try
	{
		builder->add_from_file("main_window.ui");
	} catch (const Glib::FileError& ex)
	{
		std::cerr << "FileError: " << ex.what() << std::endl;
		return 1;
	} catch (const Gtk::BuilderError& ex)
	{
		std::cerr << "BuilderError: " << ex.what() << std::endl;
		return 1;
	}

	//Get the GtkBuilder-instantiated Dialog:
	MainWindow* main_window = 0;
	builder->get_widget_derived("main_window", main_window);
	if (main_window)
	{
		kit.run(*main_window);
	}

	return 0;
}
