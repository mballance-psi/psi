/*
 * StateStruct.h
 *
 *  Created on: May 10, 2016
 *      Author: ballance
 */

#ifndef SRC_PSI_CLASSLIB_STATESTRUCT_H_
#define SRC_PSI_CLASSLIB_STATESTRUCT_H_

#include <string>
#include <vector>
#include "classlib/Struct.h"
#include "classlib/Rand.h"
#include "classlib/Bool.h"

namespace psi {

class Scope;
class StateStruct: public Struct {
public:
	friend TypeRgy<StateStruct>;

	Rand<Bool>			initial;

	StateStruct(const Scope &p, const std::string &name);

	virtual ~StateStruct();

private:
	StateStruct(const Scope &p);
};

} /* namespace psi */

#endif /* SRC_PSI_CLASSLIB_STATESTRUCT_H_ */
