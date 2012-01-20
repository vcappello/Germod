#ifndef DOCUMENTNOTEBOOK_H
#define DOCUMENTNOTEBOOK_H

#include <gtkmm.h>
#include <string>

#include <diagram_edit_part.h>

#include "diagram_editor.h"

class DocumentNotebook : public Gtk::Notebook
{
public:
	typedef sigc::signal< void, DiagramEditor* > signal_close_document_t;
	typedef sigc::signal< void > signal_document_closed_t;

public:
	DocumentNotebook(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~DocumentNotebook();
	
	DiagramEditor* appendDocument(const std::string& file_name);
	
	int getDiagramEditorPageNum(DiagramEditor* diagram_editor);
	int getDiagramEditorPageNum(shared_ptr< cppgef::Diagram > diagram);

	Glib::ustring getNotebookPageTitle(int nth_page);

	void setNotebookPageTitle(int nth_page, const std::string& file_name);

	void setNotebookPageDirty(int nth_page, bool value);
	
	void closeNotebookPage(DiagramEditor* diagram_editor);

	signal_close_document_t signalCloseDocument();
	signal_document_closed_t signalDocumentClosed();
	
protected:
	int unsaved_counter_;
	
	signal_close_document_t signal_close_document_;
	signal_document_closed_t signal_document_closed_;
	
protected:
	Glib::ustring getFileTitle(const std::string& file_name);
	
	void onCloseButtonClicked(DiagramEditor* diagram_editor);
};

#endif // DOCUMENTNOTEBOOK_H
