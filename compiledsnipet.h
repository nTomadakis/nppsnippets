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

#ifndef NPPSPNIPPETS_COMPILEDSNIPET_H_
#define NPPSPNIPPETS_COMPILEDSNIPET_H_

#include "commands.h"
#include "NppSnippets.h"
#include "Snippets.h"
#include "stringUtility.h"

#include <unordered_map>
#include <locale>
//#include <sstream>


namespace nppSnipp {

template<class CharT>
struct OneCmd {
	CommandBase<CharT>* _cmd;
	std::basic_string<CharT> _args;

	void						onBeforeExecute()	{ return _cmd->onBeforeExecute();}
	std::basic_string<CharT>	onExecute()			{ return _cmd->onExecute(_args); }
	void						onAfterExecute()	{ return _cmd->onAfterExecute(); }
	void						onCompile()			{ return _cmd->onCompile(_args); }
};

enum {
	eComplete = -1,
	eBefore = 0,
	eAfter  = 1
};

template<class CharT>
class CompiledSnippet {
	using stringT = std::basic_string<CharT>;		// let's make the code less verbose with a shortcut

public :
	static const stringT 										STARTCMD;
	static const stringT 										ENDCMD;
	static inline std::unordered_map<stringT, CommandBase<CharT>*> 	_allCmds;
	static inline bool _started = false;

	static bool addCmd(stringT tag, CommandBase<CharT>* cmd) {
		for(CharT &c : tag)
			c = std::toupper(c, std::locale());
		if (_allCmds.insert({ tag, cmd }).second) {
			if (_started) cmd->onStart();
			return true;
		}
		else {
			MsgBox("Insertion failed...\n is this a duplicate tag? \n\n (remember tags are case insensitive)");
			return false;
		}
	}

	static void start() {
		if (_started) return;
		_allCmds[SNIPSTR("FILENAME")]	= new CmdFileName<CharT>();
		_allCmds[SNIPSTR("NAMEPART")]	= new CmdNamepart<CharT>();
		_allCmds[SNIPSTR("EXTENSION")]	= new CmdFileExtension<CharT>();
		_allCmds[SNIPSTR("FULLPATH")]	= new CmdFullPath<CharT>();

		_allCmds[SNIPSTR("@@@ERROR@@@")]= new CmdError<CharT>();	// internal command -- error reporting (mostly todo)
		_allCmds[SNIPSTR("@@@SKIP@@@")] = new CmdSkip<CharT>();		// internal placeholder command doing nothing

		for (auto cmd : _allCmds)
			cmd.second->onStart();

		_started = true;
	}


	static void printHelp(bool inEditor = true) {	// prints all commands ... for debugging . later will generate a documentation of the commands.
		if (!inEditor)
			for (auto v : _allCmds)
				std::cout << v.first << " --> " << v.second->help() << std::endl;
		else // dans la commande scintilla
			; // cf TODO below...

		/* [[TODO]]
		SendMessage(g_nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
		SetFocusOnEditor();

		for(auto v : _allCmds) {
			SendText(v.first.c_str());
			SendText(v.second->help());
			}

		*/
	}

	static void finish() {
		for (auto cmd : _allCmds) {
			if (cmd.second != nullptr) { // there may be aliases for commands ... --> sharedPointer serait alors sans doute mieux !
				cmd.second->onExit();
				delete cmd.second;
			}
		}
		_allCmds.clear();
	}

	static bool hasVars(const stringT &wstr) {
		auto pos = wstr.find(STARTCMD);
		if (pos == stringT::npos)
			return false;
		else // got a start ... but we still need to check there's a close tag !
			return wstr.find(ENDCMD, pos) != stringT::npos;
	}

private : 
	static const int MAXSIZE = 10000;
	static const int CMD_SIZE = 100;

	//using cmdPos	=	std::pair<int, int>; // delta et index de commande
	using InsertPoints = std::vector<std::pair<int, OneCmd<CharT>>>;
	using InsertIterator = typename InsertPoints::iterator;
	using size_type = typename std::basic_string<CharT>::size_type;
	const size_type _NoMore = -1;

	bool _compiled	= false;
	bool _populated = false;

