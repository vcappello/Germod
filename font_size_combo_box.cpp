#include "font_size_combo_box.h"

FontSizeComboBox::FontSizeComboBox(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	Gtk::ComboBox( cobject )
{
	font_size_tree_model_ = Gtk::ListStore::create(columns_);
	
	font_size_tree_model_->set_sort_column(columns_.font_size, Gtk::SORT_ASCENDING);
	
	set_model(font_size_tree_model_);
	
	Gtk::TreeModel::Row row = *(font_size_tree_model_->append()); row[columns_.font_size] = 6; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 8; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 10; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 11; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 12; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 14; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 16; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 18; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 24; 
	row = *(font_size_tree_model_->append()); row[columns_.font_size] = 32; 
	
	Gtk::CellRendererText* renderer = Gtk::manage (new Gtk::CellRendererText());
	pack_start (*renderer, true);
	
	add_attribute(renderer->property_text(), columns_.font_size);	
}

FontSizeComboBox::~FontSizeComboBox()
{
}

int FontSizeComboBox::getActiveFontSize()
{
	Gtk::TreeModel::iterator iter = get_active();
	if(iter)
	{
		Gtk::TreeModel::Row row = *iter;

		return row[columns_.font_size];
	}
	
	return 0;
}

void FontSizeComboBox::setActiveFontSize(int value)
{
	if (value == 0)
	{
		unset_active();
		return;
	}
	
    typedef Gtk::TreeModel::Children Children;
    Children children = font_size_tree_model_->children();
    for (Children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if (row[columns_.font_size] == value)
		{
			set_active (iter);
			break;
		}
	}
}

