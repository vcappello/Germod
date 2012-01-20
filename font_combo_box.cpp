#include "font_combo_box.h"

FontComboBox::FontComboBox(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	Gtk::ComboBox( cobject )
{
	font_tree_model_ = Gtk::ListStore::create(columns_);
	
	font_tree_model_->set_sort_column(columns_.font_family, Gtk::SORT_ASCENDING);
	
	set_model(font_tree_model_);
	
	Glib::RefPtr< Pango::Context > pc = this->get_pango_context();
    if (pc) 
	{
		Glib::ArrayHandle<Glib::RefPtr<Pango::FontFamily> > families = pc->list_families();
		for(Glib::ArrayHandle< Glib::RefPtr< Pango::FontFamily > >::iterator it = families.begin(); it < families.end(); it++) 
		{
			if ((*it)->get_name() != "Cambria Math")
			{
				Gtk::TreeModel::Row row = *(font_tree_model_->append());
				row[columns_.font_family] = (*it)->get_name();
			}
		}
	} 	
	
	Gtk::CellRendererText* renderer = Gtk::manage (new Gtk::CellRendererText());
	pack_start (*renderer, true);
	
	add_attribute(renderer->property_text(), columns_.font_family);
	add_attribute(renderer->property_family(), columns_.font_family);
}

FontComboBox::~FontComboBox()
{
}

Glib::ustring FontComboBox::getActiveFontFamily()
{
	Gtk::TreeModel::iterator iter = get_active();
	if(iter)
	{
		Gtk::TreeModel::Row row = *iter;

		return row[columns_.font_family];
	}
	
	return "";
}

void FontComboBox::setActiveFontFamily(const Glib::ustring& font_family)
{
	if (font_family.empty())
	{
		unset_active();
		return;
	}
	
    typedef Gtk::TreeModel::Children Children;
    Children children = font_tree_model_->children();
    for (Children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if (row[columns_.font_family] == font_family)
		{
			set_active (iter);
			break;
		}
	}
}


