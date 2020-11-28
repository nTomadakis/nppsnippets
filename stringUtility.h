/////////////////////////////////////////////////////////////////////////////
//  NppSnippetsPlus - a fork of NppSnippets								   //	
//  Copyright (C) 2020  Nicolas Tomadakis								   //	
//  NppSnippets - Code Snippets plugin for Notepad++                       //
//  Copyright (C) 2010-2020 Frank Fesevur                                  //
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


#ifndef NPPSPNIPPETS_STRINGUTILITY_H_
#define NPPSPNIPPETS_STRINGUTILITY_H_

#include <utility>

#define TOWSTR_(s) L##s
#define TOWSTR(s) TOWSTR_(s)
#define ANYSTR(type, s) (std::get<const type*>(std::pair<const char*, const wchar_t*>(s, TOWSTR(s))))

#define SNIPSTR(s) ANYSTR(CharT, s)

#define ANYCHAR(type, c) (std::get<type>(std::pair<char, wchar_t>(c, TOWSTR(c))))
#define SNIPCHAR(c) ANYCHAR(CharT, c)
#define SNIPNULL	SNIPCHAR('\0')

#endif // NPPSPNIPPETS_STRINGUTILITY_H_
