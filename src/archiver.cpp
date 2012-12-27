/*           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                 Version 2, December 2004
 *
 *        Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "archiver.h"
#include "messages/messages.pb.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define bforeach BOOST_FOREACH

using namespace libarchiver;

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

using messages::Todo;
using messages::Priority;
using messages::Carnet;

using boost::lexical_cast;

#define IN_BINARY std::ios_base::in | std::ios_base::binary
#define OUT_BINARY std::ios_base::out | std::ios_base::binary

#define RESET		0
#define BRIGHT 		1
#define DIM		    2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK       0
#define RED         1
#define GREEN       2
#define YELLOW      3
#define BLUE        4
#define MAGENTA     5
#define CYAN        6
#define	WHITE       7

string textcolor(int attr, int fg, int bg)
{
	string command;
	command += 0x1b;

	if (attr == RESET) {
		command += "[";
		command += "0m";
	}
	else {
		command += "[";
		command += boost::lexical_cast<string>(attr);
		command += ";";
		command += boost::lexical_cast<string>(fg + 30);
		command += ";";
		command += boost::lexical_cast<string>(bg + 40);
		command += "m";
	}

	return command;
}

archiver::archiver() :
	_highestid(0),
	_todo(0)
{

}

archiver::archiver(string const & ifile) :
	_highestid(0),
	_file(ifile),
	_todo(0)
{
	ifstream ifstream(_file.c_str(), IN_BINARY);
	_carnet.ParseFromIstream(&ifstream);

	set_highest_id();
}

archiver::~archiver()
{
	if (is_todo_instantiated())
		delete _todo;

	_todo = 0;
}

bool archiver::is_todo_instantiated() const
{
	return (_todo != 0);
}

Todo * archiver::new_todo()
{
	if (is_todo_instantiated()) delete _todo;
	_todo = new Todo();
	_todo->set_id(_highestid++);

	return _todo;
}

Todo * archiver::get_current_todo()
{
	return _todo;
}

const Priority const * archiver::get_priority() const
{
	if (!is_todo_instantiated()) return 0;

	return &(_todo->priority());
}


Carnet const & archiver::enqueue_todo()
{
	if (is_todo_instantiated()) {
		Todo * aNewTodo = _carnet.add_todos();
		aNewTodo->CopyFrom(*_todo);

		delete _todo;
		_todo = 0;
	}

	return _carnet;
}

const Todo const * archiver::set_id(uint32_t const & iid)
{
	if (!is_todo_instantiated()) return 0;
	_todo->set_id(iid);

	return _todo;
}

const Todo const * archiver::set_title(std::string const & ititle)
{
	if (!is_todo_instantiated()) return 0;
	_todo->set_title(ititle);

	return _todo;
}

const Todo const * archiver::set_body(std::string const & ibody)
{
	if (!is_todo_instantiated()) return 0;
	_todo->set_body(ibody);

	return _todo;
}

const Todo const * archiver::set_priority(uint32_t const & ilevel, colors_t const & icolor)
{
	if (!is_todo_instantiated()) return 0;
	_todo->mutable_priority()->set_color((int32_t) icolor);
	_todo->mutable_priority()->set_level(ilevel);

	return _todo;
}

const Todo const * archiver::set_links(ids_vector const & ivectorid)
{
	if (!is_todo_instantiated()) return 0;
	bforeach(uint32_t const & anint, ivectorid)
		_todo->add_linked(anint);

	return _todo;
}

Carnet const & archiver::get_carnet() const
{
	return _carnet;
}

Carnet const & archiver::add_todo(Todo const & itodo)
{
	Todo * anewtodo = _carnet.add_todos();
	anewtodo->CopyFrom(itodo);

	return _carnet;
}

Carnet const & archiver::remove_todo(uint32_t const & iid)
{
	int32_t index(0);
	bforeach(Todo & todo, *(_carnet.mutable_todos())) {
		if (todo.id() == iid) break;
		index++;
	}

	_carnet.mutable_todos()->SwapElements(index, _carnet.todos_size()-1);
	_carnet.mutable_todos()->RemoveLast();

	return _carnet;
}

Carnet const & archiver::reorder_carnet(orders_t const & iorder)
{
	switch (iorder) {
	case kPRIORITY:
		reorder_priority(0);
		break;
	case kID:
		reorder_id(0);
		break;
	}

	return _carnet;
}

void archiver::reorder_id(uint16_t iindex)
{
	/* no todos (0) or last todo (1) */
	if (iindex == _carnet.todos_size()) return;
	if (iindex == _carnet.todos_size()-1) return;

	int tempindex(0);
	bforeach(Todo const & todo, _carnet.todos()) {
		if (todo.id() > _carnet.todos(++tempindex).id()) {
			_carnet.mutable_todos()->SwapElements(tempindex-1, tempindex);
			reorder_id(tempindex);
		}

		if (tempindex == _carnet.todos_size()-1) break;
	}
}

