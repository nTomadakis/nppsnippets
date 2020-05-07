/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  NppSnippets - Code Snippets plugin for Notepad++                       //
//  Copyright (C) 2010-2016 Frank Fesevur                                  //
//                                                                         //
//  This program is free software; you can redistribute it and/or modify   //
//  it under the terms of the GNU General Public License as published by   //
//  the Free Software Foundation; either version 2 of the License, or      //
//  (at your option) any later version.                                    //
//                                                                         //
//  This program is distributed in the hope that it will be useful,        //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           //
//  GNU General Public License for more details.                           //
//                                                                         //
//  You should have received a copy of the GNU General Public License      //
//  along with this program; if not, write to the Free Software            //
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NppOptions.h"

/////////////////////////////////////////////////////////////////////////////
//

class Options : public NppOptions
{
public:
	Options() noexcept;
	~Options();

	bool showConsoleDlg;
	bool toolbarIcon;
	bool indentSnippet;

	std::wstring GetPrevVersion() { return _prevVersion; };
	std::wstring GetDBFile() { return _DBFile; };

	void Write();
	void Read();

private:
	std::wstring _prevVersion;
	std::wstring _DBFile;
};
