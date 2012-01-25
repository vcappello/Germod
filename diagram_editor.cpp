#include "diagram_editor.h"

#include <text_inplace_editor.h>
#include <gtk/gtk.h>

DiagramEditor::DiagramEditor()
{
	initialize();	
}

DiagramEditor::DiagramEditor(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) :
	Gtk::Fixed( cobject )
{
	initialize();
}

DiagramEditor::~DiagramEditor()
{
}

void DiagramEditor::initialize()
{
	last_save_time_ = time(0);

	external_editing_ = false;
	
	drawing_area_ = Gtk::manage(new Gtk::DrawingArea() );
	
	// GTKMM_3
#ifdef GTKMM_3
	drawing_area_->set_can_focus (true);

	drawing_area_->signal_draw().connect (sigc::mem_fun( *this,
		&DiagramEditor::onDrawingAreaDraw ));
#else
	drawing_area_->set_flags (Gtk::CAN_FOCUS);

	drawing_area_->signal_expose_event().connect (sigc::mem_fun( *this,
		&DiagramEditor::onDrawingAreaExposeEvent ));
#endif

	drawing_area_->signal_button_press_event().connect (sigc::mem_fun( *this, 
		&DiagramEditor::onDrawingAreaButtonPressEvent ));

	drawing_area_->signal_button_release_event().connect (sigc::mem_fun( *this, 
		&DiagramEditor::onDrawingAreaButtonReleaseEvent ));

	drawing_area_->signal_motion_notify_event().connect (sigc::mem_fun( *this, 
		&DiagramEditor::onDrawingAreaMotionNotifyEvent ));
	
	drawing_area_->set_size_request (800, 600);
	
	this->set_size_request (800, 600);
	
	this->put (*drawing_area_, 0, 0);

	//drawing_area_->set_size_request (400, 300);
	
	drawing_area_->add_events(Gdk::KEY_PRESS_MASK
		| Gdk::BUTTON_PRESS_MASK
		| Gdk::BUTTON_RELEASE_MASK
		| Gdk::BUTTON_MOTION_MASK
		| Gdk::POINTER_MOTION_MASK); // TODO: Use Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK
}

void DiagramEditor::activateInplaceEditor(shared_ptr< cppgef::IInplaceEditor > editor, const cppgef::Point& point)
{
	Gtk::Widget* editor_widget = Gtk::manage( editor->createWidget() );

	this->put (*editor_widget,
		editor->getBounds().getLocation().getX(),
		editor->getBounds().getLocation().getY());

	editor_widget->set_size_request(
		editor->getBounds().getSize().getWidth(),
		editor->getBounds().getSize().getHeight());


	editor_widget_focus_out_connection_ = editor_widget->signal_focus_out_event().connect (
			sigc::mem_fun( *this, &DiagramEditor::onEditorWidgetFocusOutEvent ));

	editor->signalStopEdit().connect (
		sigc::mem_fun( *this, &DiagramEditor::onInplaceEditorStopEdit ));
		
	editor_widget->show();
	editor_widget->grab_focus();

	inplace_editor_ = editor;
	
	shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(editor);
	if (text_inplace_editor)
	{
		text_inplace_editor->setCursorPosition (point);
		
		signal_text_edit_start_edit_.emit(text_inplace_editor->getTextFormat());
		
		text_inplace_editor->signalUpdateStyle().connect (
			sigc::mem_fun(this, &DiagramEditor::onTextEditorUpdateStyle ));
	}
}

void DiagramEditor::clearSelection()
{
	diagram_edit_part_->clearSelection();
}

void DiagramEditor::selectAll()
{
	diagram_edit_part_->selectAll();
}

shared_ptr< cppgef::DiagramEditPart > DiagramEditor::getDiagramEditPart()
{
	return diagram_edit_part_;
}

