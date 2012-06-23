#include "main_window.h"

#include <glibmm/i18n.h>

#include <insert_element_tool.h>
#include <selection_tool.h>
#include <command_manager.h>

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	Gtk::Window(cobject), 
	lock_open_blank_page_( false ),
	builder_(builder)
{
	builder_->get_widget_derived ("diagrams_notebook", diagrams_notebook_);
	builder_->get_widget ("tools_filter_entry", tools_filter_entry_);
	builder_->get_widget_derived ("tools_iconview", tools_iconview_);
	builder_->get_widget ("current_status_label", current_status_label_);
	builder_->get_widget_derived ("text_font_combobox", font_combobox_);
	builder_->get_widget_derived ("text_font_size_combobox", font_size_combobox_);
	builder_->get_widget_derived ("text_color_menu_toolbutton", text_color_menu_toolbutton_);
	builder_->get_widget_derived ("text_backcolor_menu_toolbutton", text_backcolor_menu_toolbutton_);
	builder_->get_widget_derived ("line_color_menu_toolbutton", line_color_menu_toolbutton_);
	builder_->get_widget_derived ("line_width_menu_toolbutton", line_width_menu_toolbutton_);
	builder_->get_widget_derived ("fill_color_menu_toolbutton", fill_color_menu_toolbutton_);
	builder_->get_widget_derived("dash_style_menu_toolbutton", dash_style_menu_toolbutton_);
	builder_->get_widget ("main_toolbar", main_toolbar_);
	builder_->get_widget ("style_toolbar", style_toolbar_);
	builder_->get_widget ("text_format_toolbar", text_format_toolbar_);

	// Initialization	
	current_status_label_->set_text (Glib::ustring());

	tools_filter_entry_->set_icon_activatable (true, Gtk::ENTRY_ICON_SECONDARY);

	text_color_menu_toolbutton_->initialize(
		ColorMenu::DO_NOT_DISPLAY_NO_COLOR_ITEM,
		ColorMenu::DO_NOT_ALLOW_ALPHA,
		cppgef::Settings::getInstance()->getTextForegroundColor());
		
	text_backcolor_menu_toolbutton_->initialize(
		ColorMenu::DISPLAY_NO_COLOR_ITEM,
		ColorMenu::DO_NOT_ALLOW_ALPHA,
		cppgef::Settings::getInstance()->getTextBackgroundColor(), 
		cppgef::Settings::getInstance()->getTextHasBackgroundColor());

	line_color_menu_toolbutton_->initialize(
		ColorMenu::DISPLAY_NO_COLOR_ITEM, 
		ColorMenu::ALLOW_ALPHA,
		cppgef::Settings::getInstance()->getLineColor());

	line_width_menu_toolbutton_->initialize();

	dash_style_menu_toolbutton_->initialize();

	fill_color_menu_toolbutton_->initialize(
		ColorMenu::DISPLAY_NO_COLOR_ITEM, 
		ColorMenu::ALLOW_ALPHA,
		cppgef::Settings::getInstance()->getFillColor());

	// Actions
	file_new_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("file_new_action"));
	file_open_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("file_open_action"));
	file_save_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("file_save_action"));
	file_save_as_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("file_save_as_action"));
	file_save_a_copy_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("file_save_a_copy"));
	file_revert_to_saved_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("file_revert_to_saved"));
	file_close_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("file_close_action"));
	file_exit_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("file_exit_action"));

	edit_cut_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("edit_cut_action"));
	edit_copy_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("edit_copy_action"));
	edit_paste_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("edit_paste_action"));
	edit_delete_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("edit_delete_action"));
	edit_undo_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("edit_undo_action"));
	edit_redo_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("edit_redo_action"));
	edit_select_all_action_ = Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("edit_select_all_action"));

	view_main_toolbar_toggle_action_ = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic(builder_->get_object ("view_main_toolbar_action"));
	view_style_toolbar_toggle_action_ = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic(builder_->get_object ("view_style_toolbar_action"));
	view_text_format_toolbar_toggle_action_ = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic(builder_->get_object ("view_text_format_toolbar_action"));

	help_about_action_ =  Glib::RefPtr< Gtk::Action >::cast_dynamic(builder_->get_object ("help_about_action"));

	text_actiongroup_ = Glib::RefPtr< Gtk::ActionGroup >::cast_dynamic(builder_->get_object ("text_actiongroup"));
	text_bold_action_ = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic(builder_->get_object ("text_bold_toggleaction"));
	text_italic_action_ = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic(builder_->get_object ("text_italic_toggleaction"));
	text_underline_action_ = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic(builder_->get_object ("text_underline_toggleaction"));
	
	selection_toggleaction_ = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic(builder_->get_object ("selection_toggleaction"));
	
	// Activate selection tool
	shared_ptr< cppgef::SelectionTool > tool( new cppgef::SelectionTool() );
	Toolset::getInstance()->setActiveTool (tool);
	
	bindToolToDiagramEditor();

	// Initialize actions
	view_main_toolbar_toggle_action_->set_active (true);
	view_style_toolbar_toggle_action_->set_active (true);
	view_text_format_toolbar_toggle_action_->set_active (true);
	
	// Connect action signal handlers
	file_new_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onFileNewAction));
		
	file_open_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onFileOpenAction));

	file_save_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onFileSaveAction));

	file_save_as_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onFileSaveAsAction));

	file_save_a_copy_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onFileSaveACopyAction));

	file_revert_to_saved_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onFileRevertToSavedAction));

	file_close_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onFileCloseAction));

	file_exit_action_->signal_activate().connect (
			sigc::mem_fun(this, &MainWindow::onFileExitAction));

	edit_undo_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onEditUndoAction));

	edit_redo_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onEditRedoAction));

	edit_cut_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onEditCutAction));

	edit_copy_action_->signal_activate().connect(
		sigc::mem_fun(this, &MainWindow::onEditCopyAction));

	edit_paste_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onEditPasteAction));

	edit_delete_action_->signal_activate().connect (
		sigc::mem_fun(this, &MainWindow::onEditDeleteAction));

	edit_select_all_action_->signal_activate().connect (
			sigc::mem_fun(this, &MainWindow::onEditSelectAllAction));

	view_main_toolbar_toggle_action_->signal_toggled().connect (
			sigc::mem_fun(this, &MainWindow::onViewMainToolbarToggleAction));

	view_style_toolbar_toggle_action_->signal_toggled().connect (
			sigc::mem_fun(this, &MainWindow::onViewStyleToolbarToggleAction));

	view_text_format_toolbar_toggle_action_->signal_toggled().connect (
			sigc::mem_fun(this, &MainWindow::onViewTextFormatToolbarToggleAction));

	help_about_action_->signal_activate().connect (
			sigc::mem_fun(this, &MainWindow::onHelpAboutAction));

	text_bold_action_->signal_toggled().connect (
		sigc::mem_fun(this, &MainWindow::onTextBoldAction));
		
	text_italic_action_->signal_toggled().connect (
		sigc::mem_fun(this, &MainWindow::onTextItalicAction));

	text_underline_action_->signal_toggled().connect (
		sigc::mem_fun(this, &MainWindow::onTextUnderlineAction));
		
	selection_toggleaction_->signal_toggled().connect (
		sigc::mem_fun(this, &MainWindow::onSelectionToolAction));
		
	// Connect widget signal handlers
	tools_filter_entry_->signal_changed().connect (
		sigc::mem_fun(this, &MainWindow::onFilterEntryChanged));
		
	tools_filter_entry_->signal_icon_press().connect (
		sigc::mem_fun(this, &MainWindow::onFilterEntryClearPress));
		
	tools_iconview_->signal_selection_changed().connect (
		sigc::mem_fun(this, &MainWindow::onToolsIconViewSelectionChanged));

	diagrams_notebook_->signal_switch_page().connect (
		sigc::mem_fun(this, &MainWindow::onDiagramsNotebookSwitchPage));
		
	diagrams_notebook_->signalCloseDocument().connect (
			sigc::mem_fun(this, &MainWindow::onDiagramsNotebookCloseDocument));

	diagrams_notebook_->signalDocumentClosed().connect (
			sigc::mem_fun(this, &MainWindow::onDiagramsNotebookDocumentClosed));

	font_combobox_->property_popup_shown().signal_changed().connect(
		sigc::mem_fun( *this, &MainWindow::onFontComboboxPopupShownChanged ));
		
	font_combobox_->signal_changed().connect(
		sigc::mem_fun( *this, &MainWindow::onFontComboboxChanged ));
		
	font_size_combobox_->property_popup_shown().signal_changed().connect (
		sigc::mem_fun( *this, &MainWindow::onFontSizeComboboxPopupShownChanged ));
		
	font_size_combobox_->signal_changed().connect (
		sigc::mem_fun( *this, &MainWindow::onFontSizeComboboxChanged ));

	text_color_menu_toolbutton_->signal_show_menu().connect (
			sigc::mem_fun( *this, &MainWindow::onTextColorPopupShow ));

	text_color_menu_toolbutton_->get_menu()->signal_selection_done().connect(
			sigc::mem_fun( *this, &MainWindow::onTextColorPopupHide ));

	text_color_menu_toolbutton_->signalColorSelected().connect (
		sigc::mem_fun( *this, &MainWindow::onTextColorChanged ));

	text_backcolor_menu_toolbutton_->signal_show_menu().connect (
			sigc::mem_fun( *this, &MainWindow::onTextBackcolorPopupShow ));

	text_backcolor_menu_toolbutton_->get_menu()->signal_selection_done().connect (
			sigc::mem_fun( *this, &MainWindow::onTextBackcolorPopupHide ));

	text_backcolor_menu_toolbutton_->signalColorSelected().connect (
		sigc::mem_fun( *this, &MainWindow::onTextBackcolorChanged ));

	text_backcolor_menu_toolbutton_->signalNoColorSelected().connect (
		sigc::mem_fun( *this, &MainWindow::onTextBackcolorNoColorSelected ));

	line_color_menu_toolbutton_->signalColorSelected().connect (
		sigc::mem_fun( *this, &MainWindow::onLineColorChanged ));

	line_color_menu_toolbutton_->signalNoColorSelected().connect (
		sigc::mem_fun( *this, &MainWindow::onLineColorNoColorSelected ));

	line_width_menu_toolbutton_->signalLineWidthSelected().connect (
			sigc::mem_fun( *this, &MainWindow::onLineWidthSelected ));

	dash_style_menu_toolbutton_->signalDashStyleSelected().connect (
			sigc::mem_fun( *this, &MainWindow::onDashStyleSelected ));
			
	fill_color_menu_toolbutton_->signalColorSelected().connect (
		sigc::mem_fun( *this, &MainWindow::onFillColorChanged ));
		
	fill_color_menu_toolbutton_->signalNoColorSelected().connect (
		sigc::mem_fun( *this, &MainWindow::onFillColorNoColorSelected ));

	this->signal_delete_event().connect(
			sigc::mem_fun( *this, &MainWindow::onWindowDeleteEvent ));

	// Other signals
	cppgef::CommandManager::getInstance()->signalCommandExecute().connect (
			sigc::mem_fun( *this, &MainWindow::onCommandManagerCommandExecute ));

	cppgef::CommandManager::getInstance()->signalCommandUndo().connect (
			sigc::mem_fun( *this, &MainWindow::onCommandManagerCommandUndo ));

	cppgef::CommandManager::getInstance()->signalCommandRedo().connect (
			sigc::mem_fun( *this, &MainWindow::onCommandManagerCommandRedo ));

	cppgef::CommandManager::getInstance()->signalReachedSavedState().connect (
			sigc::mem_fun( *this, &MainWindow::onCommandManagerReachedSavedState ));

	// Open an empty diagram
	appendNotebookPage("");
	
	text_actiongroup_->set_sensitive (false);
	font_combobox_->set_sensitive (false);
	font_size_combobox_->set_sensitive (false);
	text_color_menu_toolbutton_->set_sensitive (false);
	text_backcolor_menu_toolbutton_->set_sensitive (false);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onFileNewAction()
{
	appendNotebookPage("");
}