void archiver::reorder_priority(uint16_t iindex)
{
	/* no todos (0) or last todo (1) */
	if (iindex == _carnet.todos_size()) return;
	if (iindex == _carnet.todos_size()-1) return;

	int tempindex(0);
	bforeach(Todo const & todo, _carnet.todos()) {
		if (todo.priority().level() > _carnet.todos(++tempindex).priority().level()) {
			_carnet.mutable_todos()->SwapElements(tempindex-1, tempindex);
			reorder_priority(tempindex);
		}

		if (tempindex == _carnet.todos_size()-1) break;
	}
}

Carnet const & archiver::pretty_print_carnet(string & ostring) const
{
	ostring.clear();

	bforeach(Todo const & todo, _carnet.todos()) {
		ostring += "[";
		ostring += boost::lexical_cast<string>(todo.id());
		ostring += "] ";

		switch (todo.priority().color()) {
		case kRED:
			ostring += textcolor(UNDERLINE, RED, BLACK);
			break;
		case kGREEN:
			ostring += textcolor(UNDERLINE, GREEN, BLACK);
			break;
		case kYELLOW:
			ostring += textcolor(UNDERLINE, YELLOW, BLACK);
			break;
		}

		ostring += todo.title();
		ostring += string("\n");
		ostring += textcolor(RESET, BLACK, WHITE);
		ostring += todo.body();
		ostring += string("\n\n");
		ostring += "(Pri: ";
		ostring += boost::lexical_cast<string>(todo.priority().level());
		ostring += string(")\n\n\n");
	}
	return _carnet;
}

Carnet const & archiver::remove_todo(messages::Todo & itodo)
{
	return (remove_todo(itodo.id()));
}

void archiver::set_working_file(std::string const & ifile)
{
	_file = ifile;
}

void archiver::save_work(std::string const & ifile)
{
	if (!ifile.empty()) _file = ifile;
	std::ofstream anofstream(_file.c_str(), OUT_BINARY);
	_carnet.SerializeToOstream(&anofstream);

	set_highest_id();
}

void archiver::load_from_file(std::string const & ifile)
{
	if (!ifile.empty()) _file = ifile;
	std::ifstream anifstream(_file.c_str(), IN_BINARY);
	_carnet.ParseFromIstream(&anifstream);

	set_highest_id();
}

std::string const & archiver::get_current_used_file() const
{
	return _file;
}

std::ostream & libarchiver::operator<<(std::ostream& ostream, archiver const & iarchiver)
{
	if (iarchiver.is_todo_instantiated()) {
		ostream << " currently opened todo: \n";
		ostream << iarchiver._todo->DebugString();
		ostream << std::endl;
	}

	ostream << " carnet containing: \n";
	ostream << iarchiver._carnet.DebugString();

	return ostream;
}

void archiver::set_highest_id()
{
	_highestid = 0;

	bforeach(Todo const & todo, _carnet.todos())
		if (todo.id() >= _highestid) _highestid = todo.id();

	_highestid += 1;
}
