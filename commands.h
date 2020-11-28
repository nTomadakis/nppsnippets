/////////////////////////////////////////////////////////////////////////////
//  NppSnippetsPlus - a fork of NppSnippets	plugin for Notepad++		   //	
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

#ifndef NPPSPNIPPETS_COMMANDS_H_
#define NPPSPNIPPETS_COMMANDS_H_

#include "NPP/PluginInterface.h"
#include "NppSnippets.h"
#include "stringutility.h"

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <utility>
#include <map>

namespace nppSnipp {

/* MsgData --> une structure qui gère les informations à transmettre aux commandes
*/
/*
template<class CharT>
struct MsgData {
public :
	std::basic_string<CharT> cmdStr;
};
for now just the args ... may change later !
*/

template <class CharT>
using MsgData = std::basic_string<CharT>;


template<class CharT>
class CommandBase {
public:
	virtual void onStart()	{}													// lors du lancement du programme
	virtual void onCompile(const MsgData<CharT>& msg) {}				// lors de la compilation lorsque la commande est trouvée 
	virtual void onBeforeExecute() {}			// avant de commencer l'insertion
	virtual std::basic_string<CharT> onExecute(const MsgData<CharT>& msg) = 0;				// lors de l'insertion (le plus utilisé dans doute !) : renvoie la chaîne à imprimer
	virtual void onAfterExecute() {}			// quand toutes les insertions sont finies 
	virtual void onExit()	{}				// lors de la sortie du programme (nettoyages)
	virtual const CharT* name() const { return SNIPSTR(""); }        // yes it's char* and not CharT* ... at least for now
	virtual const CharT* help() const { return name(); }		// yes it's char* and not CharT* ... at least for now
};


template<class CharT>
class CmdVars : public CommandBase<CharT>	{ // user variable
	using stringT = std::basic_string<CharT>;	// let's make the code a little less verbose

private :
	int count = 0;
	std::unordered_map<stringT, stringT> _values;

public :
	void onStart() override { // lors du lancement du programme
		_values.clear();
	}

	void onCompile(const MsgData<CharT>& msg) override { 				// lors de la compilation lorsque la commande est trouvée
		_values.insert(std::make_pair(msg, stringT{}));
	}
	void onBeforeExecute() override {		// avant l'insertion --> demander les valeurs de toutes les variables
		/* [[TODO]] construire le dialogue qui demande les valeurs de chacune des variables et les MaJ
		ou alternativement faire un simple dialogue pour chacune des variables
		*/

	}			
	std::basic_string<CharT> onExecute(const MsgData<CharT>& msg) override {
		return _values[msg];
	}

	//void onAfterExte() {}			// quand toutes les insertions sont finies 
	void onExit() {				// lors de la sortie du programme (nettoyages)
	}

	virtual const CharT* name() const { return SNIPSTR("named varirable"); }
};

template<class CharT>
class CmdSkip : public CommandBase<CharT> { // does nothing .. .just skips the tag. Probably nothing to overload
	virtual const CharT* help() const { return SNIPSTR("does noting as it is just removed on insertion"); }
	virtual const CharT* name() const { return SNIPSTR("skip"); }
	virtual std::basic_string<CharT> onExecute(const MsgData<CharT>& msg) { return SNIPSTR(""); }
};

template<class CharT>
class CmdEnd : public CommandBase<CharT> { // just process the end of the string 
};

template<class CharT>
class SimpleInsertCmd : public CommandBase<CharT> { // a sub-kind of command that

protected:
	bool _cached = false;
	std::basic_string<CharT> _value;

	virtual std::basic_string<CharT> _getValue() = 0;

	static std::basic_string<CharT> strFrongMsg(int msgNum, int maxSize) {	// one helper function ... used or not
		CharT* buffer = new CharT[maxSize + 1];
		SendMessage(g_nppData._nppHandle, msgNum, maxSize,(LPARAM) buffer);
		std::basic_string<CharT> val(buffer);
		delete[] buffer;
		return val;
	}

public:
	void onBeforeExecute() override { _cached = false; }			// avant de commencer l'insertion
	std::basic_string<CharT> onExecute(const MsgData<CharT>& msg) final {
		if (!_cached) {
			_value = _getValue();
			_cached = true;
		}
		return _value;
	}				// lors de l'insertion (le plus utilisé dans doute !) : renvoie la chaîne à imprimer
	void onAfterExecute() override { _cached = false; }			// quand toutes les insertions sont finies 
};

template<class CharT>
class CmdFileName : public SimpleInsertCmd<CharT> { // adds the filename
protected:
	std::basic_string<CharT> _getValue()				{		return SimpleInsertCmd<CharT>::strFrongMsg(NPPM_GETFILENAME, _MAX_PATH);		}
	const CharT* help() const override { return SNIPSTR("replaced by the active file's name"); }
	const CharT* name() const override { return SNIPSTR("filename"); }
};

template<class CharT>
class CmdNamepart : public SimpleInsertCmd<CharT> { // adds the filename
protected:
	std::basic_string<CharT> _getValue() { return SimpleInsertCmd<CharT>::strFrongMsg(NPPM_GETNAMEPART, _MAX_PATH); }
	const CharT* help() const override { return SNIPSTR("replaced by the active file's name whitout the extension"); }
	const CharT* name() const override { return SNIPSTR("namepart"); }
};

template<class CharT>
class CmdFileExtension : public SimpleInsertCmd<CharT> { // adds the filename
protected:
	std::basic_string<CharT> _getValue() { return SimpleInsertCmd<CharT>::strFrongMsg(NPPM_GETEXTPART, _MAX_PATH); }
	const CharT* help() const override { return SNIPSTR("replaced by the extension of the active file"); }
	const CharT* name() const override { return SNIPSTR("fileextension"); }
};

template<class CharT>
class CmdFullPath : public SimpleInsertCmd<CharT> { // adds the filename
protected:
	std::basic_string<CharT> _getValue() { return SimpleInsertCmd<CharT>::strFrongMsg(NPPM_GETEXTPART, _MAX_PATH); }
	const CharT* help() const override { return SNIPSTR("replaced by the full path of the active file ... untested ... use caution"); }
	const CharT* name() const override { return SNIPSTR("fullPath(beta)"); }
};

template<class CharT>
class CmdError : public CommandBase<CharT> {
	std::vector<std::basic_string<CharT>>  _errors;
	int _curPos = 0;

	const CharT* help() const override { return SNIPSTR("plugin's error reporting command"); }
	const CharT* name() const override { return SNIPSTR("error"); }

	void onCompile(const MsgData<CharT>& msg)  override {
		_errors.push_back(msg); // [[TODO]] improve this ! --> more information !!!!
	}

	virtual void onBeforeExecute()  override { _curPos = 0; }	// reset position

	std::basic_string<CharT> onExecute(const MsgData<CharT>& msg)  override {
		return _errors[_curPos++];
	}

	void onExit() override {
		_curPos = 0;
		_errors.clear();
	}
};



} //namespace nppSnipp

#endif //NPPSPNIPPETS_COMMANDS_H_