void MainWindow::onFileOpenAction()
{
	Gtk::FileChooserDialog dialog(*this, _("Open diagram"),
		Gtk::FILE_CHOOSER_ACTION_OPEN);

	dialog.add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button (Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	
#ifdef GTKMM_3
	Glib::RefPtr< Gtk::FileFilter > filter_any = Gtk::FileFilter::create();
	filter_any->set_name (_("All files"));
	filter_any->add_pattern ("*");
#else
	Gtk::FileFilter filter_any;
	filter_any.set_name (_("All files"));
	filter_any.add_pattern ("*");
#endif

	dialog.add_filter (filter_any);
	
	if (dialog.run() == Gtk::RESPONSE_OK)
	{
		try
		{
			DiagramEditor* active_diagram_editor = getActiveDiagramEditor();

			if (active_diagram_editor)
			{
				if (!active_diagram_editor->isDirty() && active_diagram_editor->isEmpty())
				{
					bool prev_lock_status = lock_open_blank_page_;
					lock_open_blank_page_ = true;

					closeDocument (active_diagram_editor);

					lock_open_blank_page_ = prev_lock_status;
				}
			}

			appendNotebookPage(dialog.get_filename());
		}
		catch (const std::exception& e)
		{
			Gtk::MessageDialog msg(*this, _("Error opening diagram"), false,
				Gtk::MESSAGE_ERROR);
					
			msg.set_secondary_text(e.what());
			msg.run();      
		}
	}	
}

void MainWindow::onFileSaveAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();
	
	if (!active_diagram_editor)
		return;	
		
	if (active_diagram_editor->hasFile())
	{
		active_diagram_editor->save();

		shared_ptr< cppgef::Diagram > diagram = dynamic_pointer_cast< cppgef::Diagram >(active_diagram_editor->getDiagramEditPart()->getModel());
		diagrams_notebook_->setNotebookPageDirty (diagrams_notebook_->get_current_page(), !(cppgef::CommandManager::getInstance()->isReachedSavedState (diagram)));
	}
	else
	{
		onFileSaveAsAction();
	}
}

