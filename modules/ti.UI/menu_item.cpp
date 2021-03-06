/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include "menu_item.h"

namespace ti
{

	MenuItem::MenuItem() : AccessorBoundList()
	{

		// query methods
		this->SetMethod("isSeparator", &MenuItem::_IsSeparator);
		this->SetMethod("isItem", &MenuItem::_IsItem);
		this->SetMethod("isSubMenu", &MenuItem::_IsSubMenu);

		// submenu methods
		this->SetMethod("addSeparator", &MenuItem::_AddSeparator);
		this->SetMethod("addItem", &MenuItem::_AddItem);
		this->SetMethod("addSubMenu", &MenuItem::_AddSubMenu);

		this->SetMethod("enable", &MenuItem::_Enable);
		this->SetMethod("disable", &MenuItem::_Disable);

		this->SetMethod("setLabel", &MenuItem::_SetLabel);
		this->SetMethod("setIcon", &MenuItem::_SetIcon);

		// mutators
		//this->SetMethod("makeSeparator", &MenuItem::_MakeSeparator);
		//this->SetMethod("makeItem", &MenuItem::_MakeItem);
		//this->SetMethod("makeSubMenu", &MenuItem::_MakeSubMenu);
	}

	MenuItem::~MenuItem()
	{
	}

	void MenuItem::SetMethod(const char *name, void (MenuItem::*method)(const ValueList&, SharedValue))
	{
		MethodCallback* callback = NewCallback<MenuItem, const ValueList&, SharedValue>(static_cast<MenuItem*>(this), method);
		SharedBoundMethod bound_method = new StaticBoundMethod(callback);
		SharedValue method_value = Value::NewMethod(bound_method);
		this->RawSet(name, method_value);
	}

	bool MenuItem::IsSeparator()
	{
		SharedValue type = this->RawGet("type");
		return (type->IsInt() && type->ToInt() == SEP);
	}

	bool MenuItem::IsItem()
	{
		SharedValue type = this->RawGet("type");
		return (type->IsInt() && type->ToInt() == ITEM);
	}

	bool MenuItem::IsSubMenu()
	{
		SharedValue type = this->RawGet("type");
		return (type->IsInt() && type->ToInt() == SUBMENU);
	}

	void MenuItem::_IsSeparator(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsSeparator());
	}

	void MenuItem::_IsItem(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsItem());
	}

	void MenuItem::_IsSubMenu(const ValueList& args, SharedValue result)
	{
		result->SetBool(this->IsSubMenu());
	}

	void MenuItem::_AddSeparator(const ValueList& args, SharedValue result)
	{
		SharedValue new_item = this->AddSeparator();
		result->SetValue(new_item);
	}

	void MenuItem::_AddItem(const ValueList& args, SharedValue result)
	{
		SharedValue label = Value::Undefined;
		SharedValue icon_url = Value::Undefined;
		SharedValue callback = Value::Undefined;

		if (args.size() > 0 && args.at(0)->IsString())
			label = args.at(0);

		if (args.size() > 1 && args.at(1)->IsMethod())
			callback = args.at(1);

		if (args.size() > 2 && args.at(2)->IsString())
			icon_url = args.at(2);

		SharedValue new_item = this->AddItem(label, callback, icon_url);
		result->SetValue(new_item);
	}

	void MenuItem::_AddSubMenu(const ValueList& args, SharedValue result)
	{
		SharedValue label = Value::Undefined;
		SharedValue icon_url = Value::Undefined;

		if (args.size() > 0 && args.at(0)->IsString())
			label = args.at(0);

		if (args.size() > 1 && args.at(1)->IsString())
			icon_url = args.at(1);

		SharedValue new_item = this->AddSubMenu(label, icon_url);
		result->SetValue(new_item);
	}

	void MenuItem::_Enable(const ValueList& args, SharedValue result)
	{
		this->Enable();
		this->RawSet("enabled", Value::NewBool(true));
	}

	void MenuItem::_Disable(const ValueList& args, SharedValue result)
	{
		this->Disable();
		this->RawSet("enabled", Value::NewBool(false));
	}

	void MenuItem::_SetLabel(const ValueList& args, SharedValue result)
	{
		this->RawSet("label", args.at(0));

		std::string label = std::string(args.at(0)->ToString());
		this->SetLabel(label);
	}
	void MenuItem::_SetIcon(const ValueList& args, SharedValue result)
	{
		this->RawSet("icon", args.at(0));
		std::string icon_path = std::string(args.at(0)->ToString());
		this->SetLabel(icon_path);
	}

	/* The function below, modify the bound object values
	 * of this object and are used by subclasses to ensure
	 * a consistent state */
	void MenuItem::MakeSeparator()
	{
		this->RawSet("type", Value::NewInt(SEP));
		this->RawSet("iconURL", Value::Undefined);
		this->RawSet("label", Value::Undefined);
	}

	void MenuItem::MakeItem(SharedValue label,
	                       SharedValue callback,
	                       SharedValue icon_url)
	{
		this->RawSet("type", Value::NewInt(ITEM));
		this->RawSet("label", label);
		this->RawSet("callback", callback);
		this->RawSet("iconURL", icon_url);
	}

	void MenuItem::MakeSubMenu(SharedValue label,
	                          SharedValue icon_url)
	{
		this->RawSet("type", Value::NewInt(SUBMENU));
		this->RawSet("label", label);
		this->RawSet("iconURL", icon_url);
		this->RawSet("callback", Value::Undefined);
	}

	SharedValue MenuItem::AddToListModel(MenuItem* item)
	{
		SharedBoundList so = SharedBoundList(item);
		SharedValue v = Value::NewList(so);
		this->Append(v);
		return v;
	}


	/* Handy accessor functions */
	const char* MenuItem::GetLabel()
	{
		SharedValue label_value = this->RawGet("label");
		if (label_value->IsString())
			return label_value->ToString();
		else
			return NULL;
	}

	const char* MenuItem::GetIconURL()
	{
		SharedValue label_value = this->RawGet("iconURL");
		if (label_value->IsString())
			return label_value->ToString();
		else
			return NULL;
	}

}