	stringT _BeforeSelection; // le buffer 'compilé'
	stringT _AfterSelection;  // le buffer 'compilé'
	CharT _currCmdStr[CMD_SIZE];	// used to stored the current command
	CharT _currCmdName[CMD_SIZE];
	bool _malFormedCmd = false;
	//CharT _currArgs[CMD_SIZE];

	CharT* _curPos;

	CmdVars<CharT>* _cmdVars = nullptr;
	InsertPoints _insertPoints;
	InsertIterator _curInsertPoint;

//	std::map<int, int>  _commands;
public :

	// debuging public -> remove later [[TODO]]


	void print_cmds() const {
		for (auto iP : _insertPoints)
			std::cout << "offset : " << iP.first << " name : " << iP.second._cmd->name() << " args : " << iP.second._args << std::endl;
	}



	/*	_copyUpTo : internal method that : 
		copies all characters from source to dest untill the stop chain is found (or source is finished)
		return the number of characters copied as a positive number if found, as negative otherwise
		note :neither source and dest are advanced
	*/
	static int _copyUpTo(const CharT* source, CharT* dest, const CharT* stop, int maxSize = - 1) {
		int count = 0;
		CharT* d = dest;

		int maxCount = (maxSize > 0) ? maxSize : MAXSIZE;

		while (*source && (count < maxCount)) {
			const CharT* end   = stop;	//	cible
			const CharT* tmp = source;		//	buffer temporary progress
			int security = count;
			while( (*tmp == *end) && (security < maxCount) ) {
				if (*end) { // a-t-on trouvé le tag ?
					++tmp;
					++end;
					++security;
				}
				else
					break;
			}
			if (!*end) { // found it !
				return count; 
			}
			// pas encore trouvé ... on ...
			*d = *source;  // copie
			++d;			  // et incrémente les curseurs
			++source;
			++count;		  // et le compte
		}
		return -count;  // not found
	}

	static int _goToNextTag(const CharT*& source, CharT*& dest, int maxSize = -1) {
		int n = _copyUpTo(source, dest, STARTCMD.c_str(), maxSize);
		if (n < 0) { //strickly inferior ... 0 is indeed a found tag
			//dest -= n;
			return n;
		}
		source += n + STARTCMD.size(); // we also skip STARCMD
		dest += n;
		return n;
	}

	/*
		bool _skipSpaces
		advances str to first non space character. return false if only spaces are found
	*/

	static bool _skipSpaces(const CharT*& str) {
		while (std::isspace<CharT>(*str, std::locale())) {
			++str;
			if (!*str) 	return false; 
		}
		return true;
	}

	static bool _skipToAlnum(const CharT*& str) {
		if (!*str) return false;

		while (!std::isalnum<CharT>(*str, std::locale())) {
			++str;
			if (!*str) 	return false; // empty command --> not valid revert to not found !
		}
		return true;
	}

	void _addFoundCmd(int pos, CommandBase<CharT>* cmd, const char* args) {
		_insertPoints.emplace_back(pos, OneCmd<CharT>{  cmd, args });
		cmd->onCompile(args);
	}


	int _getCommand(const CharT*& source, int delta, int maxSize = -1) {
		int maxCount = ((maxSize > 0) && (maxSize < CMD_SIZE)) ? maxSize : CMD_SIZE;

		CharT* cursor = _currCmdStr;

		int n = _copyUpTo(source, cursor, ENDCMD.c_str(), maxCount);
		if (n < 0) {	// not found
			return n;
		}
		*(_currCmdStr +n) = SNIPNULL;
		source += n + ENDCMD.size();

		// translate the command 

		const CharT* from = _currCmdStr;
		CharT* to = _currCmdName;
		*to = SNIPNULL;

		if (!_skipToAlnum(from))	{
			_malFormedCmd = true;
			_insertPoints.emplace_back(delta, OneCmd<CharT>{  _allCmds[SNIPSTR("@@@ERROR@@@")], _currCmdStr });
			return n;
		}
	
		// now we get the command
		while (std::isalnum<CharT>(*from, std::locale())) {
			*to++ = std::toupper<CharT>(*from++, std::locale());
		}
		//*++to = SNIPNULL; //std::char_traits<CharT>::to_char_type(0);
		*to = SNIPNULL;

		_skipSpaces(from); // skip the spaces now from points to the args
		//_currArgs = from;

		auto cmdIt = _allCmds.find(_currCmdName);
		if (cmdIt == _allCmds.end()) { // cas de la variable nommée
			if (_cmdVars == nullptr)
				_cmdVars = new CmdVars<CharT>();
			_insertPoints.emplace_back(delta, OneCmd<CharT>{  _cmdVars, _currCmdStr });
		}
		else {
			_insertPoints.emplace_back(delta, OneCmd<CharT>{ cmdIt->second, from });	//
		}
		return n;
	}