void MainWindow::onFileSaveAsAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();
	
	if (!active_diagram_editor)
		return;

	promptSaveDiagram (active_diagram_editor, false);
}

void MainWindow::onFileSaveACopyAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();

	if (!active_diagram_editor)
		return;

	promptSaveDiagram (active_diagram_editor, true);
}

void MainWindow::onFileRevertToSavedAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();

	if (!active_diagram_editor)
		return;

	if (!active_diagram_editor->hasFile())
		return;

	active_diagram_editor->reOpen();
}

void MainWindow::onFileCloseAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();

	if (!active_diagram_editor)
		return;

	closeDocument (active_diagram_editor);
}

void MainWindow::onFileExitAction()
{
	if (!closeAndPromptForSave())
		this->hide();
}

void MainWindow::onEditUndoAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();
	
	if (!active_diagram_editor)
		return;
	
	active_diagram_editor->undo();
}

void MainWindow::onEditRedoAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();
	
	if (!active_diagram_editor)
		return;
	
	active_diagram_editor->redo();
}

void MainWindow::onEditCutAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();

	if (!active_diagram_editor)
		return;

	active_diagram_editor->clipboardCut();
}

void MainWindow::onEditCopyAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();
	
	if (!active_diagram_editor)
		return;

	active_diagram_editor->clipboardCopy();
}

