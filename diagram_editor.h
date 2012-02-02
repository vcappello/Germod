#ifndef DIAGRAMEDITOR_H
#define DIAGRAMEDITOR_H

#include <gtkmm.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <ctime>

#include <point.h>
#include <diagram_edit_part.h>
#include <i_diagram_editor.h>
#include <text_format.h>
#include <color.h>
#include <command_manager.h>
#include <clipboard_copy_command.h>
#include <clipboard_paste_command.h>
#include <delete_command.h>

#include "toolset.h"

using boost::shared_ptr;

class DiagramEditor : public Gtk::Fixed, public cppgef::IDiagramEditor
{
public:
	typedef sigc::signal< void, const cppgef::TextFormat& > signal_text_edit_start_edit_t;
	typedef sigc::signal< void, const cppgef::TextFormat& > signal_text_edit_update_style_t;
	typedef sigc::signal< void > signal_text_edit_end_edit_t;
	typedef sigc::signal< void, shared_ptr< cppgef::IEditPart > > signal_selection_change_t;
	
public:
	DiagramEditor();
	DiagramEditor(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~DiagramEditor();

	virtual void activateInplaceEditor(shared_ptr< cppgef::IInplaceEditor > editor, const cppgef::Point& point);
	virtual void clearSelection();
	virtual void selectAll();

	shared_ptr< cppgef::DiagramEditPart > getDiagramEditPart();
	void setDiagramEditPart (shared_ptr< cppgef::DiagramEditPart > value);
	void setDiagramEditPart (shared_ptr< cppgef::DiagramEditPart > value, const std::string& file_name);
	
	bool hasFile() const;
	std::string getFileName() const;
	
	double getElapsedTimeFromLastSave() const;

	bool save();
	bool save(const std::string& file_name);
	bool copy(const std::string& file_name);
	
	bool open(const std::string& file_name);
	bool reOpen();
	
	bool isInplaceEditingActive();
	
	bool isDirty() const;
	bool isEmpty() const;

	void cancelInplaceEditing();
	void stopInplaceEditing();
	
	void setTextEditStyleBold(bool value);
	void setTextEditStyleItalic(bool value);
	void setTextEditStyleUnderline(bool value);
	void setTextEditFontFamily(const Glib::ustring& font_family);
	void setTextEditFontSize(const int font_size);
	void setTextEditTextColor(const cppgef::Color& color);
	void setTextEditTextBackColor(const cppgef::Color& color);
	void setTextEditTextBackColorNone();
	
	void setLineColor(const cppgef::Color& color);
	void setLineWidth (double width);
	void setFillColor(const cppgef::Color& color);
	void setDashStyle (const std::vector< double >& dash_style, double dash_offset);
	
	void beginExternalEdit();
	void endExternalEdit();
	
	void redraw();

	void undo();
	void redo();
	
	void clipboardCut();
	void clipboardCopy();
	void clipboardPaste();
	
	void deleteSelection();
	
	signal_text_edit_start_edit_t signalTextEditStartEdit();
	signal_text_edit_update_style_t signalTextEditUpdateStyle();
	signal_text_edit_end_edit_t signalTextEditEndEdit();
	signal_selection_change_t signalSelectionChange();
	
protected:
	shared_ptr< cppgef::DiagramEditPart > diagram_edit_part_;
	std::string file_name_;
	Gtk::DrawingArea* drawing_area_;
	shared_ptr< cppgef::IInplaceEditor > inplace_editor_;
	bool external_editing_;
	time_t last_save_time_;
	
	signal_text_edit_start_edit_t signal_text_edit_start_edit_;
	signal_text_edit_update_style_t signal_text_edit_update_style_;
	signal_text_edit_end_edit_t signal_text_edit_end_edit_;
	signal_selection_change_t signal_selection_change_;
	
	sigc::connection editor_widget_focus_out_connection_;
	
protected:
	virtual void on_size_allocate(Gtk::Allocation& allocation);

	bool onDrawingAreaButtonPressEvent(GdkEventButton* event);
	bool onDrawingAreaButtonReleaseEvent(GdkEventButton* event);
	bool onDrawingAreaMotionNotifyEvent(GdkEventMotion* event);
	bool onDrawingAreaKeyPressEvent(GdkEventKey* event);
	bool onDrawingAreaKeyReleaseEvent(GdkEventKey* event);

#ifdef GTKMM_3
	bool onDrawingAreaDraw(const Cairo::RefPtr< Cairo::Context >& context);
#else
	bool onDrawingAreaExposeEvent(GdkEventExpose* event);
#endif
	
	bool onEditorWidgetFocusOutEvent(GdkEventFocus* event);
	
	void onTextEditorUpdateStyle(const cppgef::TextFormat& text_format);

	void onInplaceEditorStopEdit();
	
	void onSelectionChange(shared_ptr< cppgef::IEditPart > shape_edit_part);
	
private:
	void initialize();
};

#endif // DIAGRAMEDITOR_H
