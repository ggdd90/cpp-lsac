/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file CompilerState.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <boost/spirit/include/support_utree.hpp>
#include "CodeFragment.h"

namespace eth
{

struct Macro
{
	std::vector<std::string> args;
	boost::spirit::utree code;
	std::map<std::string, CodeFragment> env;
};

struct CompilerState
{
	CodeFragment const& getDef(std::string const& _s);
	void populateStandard();

	std::map<std::string, unsigned> vars;
	std::map<std::string, CodeFragment> defs;
	std::map<std::string, CodeFragment> args;
	std::map<std::string, CodeFragment> outers;
	std::map<std::pair<std::string, unsigned>, Macro> macros;
	std::vector<boost::spirit::utree> treesToKill;
};

}