void MainWindow::onEditPasteAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();
	
	if (!active_diagram_editor)
		return;

	active_diagram_editor->clipboardPaste();
}

void MainWindow::onEditDeleteAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();
	
	if (!active_diagram_editor)
		return;

	active_diagram_editor->deleteSelection();
}

void MainWindow::onEditSelectAllAction()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();

	if (!active_diagram_editor)
		return;

	active_diagram_editor->selectAll();
}

void MainWindow::onViewMainToolbarToggleAction()
{
	main_toolbar_->set_visible (view_main_toolbar_toggle_action_->get_active());
}

void MainWindow::onViewStyleToolbarToggleAction()
{
	style_toolbar_->set_visible (view_style_toolbar_toggle_action_->get_active());
}

void MainWindow::onViewTextFormatToolbarToggleAction()
{
	text_format_toolbar_->set_visible (view_text_format_toolbar_toggle_action_->get_active());
}

void MainWindow::onHelpAboutAction()
{
	AboutDialog about;
	about.run();
}

void MainWindow::onTextBoldAction()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
		diagram_editor->setTextEditStyleBold (text_bold_action_->get_active());
}

void MainWindow::onTextItalicAction()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
		diagram_editor->setTextEditStyleItalic (text_italic_action_->get_active());
}

void MainWindow::onTextUnderlineAction()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
		diagram_editor->setTextEditStyleUnderline (text_underline_action_->get_active());
}

void MainWindow::onSelectionToolAction()
{
	// Exit if a tools in the iconview is selected and the toggle action
	// is not active
#ifdef GTKMM_3
	std::vector< Gtk::TreeModel::Path > selected_items = tools_iconview_->get_selected_items();
#else
	std::list< Gtk::TreeModel::Path > selected_items = tools_iconview_->get_selected_items();
#endif

	if (!selection_toggleaction_->get_active() && !selected_items.empty())
		return;

	// Exit if the selection tool is already active
	if (Toolset::getInstance()->isSelectionToolActive())
	{
		selection_toggleaction_->set_active (true);
		return;
	}
	
	tools_iconview_->unselect_all();
	
	shared_ptr< cppgef::SelectionTool > tool( new cppgef::SelectionTool() );
	
	if (insert_shape_tool_action_complete_connection_.connected())
		insert_shape_tool_action_complete_connection_.disconnect();
		
	Toolset::getInstance()->setActiveTool (tool);
	
	selection_toggleaction_->set_active (true);
	
	bindToolToDiagramEditor();
}

