#ifndef FONTSIZECOMBOBOX_H
#define FONTSIZECOMBOBOX_H

#include <gtkmm/combobox.h> // Base class: Gtk::ComboBox

#include <gtkmm.h>

class FontSizeComboBox : public Gtk::ComboBox
{

public:
	FontSizeComboBox(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~FontSizeComboBox();

	int getActiveFontSize();
	void setActiveFontSize(int value);
	
protected:
	//Tree model columns:
	class FontSizeModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		FontSizeModelColumns()
		{
			add (font_size);
		}

		Gtk::TreeModelColumn< int > font_size;
	};

	FontSizeModelColumns columns_;
	Glib::RefPtr<Gtk::ListStore> font_size_tree_model_;
};

#endif // FONTSIZECOMBOBOX_H
