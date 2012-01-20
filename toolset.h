#ifndef TOOLSET_H
#define TOOLSET_H

#include <boost/shared_ptr.hpp>

#include <itool.h>

class Toolset
{
private:
	static Toolset* instance_;

public:
	static Toolset* getInstance();
	static void release();

	bool hasActiveTool() const;
	
	shared_ptr< cppgef::ITool > getActiveTool();
	void setActiveTool(shared_ptr< cppgef::ITool > value);
	
	bool isSelectionToolActive();
	
private:
	Toolset();
	virtual ~Toolset();

private:
	shared_ptr< cppgef::ITool > active_tool_;
};

#endif // TOOLSET_H