void MainWindow::appendNotebookPage(const std::string& file_name)
{
	DiagramEditor* diagram_editor = diagrams_notebook_->appendDocument (file_name);
	
	diagram_editor->signalSelectionChange().connect(
		sigc::mem_fun( this, &MainWindow::onSelectionChange ));

	diagram_editor->signalTextEditStartEdit().connect(
		sigc::mem_fun( this, &MainWindow::onTextEditStartEdit ));

	diagram_editor->signalTextEditEndEdit().connect(
		sigc::mem_fun( this, &MainWindow::onTextEditEndEdit ));

	diagram_editor->signalTextEditUpdateStyle().connect(
		sigc::mem_fun( this, &MainWindow::onTextEditUpdateStyle ));
}

DiagramEditor* MainWindow::getActiveDiagramEditor()
{
	int current_page = diagrams_notebook_->get_current_page();
	
	if (current_page == -1)
		return 0;
		
	Widget* current_page_widget = diagrams_notebook_->get_nth_page (current_page);
	if (current_page_widget == 0)
		return 0;

	Gtk::ScrolledWindow* diagram_scrolledwindow = dynamic_cast< Gtk::ScrolledWindow* >(current_page_widget);
	if (diagram_scrolledwindow == 0)
		return 0;
		
	Gtk::Viewport* diagram_viewport = dynamic_cast< Gtk::Viewport* >(diagram_scrolledwindow->get_child()); 
	if (diagram_viewport == 0)
		return 0;
		
	DiagramEditor* current_diagram_editor = dynamic_cast< DiagramEditor* >(diagram_viewport->get_child());
	
	return current_diagram_editor;
}

void MainWindow::bindToolToDiagramEditor()
{
	DiagramEditor* active_diagram_editor = getActiveDiagramEditor();
	
	if (!active_diagram_editor)
		return;
	
	Toolset::getInstance()->getActiveTool()->setDiagramEditPart (active_diagram_editor->getDiagramEditPart());	
}

void MainWindow::onFilterEntryChanged()
{
	tools_iconview_->setActiveFilter (tools_filter_entry_->get_text());
}

void MainWindow::onFilterEntryClearPress(Gtk::EntryIconPosition icon_position, const GdkEventButton* event)
{
	if (icon_position == Gtk::ENTRY_ICON_SECONDARY)
	{
		// Clear entry text, this cause the signal changed to
		// be emitted
		tools_filter_entry_->set_text(Glib::ustring(" ")); // FIXME: HACK! application crash on windows if set_text to empty string
		//tools_filter_entry_->set_text(Glib::ustring());
	}
}

void MainWindow::onToolsIconViewSelectionChanged()
{
#ifdef GTKMM_3
	std::vector< Gtk::TreeModel::Path > selected_items = tools_iconview_->get_selected_items();
#else
	std::list< Gtk::TreeModel::Path > selected_items = tools_iconview_->get_selected_items();
#endif

	if (selected_items.empty())
	{
		selection_toggleaction_->set_active (true);
		return;
	}
	
	const Gtk::TreeModel::Path& path = *selected_items.begin();
	
	//shared_ptr< cppgef::InsertShapeTool > tool( new cppgef::InsertShapeTool() );
	std::string class_type = tools_iconview_->getItemFromPath(path);

	shared_ptr< cppgef::ITool > tool = cppgef::EditPartFactory::getInstance()->getModelElementType(class_type)->createInsertTool();
	
	if (insert_shape_tool_action_complete_connection_.connected())
		insert_shape_tool_action_complete_connection_.disconnect();
		
	Toolset::getInstance()->setActiveTool (tool);
	
	selection_toggleaction_->set_active (false);
	
	insert_shape_tool_action_complete_connection_ = tool->signalActionComplete().connect (
		sigc::mem_fun( this, &MainWindow::onInsertShapeToolActionComplete ));
		
	bindToolToDiagramEditor();
}

#ifdef GTKMM_3
void MainWindow::onDiagramsNotebookSwitchPage(Gtk::Widget* page, guint page_num)
#else
void MainWindow::onDiagramsNotebookSwitchPage(GtkNotebookPage* page, guint page_num)
#endif
{
	bindToolToDiagramEditor();
	
	DiagramEditor* diagram_editor = getActiveDiagramEditor();

	if (diagram_editor)
	{
		file_revert_to_saved_action_->set_sensitive (diagram_editor->hasFile());

		shared_ptr< cppgef::Diagram > diagram = dynamic_pointer_cast< cppgef::Diagram >(
				diagram_editor->getDiagramEditPart()->getModel() );

		edit_undo_action_->set_sensitive (!(cppgef::CommandManager::getInstance()->isUndoStackEmpty (diagram)));
		edit_redo_action_->set_sensitive (!(cppgef::CommandManager::getInstance()->isRedoStackEmpty (diagram)));

		updateSelectionStatus();
	}
}

