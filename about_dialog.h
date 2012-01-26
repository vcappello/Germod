/*
 * about_dialog.h
 *
 *  Created on: Jan 26, 2012
 *      Author: vincenzo
 */

#ifndef ABOUT_DIALOG_H_
#define ABOUT_DIALOG_H_

#include <gtkmm/aboutdialog.h>

#include <vector>

class AboutDialog : public Gtk::AboutDialog
{
public:
	AboutDialog();
	virtual ~AboutDialog();

protected:
	std::vector< Glib::ustring > authors_list;
};

#endif /* ABOUT_DIALOG_H_ */