	void _compile(const CharT* source, stringT& dest, bool forceRecompile = false) {
		if (_compiled && !forceRecompile)
			return;
		int len = std::char_traits<CharT>::length(source);
		CharT* writeTo = new CharT[len + 1]; //  his is actually bigger than needed as we will remove stuff !
		CharT* w = writeTo; // moving cursor

		const CharT* src = source;	// mouving cursor
		bool encore = true;
		int delta;
		while (1) {
			if ((delta = _goToNextTag(src, w, len)) < 0) {
				while (*src)
					*w++ = *src++;
				break;
			}
			if ((delta = _getCommand(src, delta, len)) < 0) {
				// no closing tag left --> let's what's left
				// report an unclosed tag
				src -= STARTCMD.size();
				while (*src) { // copy of the content and advances the cursor (
					*w++ = *src++;
				}
				break;
			}
			_insertPoints.back().second.onCompile();
		}
		*w = SNIPNULL;
		dest.assign(writeTo);
		_insertPoints.emplace_back((int)eComplete, OneCmd<CharT>{ _allCmds[SNIPSTR("@@@SKIP@@@")], SNIPSTR("") });
		delete[] writeTo;
	}

	void _initialize() {
		start();
		_insertPoints.clear();
	}

	/*
	void  _initializeCmds() {
		_cmdVars = new CmdVars();
		_cmdVars->onStart();
	}*/

	// InsertPoints::iterator _curInsertPoint;

	stringT _insert(InsertIterator start, const stringT& input) {
		if ((start == _insertPoints.end()) || (start->first < 0)) {
			_curInsertPoint = start;
			if (_curInsertPoint != _insertPoints.end()) _curInsertPoint++;
			return input;
		}

		stringT ret;
		auto it = start;
		size_type from = 0;
		int count = 0;

		while ((it != _insertPoints.end() && (count >=0))) {
			count = it->first;
			if (count < 0) {
				it++;
				break;
			}
			if (count > 0) {
				ret += input.substr(from, count);
				from += count;
			}
			ret += it->second.onExecute();
			it++;
		}

		ret += input.substr(from); // insert the tail
		_curInsertPoint = it;

		return ret;
	}

public :

	void _testCompile(const char* str)	{
		std::cout << "AVANT: \n\n" << str << "\n\n  APRES : \n\n";
		_initialize();
		_compile(str, _BeforeSelection);
		std::cout << _BeforeSelection << std::endl;
	}


//	CompiledSnippet();
	virtual ~CompiledSnippet() {
		if (_cmdVars != nullptr)
			delete _cmdVars;
	}

	void compile(Snippet* pSnip)  {
		_initialize();
		_compile(pSnip->WGetBeforeSelection(), _BeforeSelection);
		_compile(pSnip->WGetAfterSelection(), _AfterSelection);
		_populated = false;
		_compiled = true;
	}

	void beforeInsert() {
		for (auto ipt : _insertPoints) ipt.second.onBeforeExecute();
	}

	stringT insert(int stage) {
		if (stage == eBefore)
			return _insert(_insertPoints.begin(), _BeforeSelection);
		else
			return _insert(_curInsertPoint, _AfterSelection);
	}

	void afterInsert() {
		for (auto ipt : _insertPoints) ipt.second.onAfterExecute();
	}
};

	
} //namespace nppSnipp

#endif //NPPSPNIPPETS_COMPILEDSNIPET_H_