void MainWindow::onDiagramsNotebookCloseDocument(DiagramEditor* diagram_editor)
{
	closeDocument (diagram_editor);
}

void MainWindow::onDiagramsNotebookDocumentClosed()
{
	if (!lock_open_blank_page_ && diagrams_notebook_->get_n_pages() == 0)
	{
		appendNotebookPage("");
	}
}

void MainWindow::onSelectionChange(shared_ptr< cppgef::IEditPart > shape_edit_part)
{
	updateSelectionStatus();
}

void MainWindow::updateSelectionStatus()
{
	cppgef::IContainerEditPart::size_type selected_objects = getActiveDiagramEditor()->getDiagramEditPart()->getSelectedChildrenSize();
	
	if (selected_objects == 0)
	{
		current_status_label_->set_text (Glib::ustring());

		edit_cut_action_->set_sensitive (false);
		edit_copy_action_->set_sensitive (false);
		edit_delete_action_->set_sensitive (false);
	}
	else
	{
		current_status_label_->set_text (Glib::ustring::compose(_("Selected %1 object(s)"), selected_objects));


		edit_cut_action_->set_sensitive (true);
		edit_copy_action_->set_sensitive (true);
		edit_delete_action_->set_sensitive (true);
	}
}

void MainWindow::promptSaveDiagram(DiagramEditor* diagram_editor, bool copy)
{
	Gtk::FileChooserDialog dialog(*this, _("Save diagram"),
		Gtk::FILE_CHOOSER_ACTION_SAVE);

	dialog.add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button (Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	dialog.set_do_overwrite_confirmation();

#ifdef GTKMM_3
	Glib::RefPtr< Gtk::FileFilter > filter_any = Gtk::FileFilter::create();
	filter_any->set_name (_("All files"));
	filter_any->add_pattern ("*");
#else
	Gtk::FileFilter filter_any;
	filter_any.set_name (_("All files"));
	filter_any.add_pattern ("*");
#endif

	dialog.add_filter (filter_any);

	if (dialog.run() == Gtk::RESPONSE_OK)
	{
		try
		{
			if (copy)
			{
				diagram_editor->copy (dialog.get_filename());
			}
			else
			{
				diagram_editor->save (dialog.get_filename());

				shared_ptr< cppgef::Diagram > diagram = dynamic_pointer_cast< cppgef::Diagram >(diagram_editor->getDiagramEditPart()->getModel());
				diagrams_notebook_->setNotebookPageTitle (diagrams_notebook_->get_current_page(), dialog.get_filename());
				diagrams_notebook_->setNotebookPageDirty (diagrams_notebook_->get_current_page(), !(cppgef::CommandManager::getInstance()->isReachedSavedState (diagram)));

			}
		}
		catch (const std::exception& e)
		{
			Gtk::MessageDialog msg(*this, _("Error saving diagram"), false,
				Gtk::MESSAGE_ERROR);

			msg.set_secondary_text(e.what());
			msg.run();
		}
	}
}

void MainWindow::closeDocument(DiagramEditor* diagram_editor)
{
	if (diagram_editor->isDirty())
	{
		int page_num = diagrams_notebook_->getDiagramEditorPageNum (diagram_editor);
		Glib::ustring document_title = diagrams_notebook_->getNotebookPageTitle (page_num);

		Gtk::MessageDialog dialog(*this,
			Glib::ustring::compose(_("Save the changes to document \"%1\" before closing?"), document_title),
			false /* use_markup */, Gtk::MESSAGE_QUESTION,
			Gtk::BUTTONS_NONE);

		double elapsed_time_from_last_save = diagram_editor->getElapsedTimeFromLastSave();

		Glib::ustring last_save;

		if (elapsed_time_from_last_save > 60 * 60)
		{
			last_save = Glib::ustring::compose (_("%1 hours"), (int)(elapsed_time_from_last_save / (60 * 60)));
		}
		else if (elapsed_time_from_last_save > 60)
		{
			last_save = Glib::ustring::compose (_("%1 minutes"), (int)(elapsed_time_from_last_save / 60));
		}
		else
		{
			last_save = Glib::ustring::compose (_("%1 seconds"), elapsed_time_from_last_save);
		}

		dialog.set_secondary_text(
				Glib::ustring::compose(_("If you don't save, changes from the last %1 will be permanently lost."), last_save));

		// Add buttons
		dialog.add_button (_("Close _without saving"), Gtk::RESPONSE_CLOSE);

		dialog.add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

		if (diagram_editor->hasFile())
			dialog.add_button (Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
		else
			dialog.add_button (Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);


		// Display dialog
		int result = dialog.run();

		switch (result)
		{
		case Gtk::RESPONSE_CLOSE:
			// Close tab
			diagrams_notebook_->closeNotebookPage (diagram_editor);
			break;
		case Gtk::RESPONSE_CANCEL:
			break;
		case Gtk::RESPONSE_OK:
			// Save diagram, ask file name if thte diagram was never saved
			if (!diagram_editor->hasFile())
				promptSaveDiagram (diagram_editor, false);
			else
				diagram_editor->save();

			// Close tab
			diagrams_notebook_->closeNotebookPage (diagram_editor);

			break;
		}
	}
	else
	{
		// Close tab
		diagrams_notebook_->closeNotebookPage (diagram_editor);
	}
}

bool MainWindow::closeAndPromptForSave()
{
	bool prev_lock = lock_open_blank_page_;
	lock_open_blank_page_ = true;

	while (diagrams_notebook_->get_n_pages() > 0)
	{
		int pages_count_before_close = diagrams_notebook_->get_n_pages();

		DiagramEditor* diagram_editor = diagrams_notebook_->getDiagramEditor (0);

		closeDocument (diagram_editor);

		// If after close the page count is equal before the close
		// the user select to cancel
		if (pages_count_before_close == diagrams_notebook_->get_n_pages())
		{
			// Do not close the window if the user select cancel
			lock_open_blank_page_ = prev_lock;
			return true;
		}
	}

	// Close the window
	return false;
}

void MainWindow::onInsertShapeToolActionComplete()
{
	selection_toggleaction_->set_active (true);	
}

void MainWindow::onTextEditStartEdit(const cppgef::TextFormat& format)
{
	// TODO: Disconnect signals
	text_bold_action_->set_active (format.isBold());
	text_italic_action_->set_active (format.isItalic());
	text_underline_action_->set_active (format.isUnderline());
	
	font_combobox_->setActiveFontFamily (format.getFontFamily());
	font_size_combobox_->setActiveFontSize (format.getFontSize());
	
	text_actiongroup_->set_sensitive (true);
	font_combobox_->set_sensitive (true);
	font_size_combobox_->set_sensitive (true);
	text_color_menu_toolbutton_->set_sensitive (true);
	text_backcolor_menu_toolbutton_->set_sensitive (true);
}

void MainWindow::onTextEditUpdateStyle(const cppgef::TextFormat& format)
{
	// TODO: Disconnect signals
	text_bold_action_->set_active (format.isBold());
	text_italic_action_->set_active (format.isItalic());
	text_underline_action_->set_active (format.isUnderline());
	
	font_combobox_->setActiveFontFamily (format.getFontFamily());
	
	font_size_combobox_->setActiveFontSize (format.getFontSize());
}

void MainWindow::onTextEditEndEdit()
{
	text_actiongroup_->set_sensitive (false);
	font_combobox_->set_sensitive (false);
	font_size_combobox_->set_sensitive (false);
	text_color_menu_toolbutton_->set_sensitive (false);
	text_backcolor_menu_toolbutton_->set_sensitive (false);
}

void MainWindow::onFontComboboxPopupShownChanged()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		if (font_combobox_->property_popup_shown())
			diagram_editor->beginExternalEdit();
		else
			diagram_editor->endExternalEdit();
	}
}

