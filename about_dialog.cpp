/*
 * about_dialog.cpp
 *
 *  Created on: Jan 26, 2012
 *      Author: vincenzo
 */

#include "about_dialog.h"

const Glib::ustring PACKAGE = "Germod";
const Glib::ustring VERSION = "0.0.0";

AboutDialog::AboutDialog()
{
	authors_list.push_back("Vincenzo Cappello");

	set_authors( authors_list );
	set_name(PACKAGE);
	set_version(VERSION);
	set_comments("A diagram editor.");
	set_copyright("Copyright (c) 2012 Vincenzo Cappello");
	//set_logo( Gdk::Pixbuf::create_from_file( DATASDIR "movic.png") );
	set_website("https://github.com/vcappello/germod");
	//set_license("Mobile-Video-Converter is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.\n\nMobile-Video-Converter is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License along with Percent; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA");
	//set_wrap_license( true );
}

AboutDialog::~AboutDialog()
{
}

