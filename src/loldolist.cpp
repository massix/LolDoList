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

#include <string>
#include <iostream>
#include <boost/program_options.hpp>

#include "archiver.h"

using namespace std;
using std::string;
using namespace boost::program_options;

int main(int argc, char *argv[]) {
	libarchiver::archiver archiver;

	options_description optdesc("Allowed options");
	optdesc.add_options()
		("help,h", "Prints help message and exists")
		("file,f", value<string>(), "Reads carnet from file")
		("add,a", "Interactively add a todo")
		("remove,r", value<uint32_t>(), "Remove todo id from file")
		("print,p", "Print opened carnet with -f");

	variables_map varmap;
	store(parse_command_line(argc, argv, optdesc), varmap);

	if (varmap.count("help")) {
		cout << optdesc << endl;
		return 0;
	}

	if (varmap.count("file")) {
		archiver.load_from_file(varmap["file"].as<string>());
	}

	if (varmap.count("add")) {
		string title, body;
		int64_t priority, color;
		cout << "Insert title: ";
		cin >> title;
		cout << "Insert body: ";
		cin >> body;
		cout << "Insert priority: ";
		cin >> priority;
		cout << "Insert color: ";
		cin >> color;

		archiver.new_todo();
		archiver.set_title(title);
		archiver.set_body(body);
		archiver.set_priority(priority, (libarchiver::colors_t) color);
		archiver.enqueue_todo();
	}

	if (varmap.count("remove")) {
		archiver.remove_todo(varmap["remove"].as<uint32_t>());
	}

	if (varmap.count("print")) {
		string carnet;
		archiver.pretty_print_carnet(carnet);
		std::cout << carnet << std::endl;
	}


	archiver.save_work();

	return 0;
}



