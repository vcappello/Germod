#include "document_notebook.h"

#include <glibmm/i18n.h>

#include <command_manager.h>
#include <edit_part_factory.h>

DocumentNotebook::DocumentNotebook(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	Gtk::Notebook( cobject ),
	unsaved_counter_( 1 )
{
}

DocumentNotebook::~DocumentNotebook()
{
}

DiagramEditor* DocumentNotebook::appendDocument(const std::string& file_name)
{
	Glib::ustring page_title = getFileTitle (file_name); 

	// New empty model element
	shared_ptr< cppgef::Diagram > diagram( new cppgef::Diagram() );
	
	shared_ptr< cppgef::DiagramEditPart > diagram_edit_part = 
		dynamic_pointer_cast< cppgef::DiagramEditPart >(
			cppgef::EditPartFactory::getInstance()->createEditPart (diagram));

	if (!file_name.empty())
	{
		diagram_edit_part->open (file_name);
		
		// The model is the one created by the EditPart, instead
		// of the empty model created before
		diagram = dynamic_pointer_cast< cppgef::Diagram >( diagram_edit_part->getModel() );
	}
		
	// Page title
	Gtk::HBox* page_title_hbox = Gtk::manage( new Gtk::HBox() );

	Gtk::Label* page_dirty_label = Gtk::manage( new Gtk::Label( "*" ) );
	page_dirty_label->set_name ("page_dirty_label");

	Gtk::Label* page_title_label = Gtk::manage( new Gtk::Label( page_title ) );
	page_title_label->set_name ("page_title_label");

	Gtk::Button* page_close_button = Gtk::manage( new Gtk::Button() );
	page_close_button->set_name ("page_close_button");
	page_close_button->set_relief (Gtk::RELIEF_NONE);
	page_close_button->add (*Gtk::manage( new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU) ));

	page_title_hbox->pack_start (*page_dirty_label);
	page_title_hbox->pack_start (*page_title_label);
	page_title_hbox->pack_start (*page_close_button, Gtk::PACK_SHRINK);
	
	page_title_hbox->show_all();
	page_dirty_label->hide();
	
	// Page contents
	DiagramEditor* diagram_editor = Gtk::manage( new DiagramEditor() );
	
	int width = 0; int height = 0;
	diagram_editor->get_size_request(width, height);

	Gtk::ScrolledWindow* diagram_scrolledwindow = Gtk::manage(new Gtk::ScrolledWindow());
	diagram_scrolledwindow->set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

#ifdef GTKMM_3
	Glib::RefPtr< Gtk::Adjustment > ha = Gtk::Adjustment::create (0, 0, width);
	Glib::RefPtr< Gtk::Adjustment > va = Gtk::Adjustment::create (0, 0, height);
	
	Gtk::Viewport* diagram_viewport = Gtk::manage(new Gtk::Viewport(ha, va));
#else
	Gtk::Adjustment* ha = manage (new Gtk::Adjustment (0, 0, width));
    Gtk::Adjustment* va = manage (new Gtk::Adjustment (0, 0, height));

	Gtk::Viewport* diagram_viewport = Gtk::manage(new Gtk::Viewport(*ha, *va));
#endif

	diagram_scrolledwindow->add (*diagram_viewport);
	
	diagram_scrolledwindow->set_border_width (6);
	
	diagram_viewport->set_shadow_type (Gtk::SHADOW_NONE);
	diagram_scrolledwindow->set_shadow_type (Gtk::SHADOW_IN);
	
	diagram_viewport->add (*diagram_editor);

	// Append the new page
	int page_num = this->append_page (*diagram_scrolledwindow, *page_title_hbox);
	
	// Open a new command stack for the new diagram
	cppgef::CommandManager::getInstance()->openStack (diagram);
	
	// Bind DiagramEditPart to DiagramEditor
	if (!file_name.empty())
	{
		diagram_editor->setDiagramEditPart (dynamic_pointer_cast< cppgef::DiagramEditPart >(diagram_edit_part), file_name);
	}
	else
	{
		diagram_editor->setDiagramEditPart (dynamic_pointer_cast< cppgef::DiagramEditPart >(diagram_edit_part));
	}

	show_all_children();	
	
	// Set the new page as the current page,
	// need to do this after show_all_children
	this->set_current_page (page_num);

	page_close_button->signal_clicked().connect (
		sigc::bind< DiagramEditor* >(
			sigc::mem_fun( *this, &DocumentNotebook::onCloseButtonClicked ),
			diagram_editor));
		
	
	return diagram_editor;
}

int DocumentNotebook::getDiagramEditorPageNum (DiagramEditor* diagram_editor)
{
	// Retrieve the ScrolledWindow container for given DiagramEditor
	Gtk::Widget* view_port = diagram_editor->get_parent();

	if (view_port)
	{
		Gtk::Widget* scrolled_window = view_port->get_parent();
		if (scrolled_window)
		{
			return page_num (*scrolled_window);
		}
	}

	return -1;
}

