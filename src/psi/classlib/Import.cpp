/*
 * Import.cpp
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
 *  Created on: Apr 29, 2016
 *      Author: ballance
 */

#include "classlib/Import.h"

#include "classlib/ExprList.h"


namespace psi {

Import::Import(BaseItem *p, const std::string &name, const ExprList &plist) :
	BaseItem(BaseItem::TypeImport, p, name){
}

Import::Import(BaseItem *p, const std::string &name,
		const BaseItem &ret, const ExprList &plist) :
	BaseItem(BaseItem::TypeImport, p, name) { }

Import::~Import() {
	// TODO Auto-generated destructor stub
}

ExprImportCall Import::operator()(const ExprList &plist) {
	return ExprImportCall(*this, plist);
}

ExprImportCall Import::operator()() {
	return ExprImportCall(*this, ExprList());
}

} /* namespace psi */
