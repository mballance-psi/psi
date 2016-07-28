/*
 * Field.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *  Created on: Apr 26, 2016
 *      Author: ballance
 */

#ifndef INCLUDED_FIELD_H
#define INCLUDED_FIELD_H

#include <string>

#include "classlib/FieldBase.h"
#include "classlib/With.h"

namespace psi {

template <class T> class Field : public FieldBase<T> {
	public:

		Field(BaseItem *p, const std::string &name) :
			FieldBase<T>(FieldItem::AttrNone, p, name) { }

		virtual ~Field() { }

		With with(const ExprList &l) const { return With(FieldBase<T>::m_field, l); }
};

}




#endif /* SRC_CLASSLIB_FIELD_H_ */