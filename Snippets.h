/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  NppSnippets - Code Snippets plugin for Notepad++                       //
//  Copyright (C) 2010 Frank Fesevur                                       //
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

#include "SnippetsDB.h"

class SnippetBase
{
public:
	int GetLibraryID() { return _LibraryID; }
	WCHAR* WGetName() { return _Name; }

	virtual void Set(SqliteStatement* stmt) = 0;
	void SetLibraryID(int i) { _LibraryID = i; }
	void WSetName(LPCWCH txt);

	virtual void SaveToDB(bool autoOpen = true) = 0;
	virtual void DeleteFromDB() = 0;

protected:
	int _LibraryID;
	WCHAR* _Name;
};

/////////////////////////////////////////////////////////////////////////////
//

class Snippet : public SnippetBase
{
public:
	Snippet();
	Snippet(SqliteStatement* stmt);
	Snippet(const Snippet&);

	virtual ~Snippet();

	Snippet& operator=(const Snippet&);
	Snippet& operator=(SqliteStatement* stmt);
	void Set(SqliteStatement* stmt) override;

	int GetSnippetID() { return _SnippetID; }
	WCHAR* WGetBeforeSelection() const { return _BeforeSelection; }
	WCHAR* WGetAfterSelection() const { return _AfterSelection; }
	char* GetBeforeSelection() { return Unicode2Ansi(_BeforeSelection); }
	char* GetAfterSelection() { return Unicode2Ansi(_AfterSelection); }
	bool GetReplaceSelection() { return _ReplaceSelection; }
	bool GetNewDocument() { return _NewDocument; }
	LangType GetNewDocumentLang() { return _NewDocumentLang; }
	int GetSort() { return _Sort; }

	void SetSnippetID(int i) { _SnippetID = i; }
	void WSetBeforeSelection(LPCWCH txt);
	void WSetAfterSelection(LPCWCH txt);
	void SetBeforeSelection(LPCSTR txt);
	void SetReplaceSelection(bool b) { _ReplaceSelection = b; }
	void SetReplaceSelection(int i) { _ReplaceSelection = (i != 0); }
	void SetNewDocument(bool b) { _NewDocument = b; }
	void SetNewDocument(int i) { _NewDocument = (i != 0); }
	void SetNewDocumentLang(int i) { _NewDocumentLang = (LangType) i; }
	void SetSort(int i) { _Sort = i; }

	void SaveToDB(bool autoOpen = true) override;
	void DeleteFromDB() override;

	void GuessName();

private:
	int _SnippetID;
	WCHAR* _BeforeSelection;
	WCHAR* _AfterSelection;
	bool _ReplaceSelection;
	bool _NewDocument;
	LangType _NewDocumentLang;
	int _Sort;

	void CopyValues(const Snippet&);
	int GetReplaceSelectionInt() { return _ReplaceSelection ? 1 : 0; }
	int GetNewDocumentInt() { return _NewDocument ? 1 : 0; }

	LPSTR Unicode2Ansi(LPCWSTR wszStr);
};