void DiagramEditor::setDiagramEditPart (shared_ptr< cppgef::DiagramEditPart > value)
{
	diagram_edit_part_ = value;
	
	diagram_edit_part_->signalSelectionAdd().connect (
		sigc::mem_fun( this, &DiagramEditor::onSelectionChange ));
		
	diagram_edit_part_->signalSelectionRemove().connect (
		sigc::mem_fun( this, &DiagramEditor::onSelectionChange ));
}

void DiagramEditor::setDiagramEditPart (shared_ptr< cppgef::DiagramEditPart > value, const std::string& file_name)
{
	file_name_ = file_name;
	
	setDiagramEditPart (value);
}

bool DiagramEditor::hasFile() const
{
	return !file_name_.empty();
}

std::string DiagramEditor::getFileName() const
{
	return file_name_;
}

double DiagramEditor::getElapsedTimeFromLastSave() const
{
	return difftime (time(0), last_save_time_);
}

bool DiagramEditor::save()
{
	last_save_time_ = time(0);

	return diagram_edit_part_->save (file_name_);
}

bool DiagramEditor::save(const std::string& file_name)
{
	file_name_ = file_name;
	
	return save();
}

bool DiagramEditor::copy(const std::string& file_name)
{
	return diagram_edit_part_->save (file_name);
}

bool DiagramEditor::open(const std::string& file_name)
{
	last_save_time_ = time(0);

	file_name_ = file_name;
	
	return diagram_edit_part_->open (file_name);
}

bool DiagramEditor::reOpen()
{
	last_save_time_ = time(0);

	diagram_edit_part_->clearSelection();

	shared_ptr< cppgef::Diagram > old_diagram = dynamic_pointer_cast< cppgef::Diagram >(
			diagram_edit_part_->getModel() );

	cppgef::CommandManager::getInstance()->closeStack (old_diagram);

	bool res = diagram_edit_part_->open (file_name_);

	shared_ptr< cppgef::Diagram > new_diagram = dynamic_pointer_cast< cppgef::Diagram >(
			diagram_edit_part_->getModel() );

	cppgef::CommandManager::getInstance()->openStack (new_diagram);

	return res;
}

bool DiagramEditor::isInplaceEditingActive()
{
	return (bool)inplace_editor_;
}

bool DiagramEditor::isDirty() const
{
	shared_ptr< cppgef::Diagram > diagram = dynamic_pointer_cast< cppgef::Diagram >(
			diagram_edit_part_->getModel() );

	return !(cppgef::CommandManager::getInstance()->isUndoStackEmpty (diagram));
}

bool DiagramEditor::isEmpty() const
{
	shared_ptr< cppgef::Diagram > diagram = dynamic_pointer_cast< cppgef::Diagram >(
			diagram_edit_part_->getModel() );

	return (diagram->getChildrenSize() == 0);
}

void DiagramEditor::on_size_allocate(Gtk::Allocation& allocation)
{
	int w;
	int h;
	drawing_area_->get_size_request(w, h);
	drawing_area_->size_allocate (
			Gdk::Rectangle(0, 0, w, h));

	if (inplace_editor_)
	{
		 GValue x_value = { 0, };
		 g_value_init(&x_value, G_TYPE_INT);
		 gtk_container_child_get_property(GTK_CONTAINER(this->gobj()),
				 inplace_editor_->getWidget()->gobj(),
				 "x",
				 &x_value);
		 int x = g_value_get_int(&x_value);

		 GValue y_value = { 0, };
		 g_value_init(&y_value, G_TYPE_INT);
		 gtk_container_child_get_property(GTK_CONTAINER(this->gobj()),
				 inplace_editor_->getWidget()->gobj(),
				 "y",
				 &y_value);
		 int y = g_value_get_int(&y_value);

		inplace_editor_->getWidget()->get_size_request(w, h);

		inplace_editor_->getWidget()->size_allocate (
				Gdk::Rectangle(x, y, w, h));
	}
}