int DocumentNotebook::getDiagramEditorPageNum(shared_ptr< cppgef::Diagram > diagram)
{
	for (int i=0; i < get_n_pages(); i++)
	{
		DiagramEditor* diagram_editor = getDiagramEditor (i);
		if (!diagram_editor)
			return -1;

		if (diagram_editor->getDiagramEditPart()->getModel() == diagram)
			return i;
	}

	return -1;
}

DiagramEditor* DocumentNotebook::getDiagramEditor(int page_num)
{
	Gtk::ScrolledWindow* scrolled_window = dynamic_cast< Gtk::ScrolledWindow* >( get_nth_page (page_num) );
	if (!scrolled_window)
		return 0;

	Gtk::Viewport* view_port = dynamic_cast< Gtk::Viewport* >( scrolled_window->get_child() );
	if (!view_port)
		return 0;

	DiagramEditor* diagram_editor = dynamic_cast< DiagramEditor* >( view_port->get_child() );

	return diagram_editor;
}

Glib::ustring DocumentNotebook::getNotebookPageTitle(int nth_page)
{
	Widget* child = this->get_nth_page (nth_page);
	if (!child)
		return "";

	Widget* label = this->get_tab_label (*child);
	if (!label)
		return "";

	Gtk::HBox* page_title_hbox = dynamic_cast< Gtk::HBox* >(label);
	if (!page_title_hbox)
		return "";

	Glib::ListHandle< Widget* > label_children = page_title_hbox->get_children();

	typedef Glib::ListHandle< Widget* >::iterator itor_t;
	for (itor_t witor = label_children.begin(); witor != label_children.end(); witor++)
	{
		if ((*witor)->get_name() == "page_title_label")
		{
			Gtk::Label* page_title_label = dynamic_cast< Gtk::Label* >((*witor));

			if (page_title_label)
				return page_title_label->get_text();
			else
				return "";
		}
	}

	return "";
}

void DocumentNotebook::setNotebookPageTitle(int nth_page, const std::string& file_name)
{
	Widget* child = this->get_nth_page (nth_page);
	if (!child)
		return;

	Widget* label = this->get_tab_label (*child);
	if (!label)
		return;

	Gtk::HBox* page_title_hbox = dynamic_cast< Gtk::HBox* >(label);
	if (!page_title_hbox)
		return;
		
	Glib::ListHandle< Widget* > label_children = page_title_hbox->get_children();

	typedef Glib::ListHandle< Widget* >::iterator itor_t;
	for (itor_t witor = label_children.begin(); witor != label_children.end(); witor++)
	{
		if ((*witor)->get_name() == "page_title_label")
		{
			Gtk::Label* page_title_label = dynamic_cast< Gtk::Label* >((*witor));

			if (page_title_label)
				page_title_label->set_text (getFileTitle (file_name));
		}
	}
}

void DocumentNotebook::setNotebookPageDirty(int nth_page, bool value)
{
	Widget* child = this->get_nth_page (nth_page);
	if (!child)
		return;

	Widget* label = this->get_tab_label (*child);
	if (!label)
		return;

	Gtk::HBox* page_title_hbox = dynamic_cast< Gtk::HBox* >(label);
	if (!page_title_hbox)
		return;

	Glib::ListHandle< Widget* > label_children = page_title_hbox->get_children();

	typedef Glib::ListHandle< Widget* >::iterator itor_t;
	for (itor_t witor = label_children.begin(); witor != label_children.end(); witor++)
	{
		if ((*witor)->get_name() == "page_dirty_label")
		{
			Gtk::Label* page_dirty_label = dynamic_cast< Gtk::Label* >((*witor));

			if (page_dirty_label)
			{
				if (value)
				{
					page_dirty_label->show();
				}
				else
				{
					page_dirty_label->hide();
				}
			}
		}
	}
	
}

void DocumentNotebook::closeNotebookPage(DiagramEditor* diagram_editor)
{
	int page_num = getDiagramEditorPageNum (diagram_editor);

	if (page_num != -1)
	{
		closeNotebookPage (page_num);
	}
}

void DocumentNotebook::closeNotebookPage(int page_num)
{
	remove_page (page_num);

	signal_document_closed_.emit();
}

DocumentNotebook::signal_close_document_t DocumentNotebook::signalCloseDocument()
{
	return signal_close_document_;
}

DocumentNotebook::signal_document_closed_t DocumentNotebook::signalDocumentClosed()
{
	return signal_document_closed_;
}

Glib::ustring DocumentNotebook::getFileTitle(const std::string& file_name)
{
	if (file_name.empty())
	{
		Glib::ustring unsaved_title = Glib::ustring("<") + Glib::ustring(_("Unsaved diagram"));
		unsaved_title.append (" ");
		unsaved_title.append (Glib::ustring::format(unsaved_counter_));
		unsaved_title.append (">");
		
		unsaved_counter_++;		
		
		return unsaved_title;
	
	}
		
	Glib::RefPtr< Gio::File > file = Gio::File::create_for_path (file_name);
	if(!file)
		return file_name; //Actually an error.

	return file->get_basename();	
}

void DocumentNotebook::onCloseButtonClicked(DiagramEditor* diagram_editor)
{
	signal_close_document_.emit (diagram_editor);
}

