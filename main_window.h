#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtkmm.h>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <diagram.h>
#include <diagram_edit_part.h>
#include <color.h>
#include <settings.h>

#include "diagram_editor.h"
#include "tools_icon_view.h"
#include "toolset.h"
#include "font_combo_box.h"
#include "font_size_combo_box.h"
#include "text_color_menu_tool_button.h"
#include "text_backcolor_menu_tool_button.h"
#include "line_color_menu_tool_button.h"
#include "fill_color_menu_tool_button.h"
#include "line_width_menu_tool_button.h"
#include "dash_style_menu_tool_button.h"
#include "document_notebook.h"

using boost::shared_ptr;
using boost::dynamic_pointer_cast;

class MainWindow: public Gtk::Window
{
public:
	MainWindow(BaseObjectType* cobject,
	           const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~MainWindow();

protected:
	bool lock_open_blank_page_;
	Glib::RefPtr<Gtk::Builder> builder_;
	DocumentNotebook* diagrams_notebook_;
	Gtk::Entry* tools_filter_entry_;
	ToolsIconView* tools_iconview_;
	Gtk::Label* current_status_label_;
	FontComboBox * font_combobox_;
	FontSizeComboBox* font_size_combobox_;
	TextColorMenuToolButton* text_color_menu_toolbutton_;
	TextBackcolorMenuToolButton* text_backcolor_menu_toolbutton_;
	LineColorMenuToolButton* line_color_menu_toolbutton_;
	LineWidthMenuToolButton* line_width_menu_toolbutton_;
	FillColorMenuToolButton* fill_color_menu_toolbutton_;
	DashStyleMenuToolButton* dash_style_menu_toolbutton_;
	
	Glib::RefPtr< Gtk::UIManager > ui_manager_;
	Glib::RefPtr< Gtk::ActionGroup > edit_action_group_;

	Glib::RefPtr< Gtk::Action > file_new_action_;
	Glib::RefPtr< Gtk::Action > file_open_action_;
	Glib::RefPtr< Gtk::Action > file_save_action_;
	Glib::RefPtr< Gtk::Action > file_save_as_action_;
	Glib::RefPtr< Gtk::Action > file_save_a_copy_action_;
	Glib::RefPtr< Gtk::Action > file_revert_to_saved_action_;
	Glib::RefPtr< Gtk::Action > file_close_action_;
	Glib::RefPtr< Gtk::Action > file_exit_action_;

	Glib::RefPtr< Gtk::Action > edit_undo_action_;
	Glib::RefPtr< Gtk::Action > edit_redo_action_;
	Glib::RefPtr< Gtk::Action > edit_cut_action_;
	Glib::RefPtr< Gtk::Action > edit_delete_action_;
	Glib::RefPtr< Gtk::Action > edit_copy_action_;
	Glib::RefPtr< Gtk::Action > edit_paste_action_;
	Glib::RefPtr< Gtk::Action > edit_select_all_action_;

	Glib::RefPtr< Gtk::ActionGroup > text_actiongroup_;
	Glib::RefPtr< Gtk::ToggleAction > text_bold_action_;
	Glib::RefPtr< Gtk::ToggleAction > text_italic_action_;
	Glib::RefPtr< Gtk::ToggleAction > text_underline_action_;

	Glib::RefPtr< Gtk::ToggleAction > selection_toggleaction_;

	sigc::connection insert_shape_tool_action_complete_connection_;
	
protected:
	void appendNotebookPage(const std::string& file_name);

	DiagramEditor* getActiveDiagramEditor();

	void bindToolToDiagramEditor();
	void updateSelectionStatus();
	
	void promptSaveDiagram(DiagramEditor* diagram_editor, bool copy);
	void closeDocument(DiagramEditor* diagram_editor);

private:
	// Signal handlers
	void onFileNewAction();
	void onFileOpenAction();
	void onFileSaveAction();
	void onFileSaveAsAction();
	void onFileSaveACopyAction();
	void onFileRevertToSavedAction();
	void onFileCloseAction();
	
	void onEditCutAction();
	void onEditCopyAction();
	void onEditPasteAction();
	void onEditDeleteAction();
	void onEditUndoAction();
	void onEditRedoAction();
	void onEditSelectAllAction();
	
	void onTextBoldAction();
	void onTextItalicAction();
	void onTextUnderlineAction();
	
	void onSelectionToolAction();
	
	void onFilterEntryChanged();
	void onFilterEntryClearPress(Gtk::EntryIconPosition icon_position, const GdkEventButton* event);
	
	void onToolsIconViewSelectionChanged();

#ifdef GTKMM_3
	void onDiagramsNotebookSwitchPage(Gtk::Widget* page, guint page_num);
#else
	void onDiagramsNotebookSwitchPage(GtkNotebookPage* page, guint page_num);
#endif
	
	void onDiagramsNotebookCloseDocument(DiagramEditor* diagram_editor);
	void onDiagramsNotebookDocumentClosed();

	void onSelectionChange(shared_ptr< cppgef::IEditPart > shape_edit_part);
	
	void onInsertShapeToolActionComplete();
	
	void onTextEditStartEdit(const cppgef::TextFormat& format);
	void onTextEditUpdateStyle( const cppgef::TextFormat& format);
	void onTextEditEndEdit();
	
	void onFontComboboxPopupShownChanged();
	void onFontComboboxChanged();
	void onFontSizeComboboxPopupShownChanged();
	void onFontSizeComboboxChanged();

	void onTextColorChanged(cppgef::Color color);
	void onTextColorPopupShow();
	void onTextColorPopupHide();

	void onTextBackcolorChanged(cppgef::Color color);
	void onTextBackcolorNoColorSelected();
	void onTextBackcolorPopupShow();
	void onTextBackcolorPopupHide();
	
	void onLineColorChanged(cppgef::Color color);
	void onLineColorNoColorSelected();
	void onLineWidthSelected(double width);
	void onFillColorChanged(cppgef::Color color);
	void onFillColorNoColorSelected();
	void onDashStyleSelected(std::vector< double > dash_style);

	void onCommandManagerCommandExecute(shared_ptr< cppgef::Diagram > diagram);
	void onCommandManagerCommandUndo(shared_ptr< cppgef::Diagram > diagram);
	void onCommandManagerCommandRedo(shared_ptr< cppgef::Diagram > diagram);
	void onCommandManagerReachedSavedState(shared_ptr< cppgef::Diagram > diagram);
};

#endif /* MAINWINDOW_H_ */
