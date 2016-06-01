/*
 * IActionObject.h
 *
 *  Created on: Jun 1, 2016
 *      Author: ballance
 */

#ifndef SRC_PSI_API_IACTIONOBJECT_H_
#define SRC_PSI_API_IACTIONOBJECT_H_

namespace psi_api {

class IActionObject : public IObject {
public:

	virtual ~IActionObject() { }

	/**
	 * Returns the object representation of a
	 * field of this struct.
	 */
	IObject getField(IField *field) = 0;


};

}




#endif /* SRC_PSI_API_IACTIONOBJECT_H_ */
