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

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "messages/messages.pb.h"

namespace libarchiver {

typedef std::vector<uint32_t> ids_vector;

typedef enum {
	kRED,
	kYELLOW,
	kGREEN
} colors_t;

class archiver {

public:
	archiver();
	explicit archiver(std::string const & ifile);
	virtual ~archiver();

	/* Todos modifiers */
	messages::Todo * new_todo();
	messages::Todo * get_current_todo();
	const messages::Priority const * get_priority() const;

	messages::Carnet const & enqueue_todo();

	/* Following method is useless if archiver APIs are used */
	const messages::Todo const * set_id(uint32_t const & iid);

	const messages::Todo const * set_title(std::string const & ititle);
	const messages::Todo const * set_body(std::string const & ibody);
	const messages::Todo const * set_priority(
			uint32_t const & ilevel,
			libarchiver::colors_t const & icolor);
	const messages::Todo const * set_links(libarchiver::ids_vector const & ivectorid);

	/* Carnet modifiers */
	messages::Carnet const & get_carnet() const;
	messages::Carnet const & add_todo(messages::Todo const & itodo);
	messages::Carnet const & remove_todo(uint32_t const & iid);
	messages::Carnet const & remove_todo(messages::Todo & itodo);

	/* Save and load */
	void set_working_file(std::string const & ifile);
	void save_work(std::string const & ifile = "");
	void load_from_file(std::string const & ifile = "");
	std::string const & get_current_used_file() const;

	friend std::ostream & operator<<(std::ostream& ostream, archiver const & iarchiver);

private:
	bool is_todo_instantiated() const;
	void set_highest_id();
	uint32_t _highestid;

	std::string _file;
	messages::Todo * _todo;
	messages::Carnet _carnet;
};

}