void MainWindow::onFontComboboxChanged()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		Glib::ustring active_font_family = font_combobox_->getActiveFontFamily();
		
		if (!active_font_family.empty())
			diagram_editor->setTextEditFontFamily (active_font_family);
	}	
}

void MainWindow::onFontSizeComboboxPopupShownChanged()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		if (font_size_combobox_->property_popup_shown().get_value())
			diagram_editor->beginExternalEdit();
		else
			diagram_editor->endExternalEdit();
	}
}

void MainWindow::onFontSizeComboboxChanged()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		int active_font_size = font_size_combobox_->getActiveFontSize();
		
		if (active_font_size != 0)
			diagram_editor->setTextEditFontSize (active_font_size);
	}	
}

void MainWindow::onTextColorChanged(cppgef::Color color)
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->setTextEditTextColor (color);
		
		cppgef::Settings::getInstance()->setTextForegroundColor (color);
	}
}

void MainWindow::onTextColorPopupShow()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->beginExternalEdit();
	}
}

void MainWindow::onTextColorPopupHide()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->endExternalEdit();
	}
}

void MainWindow::onTextBackcolorChanged(cppgef::Color color)
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->setTextEditTextBackColor (color);
		
		cppgef::Settings::getInstance()->setTextBackgroundColor (color);
		cppgef::Settings::getInstance()->setTextHasBackgroundColor (true);
	}
}