bool DiagramEditor::onDrawingAreaButtonPressEvent(GdkEventButton* event)
{
	if (!drawing_area_->has_focus() && drawing_area_->is_sensitive())
		drawing_area_->grab_focus();
		
	if (Toolset::getInstance()->hasActiveTool() && Toolset::getInstance()->getActiveTool()->buttonPress (event, this))
		drawing_area_->queue_draw();
	
	return true;
}

bool DiagramEditor::onDrawingAreaButtonReleaseEvent(GdkEventButton* event)
{
	if (Toolset::getInstance()->hasActiveTool() && Toolset::getInstance()->getActiveTool()->buttonRelease (event, this))
		drawing_area_->queue_draw();
	
	return true;
}

bool DiagramEditor::onDrawingAreaMotionNotifyEvent(GdkEventMotion* event)
{
	if (Toolset::getInstance()->hasActiveTool() && Toolset::getInstance()->getActiveTool()->motionNotify (event, this))
		drawing_area_->queue_draw();
	
	return true;
}

#ifdef GTKMM_3

bool DiagramEditor::onDrawingAreaDraw(const Cairo::RefPtr< Cairo::Context >& context)
{

#else

bool DiagramEditor::onDrawingAreaExposeEvent(GdkEventExpose* event)
{
	Cairo::RefPtr<Cairo::Context> context = drawing_area_->get_window()->create_cairo_context();

#endif	

	Gtk::Allocation allocation = drawing_area_->get_allocation();
    cppgef::Size view_size( allocation.get_width(), allocation.get_height() );
	
	shared_ptr< cppgef::RectangleFigure > diagram_figure = dynamic_pointer_cast< cppgef::RectangleFigure >(diagram_edit_part_->getFigure());
	diagram_figure->getBounds().setSize (view_size);
	
	diagram_figure->paint (context);

	if (Toolset::getInstance()->hasActiveTool())
	{
#ifdef GTKMM_3
		Toolset::getInstance()->getActiveTool()->draw (context);
#else		
		Toolset::getInstance()->getActiveTool()->expose (event, context);
#endif		
	}

	return true;
}

bool DiagramEditor::onEditorWidgetFocusOutEvent(GdkEventFocus* event)
{
	if (external_editing_)
		return true;
		
	Gtk::Allocation alloc = inplace_editor_->getWidget()->get_allocation();

	editor_widget_focus_out_connection_.disconnect();

	signal_text_edit_end_edit_.emit();
	
	inplace_editor_->getWidget()->hide();
	
	inplace_editor_->apply();
	
	remove (*inplace_editor_->getWidget());
	
	inplace_editor_.reset();
	
	return true;
}

void DiagramEditor::cancelInplaceEditing()
{
	if (inplace_editor_)
	{
		// Disconnect focus_out signal, the cancel operation
		// must not apply changes
		editor_widget_focus_out_connection_.disconnect();
		
		signal_text_edit_end_edit_.emit();
		
		inplace_editor_->getWidget()->hide();

		remove (*inplace_editor_->getWidget());
		
		inplace_editor_.reset();		
	}
}

void DiagramEditor::stopInplaceEditing()
{
	if (inplace_editor_)
	{
		// Hide the widget for raise focus_out signal
		inplace_editor_->getWidget()->hide();
	}	
}

void DiagramEditor::setTextEditStyleBold(bool value)
{
	if (inplace_editor_)
	{
		shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(inplace_editor_);
		if (text_inplace_editor)
		{
			text_inplace_editor->setBold (value);
		}
	}
}

void DiagramEditor::setTextEditStyleItalic(bool value)
{
	if (inplace_editor_)
	{
		shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(inplace_editor_);
		if (text_inplace_editor)
		{
			text_inplace_editor->setItalic (value);
		}
	}	
}

void DiagramEditor::setTextEditStyleUnderline(bool value)
{
	if (inplace_editor_)
	{
		shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(inplace_editor_);
		if (text_inplace_editor)
		{
			text_inplace_editor->setUnderline (value);
		}
	}	
}

void DiagramEditor::setTextEditFontFamily(const Glib::ustring& font_family)
{
	if (inplace_editor_)
	{
		shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(inplace_editor_);
		if (text_inplace_editor)
		{
			text_inplace_editor->setFontFamily (font_family);
		}
	}	
}

void DiagramEditor::setTextEditFontSize(const int font_size)
{
	if (inplace_editor_)
	{
		shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(inplace_editor_);
		if (text_inplace_editor)
		{
			text_inplace_editor->setFontSize (font_size);
		}
	}	
}

void DiagramEditor::setTextEditTextColor(const cppgef::Color& color)
{
	if (inplace_editor_)
	{
		shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(inplace_editor_);
		if (text_inplace_editor)
		{
			text_inplace_editor->setTextColor (color);
		}
	}
}

void DiagramEditor::setTextEditTextBackColor(const cppgef::Color& color)
{
	if (inplace_editor_)
	{
		shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(inplace_editor_);
		if (text_inplace_editor)
		{
			text_inplace_editor->setTextBackColor (color);
		}
	}
}

void DiagramEditor::setTextEditTextBackColorNone()
{
	if (inplace_editor_)
	{
		shared_ptr< cppgef::TextInplaceEditor > text_inplace_editor = dynamic_pointer_cast< cppgef::TextInplaceEditor >(inplace_editor_);
		if (text_inplace_editor)
		{
			text_inplace_editor->setTextBackColorNone();
		}
	}
}

void DiagramEditor::setLineColor(const cppgef::Color& color)
{
	shared_ptr< cppgef::ICommand > cmd = diagram_edit_part_->createSetLineColorCommand (color);
	
	if (cmd)
	{
		cppgef::CommandManager::getInstance()->execute (cmd);
		
		this->redraw();
	}
}

void DiagramEditor::setLineWidth (double width)
{
	shared_ptr< cppgef::ICommand > cmd = diagram_edit_part_->createSetLineWidthCommand (width);

	if (cmd)
	{
		cppgef::CommandManager::getInstance()->execute (cmd);

		this->redraw();
	}
}

void DiagramEditor::setFillColor(const cppgef::Color& color)
{
	shared_ptr< cppgef::ICommand > cmd = diagram_edit_part_->createSetFillColorCommand (color);
	
	if (cmd)
	{
		cppgef::CommandManager::getInstance()->execute (cmd);
		
		this->redraw();
	}	
}

void DiagramEditor::setDashStyle (const std::vector< double >& dash_style, double dash_offset)
{
	shared_ptr< cppgef::ICommand > cmd = diagram_edit_part_->createSetDashStyleCommand (dash_style, dash_offset);
	
	if (cmd)
	{
		cppgef::CommandManager::getInstance()->execute (cmd);
		
		this->redraw();
	}		
}

void DiagramEditor::beginExternalEdit()
{
	external_editing_ = true;
}

void DiagramEditor::endExternalEdit()
{
	external_editing_ = false;
}

void DiagramEditor::redraw()
{
	drawing_area_->queue_draw();
}

void DiagramEditor::undo()
{
	if (inplace_editor_)
	{
		inplace_editor_->executeUndo();
	}
	else
	{
		shared_ptr< cppgef::Diagram > active_diagram = dynamic_pointer_cast< cppgef::Diagram >(diagram_edit_part_->getModel());
		
		if (!cppgef::CommandManager::getInstance()->isUndoStackEmpty(active_diagram))
		{
			// Stop inplace-editing
			stopInplaceEditing();
			
			// Clear selection
			clearSelection();
			
			// Undo last command
			cppgef::CommandManager::getInstance()->undo (active_diagram);
		}
	}
	
	redraw();
}

void DiagramEditor::redo()
{
	if (inplace_editor_)
	{
		inplace_editor_->executeRedo();
	}
	else
	{
		shared_ptr< cppgef::Diagram > active_diagram = dynamic_pointer_cast< cppgef::Diagram >(diagram_edit_part_->getModel());
		
		if (!cppgef::CommandManager::getInstance()->isRedoStackEmpty(active_diagram))
		{
			// Cancel inplace-editing
			cancelInplaceEditing();
			
			// Clear selection
			clearSelection();

			// Redo last undo command
			cppgef::CommandManager::getInstance()->redo (active_diagram);
		}	
	}
	
	redraw();
}

void DiagramEditor::clipboardCut()
{
	if (inplace_editor_)
	{
		inplace_editor_->executeCut();
	}
	else
	{
		shared_ptr< cppgef::Diagram > active_diagram = dynamic_pointer_cast< cppgef::Diagram >(diagram_edit_part_->getModel());

		if (diagram_edit_part_->getSelectedChildrenSize() > 0)
		{
			clipboardCopy();
			deleteSelection();
			
			redraw();
		}
	}
}

void DiagramEditor::clipboardCopy()
{
	if (inplace_editor_)
	{
		inplace_editor_->executeCopy();
	}
	else
	{
		shared_ptr< cppgef::Diagram > active_diagram = dynamic_pointer_cast< cppgef::Diagram >(diagram_edit_part_->getModel());

		shared_ptr< cppgef::ClipboardCopyCommand > cmd(
			new cppgef::ClipboardCopyCommand(
				active_diagram,
				diagram_edit_part_));

		cppgef::CommandManager::getInstance()->execute (cmd);

		redraw();
	}
}

void DiagramEditor::clipboardPaste()
{
	if (inplace_editor_)
	{
		inplace_editor_->executePaste();
	}
	else
	{	
		shared_ptr< cppgef::Diagram > active_diagram = dynamic_pointer_cast< cppgef::Diagram >(diagram_edit_part_->getModel());

		shared_ptr< cppgef::ClipboardPasteCommand > cmd(
			new cppgef::ClipboardPasteCommand(
				active_diagram));

		clearSelection();

		bool prev_select_new_child = diagram_edit_part_->isSelectNewChild();
		diagram_edit_part_->setSelectNewChild (true);
		
		cppgef::CommandManager::getInstance()->execute (cmd);

		diagram_edit_part_->setSelectNewChild (prev_select_new_child);
		
		// Move pasted objects, don't use CommandManager
		// because this MoveCommand must not be undone
		shared_ptr< cppgef::ICommand > move_cmd = diagram_edit_part_->createMoveCommand (5, 5);
		move_cmd->execute();
		
		redraw();
	}
}

void DiagramEditor::deleteSelection()
{
	shared_ptr< cppgef::ICommand > delete_cmd = diagram_edit_part_->createDeleteCommand();
	
	if (delete_cmd)
	{
		cppgef::CommandManager::getInstance()->execute (delete_cmd);
		
		redraw();
	}
}

DiagramEditor::signal_text_edit_start_edit_t DiagramEditor::signalTextEditStartEdit()
{
	return signal_text_edit_start_edit_;
}

DiagramEditor::signal_text_edit_update_style_t DiagramEditor::signalTextEditUpdateStyle()
{
	return signal_text_edit_update_style_;
}

DiagramEditor::signal_text_edit_end_edit_t DiagramEditor::signalTextEditEndEdit()
{
	return signal_text_edit_end_edit_;
}

DiagramEditor::signal_selection_change_t DiagramEditor::signalSelectionChange()
{
	return signal_selection_change_;
}

void DiagramEditor::onTextEditorUpdateStyle(const cppgef::TextFormat& text_format)
{
	signal_text_edit_update_style_.emit (text_format);
}

void DiagramEditor::onInplaceEditorStopEdit()
{
	stopInplaceEditing();
}

void DiagramEditor::onSelectionChange(shared_ptr< cppgef::IEditPart > shape_edit_part)
{
	signal_selection_change_.emit (shape_edit_part);
}




