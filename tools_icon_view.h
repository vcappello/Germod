#ifndef TOOLSICONVIEW_H
#define TOOLSICONVIEW_H

#include <gtkmm.h>

#include <edit_part_factory.h>

class ToolsIconView : public Gtk::IconView
{

public:
	ToolsIconView(BaseObjectType* cobject,
	              const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~ToolsIconView();

	Glib::ustring getActiveFilter();
	void setActiveFilter(const Glib::ustring& value);
	
	void loadModel();
	
	std::string getItemFromPath(const Gtk::TreeModel::Path& path);
	
protected:
	Glib::RefPtr<Gtk::Builder> builder_;
	Glib::ustring active_filter_;
	std::vector< Glib::ustring > filter_tokens_;
	
protected:
	class ModelColumns : public Gtk::TreeModelColumnRecord
	{
	public:
		Gtk::TreeModelColumn<std::string> class_type;
		Gtk::TreeModelColumn<Glib::ustring> display_name;
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> >  pixbuf;

		ModelColumns()
		{
			add(class_type);
			add(display_name);
			add(pixbuf);
		}
	};

	bool filterFunc(const Gtk::TreeModel::const_iterator& iter);
	
protected:
	const ModelColumns columns_;
	Glib::RefPtr< Gtk::ListStore > model_;
	Glib::RefPtr< Gtk::TreeModelFilter > model_filter_;
};

#endif // TOOLSICONVIEW_H