void MainWindow::onTextBackcolorNoColorSelected()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->setTextEditTextBackColorNone();
		
		cppgef::Settings::getInstance()->setTextHasBackgroundColor (false);
	}
}

void MainWindow::onTextBackcolorPopupShow()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->beginExternalEdit();
	}
}

void MainWindow::onTextBackcolorPopupHide()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->endExternalEdit();
	}
}

void MainWindow::onLineColorChanged(cppgef::Color color)
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->setLineColor (color);
		
		cppgef::Settings::getInstance()->setLineColor (color);
	}	
}

void MainWindow::onLineColorNoColorSelected()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		cppgef::Color transparent;
		transparent.setRGBA (0, 0, 0, 0);
		diagram_editor->setLineColor (transparent);
		
		cppgef::Settings::getInstance()->setLineColor (transparent);
	}		
}

void MainWindow::onLineWidthSelected(double width)
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->setLineWidth (width);

		cppgef::Settings::getInstance()->setLineWidth (width);
	}
}

void MainWindow::onFillColorChanged(cppgef::Color color)
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->setFillColor (color);
		
		cppgef::Settings::getInstance()->setFillColor (color);
	}		
}

void MainWindow::onFillColorNoColorSelected()
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		cppgef::Color transparent;
		transparent.setRGBA (0, 0, 0, 0);
		diagram_editor->setFillColor (transparent);
		
		cppgef::Settings::getInstance()->setFillColor (transparent);
	}		
}

void MainWindow::onDashStyleSelected(std::vector< double > dash_style)
{
	DiagramEditor* diagram_editor = getActiveDiagramEditor();
	if (diagram_editor)
	{
		diagram_editor->setDashStyle (dash_style, 0.0);

		cppgef::Settings::getInstance()->setLineDashStyle (dash_style);
		cppgef::Settings::getInstance()->setLineDashStyleOffset (0.0);
	}	
}

bool MainWindow::onWindowDeleteEvent(GdkEventAny* event)
{
	return closeAndPromptForSave();
}

void MainWindow::onCommandManagerCommandExecute(shared_ptr< cppgef::Diagram > diagram)
{
	int page_num = diagrams_notebook_->getDiagramEditorPageNum (diagram);

	if (page_num == diagrams_notebook_->get_current_page())
	{
		edit_undo_action_->set_sensitive (!(cppgef::CommandManager::getInstance()->isUndoStackEmpty (diagram)));
		edit_redo_action_->set_sensitive (!(cppgef::CommandManager::getInstance()->isRedoStackEmpty (diagram)));

		diagrams_notebook_->setNotebookPageDirty (page_num, !(cppgef::CommandManager::getInstance()->isReachedSavedState (diagram)));
	}
}

void MainWindow::onCommandManagerCommandUndo(shared_ptr< cppgef::Diagram > diagram)
{
	int page_num = diagrams_notebook_->getDiagramEditorPageNum (diagram);

	if (page_num == diagrams_notebook_->get_current_page())
	{
		edit_undo_action_->set_sensitive (!(cppgef::CommandManager::getInstance()->isUndoStackEmpty (diagram)));
		edit_redo_action_->set_sensitive (!(cppgef::CommandManager::getInstance()->isRedoStackEmpty (diagram)));

		diagrams_notebook_->setNotebookPageDirty (page_num, !(cppgef::CommandManager::getInstance()->isReachedSavedState (diagram)));
	}
}

void MainWindow::onCommandManagerCommandRedo(shared_ptr< cppgef::Diagram > diagram)
{
	int page_num = diagrams_notebook_->getDiagramEditorPageNum (diagram);

	if (page_num == diagrams_notebook_->get_current_page())
	{
		edit_undo_action_->set_sensitive (!(cppgef::CommandManager::getInstance()->isUndoStackEmpty (diagram)));
		edit_redo_action_->set_sensitive (!(cppgef::CommandManager::getInstance()->isRedoStackEmpty (diagram)));

		diagrams_notebook_->setNotebookPageDirty (page_num, !(cppgef::CommandManager::getInstance()->isReachedSavedState (diagram)));
	}
}

void MainWindow::onCommandManagerReachedSavedState(shared_ptr< cppgef::Diagram > diagram)
{

}


