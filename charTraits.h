/////////////////////////////////////////////////////////////////////////////
//  NppSnippetsPlus - a fork of NppSnippets								   //	
//  Copyright (C) 2020  Nicolas Tomadakis								   //	
//                                                                         //
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

/*	contains the traits classes to instantiate for 
	CompiledSnippet class
*/

#include "compiledsnipet.h"

#include <cwctype>
#include <cctype>

#ifndef NPPSPNIPPETS_CHARTRAITS_H_
#define NPPSPNIPPETS_CHARTRAITS_H_
namespace nppSnipp
{
/*
template<class CharT>
struct CharTrait {
	virtual toLower(CharT* s) = 0;
	virtual toUpper(CharT*s) = 0;
};
*/

class charTrait : public CharTTrait<char> {
	virtual int toLower(CharT* s) = 0;
	virtual toUpper(CharT*s) = 0;
	bool isblank(int ch)	{ return std::isblank(static_cast<unsigned char>(ch); }
};

class wcharTrait : public CharTTrait<wchar_t> {

};


}	// nppSnipp

#endif // NPPSPNIPPETS_CHARTRAITS_H_
