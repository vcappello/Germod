#include "toolset.h"

#include <selection_tool.h>

Toolset* Toolset::instance_ = 0;

Toolset::Toolset()
{
}

Toolset::~Toolset()
{
}

Toolset* Toolset::getInstance()
{
	if(instance_ == 0){
		instance_ = new Toolset();
	}
	return instance_;
}

void Toolset::release()
{
	if(instance_){
		delete instance_;
	}
	instance_ = 0;
}

bool Toolset::hasActiveTool() const
{
	return static_cast<bool>(active_tool_);
}

shared_ptr< cppgef::ITool > Toolset::getActiveTool()
{
	return active_tool_;
}

void Toolset::setActiveTool(shared_ptr< cppgef::ITool > value)
{
	active_tool_ = value;
}

bool Toolset::isSelectionToolActive()
{
	shared_ptr< cppgef::SelectionTool > selection_tool = dynamic_pointer_cast< cppgef::SelectionTool >(active_tool_);
	
	return (selection_tool.get() != 0);
}

