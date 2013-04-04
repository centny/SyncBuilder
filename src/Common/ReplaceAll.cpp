/*
 * ReplaceAll.cpp
 *
 *  Created on: Nov 17, 2012
 *      Author: Scorpion
 */

#include "ReplaceAll.h"

namespace centny {

string replaceAll(string& str, string old_value, string new_value,
		size_t bpos) {
	while (true) {
		string::size_type pos(0);
		if ((pos = str.find(old_value, bpos)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

} /* namespace centny */
