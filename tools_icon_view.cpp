#include "tools_icon_view.h"

#include <boost/algorithm/string.hpp>
#include <algorithm>

ToolsIconView::ToolsIconView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	Gtk::IconView(cobject), 
	builder_(builder)
{
	loadModel();
	
	set_text_column (columns_.display_name);
	set_pixbuf_column (columns_.pixbuf);
}

ToolsIconView::~ToolsIconView()
{
}

Glib::ustring ToolsIconView::getActiveFilter()
{
	return active_filter_;
}

void ToolsIconView::setActiveFilter(const Glib::ustring& value)
{
	active_filter_ = value;
	
	// Compare the filter ignoring case
	Glib::ustring filter = active_filter_.lowercase();
	
	// Split filter in tokens using whitespace
	boost::split(filter_tokens_, filter, boost::is_any_of(" \a\b\f\n\r\t\v"));
	
	// Remove empty tokens
	filter_tokens_.erase ((
		std::remove_if (
				filter_tokens_.begin(),
				filter_tokens_.end(),
				std::bind2nd(
						std::equal_to< Glib::ustring >(),
						Glib::ustring("")))
		),
		filter_tokens_.end());
		
	// Apply new filter
	model_filter_->refilter();
}

void ToolsIconView::loadModel()
{
	model_ = Gtk::ListStore::create(columns_);

	for (cppgef::EditPartFactory::iterator i = cppgef::EditPartFactory::getInstance()->getTypesBegin();
		i != cppgef::EditPartFactory::getInstance()->getTypesEnd();
		i++)
		{
			if (!i->second->isRoot())
			{
				Gtk::TreeModel::iterator iter = model_->append();
				Gtk::TreeModel::Row row = *iter;
				row[columns_.class_type] = i->second->getClassType();
				row[columns_.display_name] = i->second->getDisplayName();
				row[columns_.pixbuf] = i->second->getDisplayImage(); 
			}
		}
		
	model_filter_ = Gtk::TreeModelFilter::create (model_);
	
	model_filter_->set_visible_func (sigc::mem_fun(this, &ToolsIconView::filterFunc));
	
	set_model (model_filter_);
}

bool ToolsIconView::filterFunc(const Gtk::TreeModel::const_iterator& iter)
{
	// If the filter has no token display all items
	if (filter_tokens_.empty())
		return true;
		
	Gtk::TreeModel::Row row = *iter;
	
	// Convert the current item value to lower for ignore case
	Glib::ustring current_value = static_cast<Glib::ustring>(row[columns_.display_name]).lowercase();
	
	// Search any of the filter token, display the current item
	// if a match was found
	for (std::vector< Glib::ustring >::iterator s = filter_tokens_.begin(); s != filter_tokens_.end(); s++)
	{
		if (current_value.find (*s) != Glib::ustring::npos)
			return true;
	}
	
	return false;
}

std::string ToolsIconView::getItemFromPath(const Gtk::TreeModel::Path& path)
{
	Gtk::TreeModel::Path child_path = model_filter_->convert_path_to_child_path (path);
	
	const Gtk::TreeModel::iterator iter = model_->get_iter (child_path);	
	std::string class_type;
	
	if (iter)
	{
		Gtk::TreeModel::Row row = *iter;
		
		class_type = row[columns_.class_type];
	}
	
	return class_type;
}

