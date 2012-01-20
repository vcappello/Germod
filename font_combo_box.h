#ifndef FONTCOMBOBOX_H
#define FONTCOMBOBOX_H

#include <gtkmm/combobox.h> // Base class: Gtk::ComboBox

#include <gtkmm.h>

class FontComboBox : public Gtk::ComboBox
{

public:
	FontComboBox(BaseObjectType* cobject,
	              const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~FontComboBox();

	Glib::ustring getActiveFontFamily();
	void setActiveFontFamily(const Glib::ustring& font_family);
	
protected:
	//Tree model columns:
	class FontModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		FontModelColumns()
		{
			add (font_family);
		}

		Gtk::TreeModelColumn< Glib::ustring > font_family;
	};

	FontModelColumns columns_;
	Glib::RefPtr<Gtk::ListStore> font_tree_model_;
};

#endif // FONTCOMBOBOX_H
