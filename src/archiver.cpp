/*
 * Archiver.cpp
 *
 *  Created on: 24/dic/2012
 *      Author: massi
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
