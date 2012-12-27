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
#include <boost/foreach.hpp>
#include <iostream>
#include <cassert>

#define bforeach BOOST_FOREACH

#define kFILE "test.tdl"
#define assert_true(test) \
	do { \
		assert(test == true); \
		std::cout << #test << " passed!" << std::endl; \
	} while (0)

#define assert_false(test) \
	do { \
		assert(test == false); \
		std::cout << #test << " passed!" << std::endl; \
	} while (0)

#define test_begin(iname)

#define test_end(iname)

const std::string title("Essential question");
const std::string body("The answer is always 42, but which is the question?");


bool test_basic_todo_ptr()
{
	libarchiver::archiver archiver;

	// Building the todo without passing from the archiver API
	libarchiver::messages::Todo * todoptr = archiver.new_todo();
	todoptr->set_id(42);
	todoptr->set_body(body);
	todoptr->set_title(title);
	todoptr->mutable_priority()->set_level(0);
	todoptr->mutable_priority()->set_color(0);
	archiver.enqueue_todo();

	libarchiver::messages::Todo todo(archiver.get_carnet().todos(0));
	assert(todo.id() == 42);
	assert(todo.body() == body);
	assert(todo.title() == title);
	assert(todo.priority().level() == 0);
	assert(todo.priority().color() == 0);

	return true;
}

bool test_basic_todo()
{
	libarchiver::archiver archiver;
	archiver.new_todo();
	archiver.set_id(42);
	archiver.set_title(title);
	archiver.set_body(body);
	archiver.set_priority(0, libarchiver::kYELLOW);
	archiver.enqueue_todo();

	libarchiver::messages::Todo todo(archiver.get_carnet().todos(0));

	assert(todo.id() == 42);
	assert(todo.title() == title);
	assert(todo.body() == body);
	assert(todo.priority().level() == 0);
	assert(todo.priority().color() == libarchiver::kYELLOW);

	return true;
}

bool test_add_todo()
{
	// Adding todo from scratch ignoring archiver api
	libarchiver::archiver archiver;
	libarchiver::messages::Todo todo;
	todo.set_id(42);
	todo.set_title(title);
	todo.set_body(body);
	todo.mutable_priority()->set_level(0);
	todo.mutable_priority()->set_color(0);

	archiver.add_todo(todo);

	libarchiver::messages::Todo retrieved(archiver.get_carnet().todos(0));
	assert(retrieved.id() == todo.id());
	assert(retrieved.title() == todo.title());
	assert(retrieved.body() == todo.body());
	assert(retrieved.priority().color() == todo.priority().color());
	assert(retrieved.priority().level() == todo.priority().level());

	return true;
}


bool test_constructor_file()
{
	test_begin("test_constructor_file()");
	bool retvalue(false);
	libarchiver::archiver * archiver = new libarchiver::archiver(kFILE);

	archiver->new_todo();
	archiver->set_id(0);
	archiver->set_title("Constructor File");
	archiver->set_body("First body");
	archiver->set_priority(0, libarchiver::kRED);

	archiver->enqueue_todo();
	archiver->save_work();
	delete archiver;

	archiver = new libarchiver::archiver(kFILE);
	retvalue = (archiver->get_carnet().todos_size() == 1);

	delete archiver;

	unlink(kFILE);

	test_end("test_constructor_file()");

	return retvalue;
}

bool test_single_todo()
{
	test_begin("test_single_todo()");
	// Create a carnet from scratch and insert a single todo
	libarchiver::archiver archiver;

	archiver.new_todo();
	archiver.set_id(0);
	archiver.set_title("First");
	archiver.set_body("First body");
	archiver.set_priority(0, libarchiver::kRED);

	// Enqueue todo in carnet
	archiver.enqueue_todo();

	// Save to temporary file
	archiver.save_work(kFILE);
	test_end("test_single_todo()");

	// We should have a single todo !
	return (archiver.get_carnet().todos_size() == 1);
}

bool test_load_from_file(uint32_t const & iexpected)
{
	test_begin("test_load_from_file()");

	libarchiver::archiver archiver;
	archiver.load_from_file(kFILE);

	test_end("test_load_from_file()");
	return (archiver.get_carnet().todos_size() == iexpected);
}

bool test_add_todo_file()
{
	test_begin("test_add_todo_file()");

	// Load a carnet from file, at the end it should contain two todos
	libarchiver::archiver archiver;
	archiver.load_from_file(kFILE);

	archiver.new_todo();
	archiver.set_id(1);
	archiver.set_title("Second");
	archiver.set_body("Second body");
	archiver.set_priority(2, libarchiver::kGREEN);

	archiver.enqueue_todo();

	archiver.save_work(archiver.get_current_used_file());

	test_end("test_add_todo_file()");

	return (archiver.get_carnet().todos_size() == 2);
}

bool test_remove_todo()
{
	test_begin("test_remove_todo()");

	libarchiver::archiver archiver(kFILE);

	archiver.new_todo();
	archiver.set_id(0);
	archiver.set_title("To be deleted");
	archiver.set_body("Deleted body");
	archiver.set_priority(0, libarchiver::kYELLOW);
	archiver.enqueue_todo();
	archiver.save_work();

	archiver.remove_todo(0);
	archiver.save_work();

	unlink(kFILE);

	test_end("test_remove_todo()");

	return (archiver.get_carnet().todos_size() == 0);
}

int main()
{
	assert_true(test_basic_todo_ptr());
	assert_true(test_basic_todo());
	assert_true(test_add_todo());
	assert_true(test_constructor_file());
	assert_true(test_remove_todo());
	assert_true(test_single_todo());
	assert_true(test_load_from_file(1));
	assert_true(test_add_todo_file());
	assert_true(test_load_from_file(2));

	// Remove temporary file
	unlink(kFILE);
}
