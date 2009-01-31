/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "script_evaluator.h"
#include <kroll/kroll.h>

using namespace kroll;

ScriptEvaluator::~ScriptEvaluator() {
	// TODO Auto-generated destructor stub
}

std::string ScriptEvaluator::BSTRToString(BSTR bstr)
{
	std::wstring wstr(bstr);
	return std::string(wstr.begin(), wstr.end());
}

std::string ScriptEvaluator::GetModuleName(std::string type)
{
	if (type.find("text/") == 0) {
		type = type.substr(5);
	}

	std::string moduleName = "";
	moduleName += (char)std::toupper(type.at(0));
	moduleName += type.substr(1);

	return moduleName;
}

HRESULT STDMETHODCALLTYPE
ScriptEvaluator::matchesMimeType(BSTR mimeType, BOOL *result)
{
	std::string moduleName = GetModuleName(BSTRToString(mimeType));
	SharedBoundObject global = host->GetGlobalObject();

	SharedValue moduleValue = global->Get(moduleName.c_str());
	*result = FALSE;
	if (!moduleValue->IsNull() && moduleValue->IsObject()) {
		if (!moduleValue->ToObject()->Get("evaluate")->IsNull()
				&& !moduleValue->ToObject()->Get("evaluate")->IsUndefined()
				&& moduleValue->ToObject()->Get("evaluate")->IsMethod()) {
			*result = TRUE;
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE
ScriptEvaluator::evaluate(BSTR mimeType, BSTR sourceCode)
{
	std::string type = BSTRToString(mimeType);
	std::string moduleName = GetModuleName(type);
	SharedBoundObject global = host->GetGlobalObject();

	SharedValue moduleValue = global->Get(moduleName.c_str());
	if (!moduleValue->IsNull()) {
		SharedValue evalValue = moduleValue->ToObject()->Get("evaluate");
		if (!evalValue->IsNull() && evalValue->IsMethod()) {
			ValueList args;
			SharedValue typeValue = Value::NewString(type);
			SharedValue sourceCodeValue = Value::NewString(BSTRToString(sourceCode));
			args.push_back(typeValue);
			args.push_back(sourceCodeValue);

			evalValue->ToMethod()->Call(args);
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE
ScriptEvaluator::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = 0;
	if (IsEqualGUID(riid, IID_IUnknown)) {
		*ppvObject = static_cast<IUnknown*>(this);
	}
	/*else if (IsEqualGUID(riid, IID_IWebScriptEvaluator)) {
		*ppvObject = static_cast<IWebScriptEvaluator*>(this);
	}*/
	else {
		return E_NOINTERFACE;
	}
	return S_OK;
}

ULONG STDMETHODCALLTYPE
ScriptEvaluator::AddRef()
{
	return ++ref_count;
}

ULONG STDMETHODCALLTYPE
ScriptEvaluator::Release()
{
	ULONG new_count = --ref_count;
	if (!new_count) delete(this);

	return new_count;
}