/*
 * ActionImp.h
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
 *  Created on: Apr 23, 2016
 *      Author: ballance
 */

#ifndef INCLUDED_ACTION_IMP_H
#define INCLUDED_ACTION_IMP_H
#include <string>

#include "classlib/Action.h"
#include "NamedBaseItemImp.h"
#include "ScopeImp.h"
#include "TypePathImp.h"
#include "api/IExecCallback.h"

using namespace psi_api;

namespace pss {

class ElaboratorImpl;
class ActionImp : public NamedBaseItemImp {
friend ElaboratorImpl;
public:

		ActionImp(Action *master, ScopeImp *p);

		virtual ~ActionImp();

		const TypePathImp &getSuperType() const { return m_super_type; }

		virtual void inline_exec_pre(IModel *ctxt, psshandle_t hndl);

		virtual void pre_solve();
		virtual void body();
		virtual void post_solve();

		virtual void inline_exec_post();

		IModel *getModel() const { return m_model; }

		psshandle_t getHandle() const { return m_hndl; }


	private:
		TypePathImp						m_super_type;
		IModel							*m_model;
		psshandle_t						m_hndl;

};

} /* namespace pss */

#endif /* INCLUDED_ACTION_H */
