/*
 * TypePathImp.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ballance
 */

#include "TypePathImp.h"

namespace psi {

TypePathImp::TypePathImp() { }

TypePathImp::TypePathImp(const std::vector<std::string> &path) :
	m_path(path) { }

TypePathImp::TypePathImp(const std::string &path) {
	m_path.push_back(path);
}

void TypePathImp::set(const std::vector<std::string> &path) {
	m_path.clear();

	for (std::vector<std::string>::const_iterator it=path.begin();
			it!=path.end(); it++) {
		std::string tmp = *it;
		m_path.push_back(tmp);
	}
}

const std::string &TypePathImp::leaf() const {
	return (m_path.size()>0)?m_path.at(m_path.size()-1):m_null;
}

TypePathImp::~TypePathImp() {
	// TODO Auto-generated destructor stub
}

std::string TypePathImp::toString() const {
	std::string ret;

	if (isNull()) {
		ret.append("(Empty)");
	} else {
		for (std::vector<std::string>::const_iterator it=m_path.begin();
				it!=m_path.end(); it++) {
			if (ret.size() > 0) {
				ret.append("::");
			}
			ret.append(*it);
		}
	}

	return ret;
}

} /* namespace psi */
