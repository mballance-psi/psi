/*
 * NamedItemImpl.h
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

#ifndef IMPL_NAMEDITEMIMPL_H_
#define IMPL_NAMEDITEMIMPL_H_
#include <string>

#include "../api_impl/BaseItemImpl.h"
#include "api/INamedItem.h"

using namespace psi_api;

namespace psi {

class NamedItemImpl : public virtual INamedItem, public BaseItemImpl {
	public:

		NamedItemImpl(const std::string &name, IBaseItem::ItemType t);

		virtual ~NamedItemImpl();

		virtual const std::string &getName();


	private:

		std::string					m_name;
};

} /* namespace psi */

#endif /* IMPL_NAMEDITEMIMPL_H_ */
