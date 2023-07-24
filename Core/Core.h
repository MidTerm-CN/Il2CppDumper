#pragma once

#define ApiManager Il2CppResolver->il2CppManager

struct PreAnalysisKlass
{
	std::string assemblyName;
	std::string nameSpace;
	std::string name;
};

inline const std::vector<PreAnalysisKlass> preAnalysisKlassList =
{
	PreAnalysisKlass({ "Assembly-CSharp", "", "WeaponManager" }),
};

struct FieldStructure
{
	std::string name;
	std::string type;
	std::string offset;
};

struct MethodPrameterStructure
{
	std::string name;
	std::string type;
};

struct MethodStructure
{
	std::string name;
	std::string flags;
	std::string signature;
	std::vector<MethodPrameterStructure> parameters;
	std::string returnType;
};

struct KlassStructure
{
	PreAnalysisKlass klass;
	std::string name;
	std::vector<std::string> inheritance;
	std::string flags;
	std::string baseKlass;
	std::vector<FieldStructure> fields;
	std::vector<FieldStructure> staticFields;
	std::vector<MethodStructure> methods;
	std::vector<MethodStructure> staticMethods;
	std::vector<KlassStructure> nestedKlasses;
};

inline std::string GetKlassFullName(Il2CppClass* klass)
{
	std::string nameSpace = ApiManager.GetClassNamespace(klass);
	std::string name = ApiManager.GetClassName(klass);
	if (nameSpace.empty())
		return name;
	return nameSpace + "::" + name;
}

inline std::string GetTypeFullName(const Il2CppType* type)
{
	std::string name = ApiManager.GetTypeName(type);
	for (auto c : name)  if (c == '.') name.replace(name.find(c), 1, "::");
	return name;
}

inline std::vector<std::string> GetKlassInheritance(Il2CppClass* klass)
{
	std::string klassName = GetKlassFullName(klass);
	std::vector<std::string> inheritance = { klassName };
	klass = ApiManager.GetClassParent(klass);
	while (klass != nullptr)
	{
		klassName = GetKlassFullName(klass);
		inheritance.push_back(klassName);
		klass = ApiManager.GetClassParent(klass);
	}
	return inheritance;
}

inline std::string GetKlassFlag(int flagIndex)
{
	std::string flags = "";
	if (flagIndex & 0x0001) flags += "Private | ";
	if (flagIndex & 0x0002) flags += "Famandassem | ";
	if (flagIndex & 0x0003) flags += "Assembly | ";
	if (flagIndex & 0x0004) flags += "Family | ";
	if (flagIndex & 0x0005) flags += "Famorassem | ";
	if (flagIndex & 0x0006) flags += "Public | ";
	if (flagIndex & 0x0010) flags += "Static | ";
	if (flagIndex & 0x0020) flags += "Initonly | ";
	if (flagIndex & 0x0040) flags += "Const | ";
	if (flagIndex & 0x0080) flags += "Notserialized | ";
	if (flagIndex & 0x0200) flags += "Specialname | ";
	if (flagIndex & 0x2000) flags += "Pinvokeimpl | ";
	for (size_t i = 0; i < 2; i++)
		flags.pop_back();
	return flags;
}

inline std::string GetMethodFlag(int flagIndex)
{
	std::string flags = "";
	if (flagIndex & 0x0001) flags += "Private | ";
	if (flagIndex & 0x0002) flags += "Famandassem | ";
	if (flagIndex & 0x0003) flags += "Assembly | ";
	if (flagIndex & 0x0004) flags += "Family | ";
	if (flagIndex & 0x0005) flags += "Famorassem | ";
	if (flagIndex & 0x0006) flags += "Public | ";
	if (flagIndex & 0x0010) flags += "Static | ";
	if (flagIndex & 0x0020) flags += "Final | ";
	if (flagIndex & 0x0040) flags += "Virtual | ";
	if (flagIndex & 0x0080) flags += "Hidebysig | ";
	if (flagIndex & 0x0100) flags += "Newslot | ";
	if (flagIndex & 0x0200) flags += "Strict | ";
	if (flagIndex & 0x0400) flags += "Abstract | ";
	if (flagIndex & 0x0800) flags += "Specialname | ";
	if (flagIndex & 0x2000) flags += "Pinvokeimpl | ";
	if (flagIndex & 0x4000) flags += "Hassecurity | ";
	if (flagIndex & 0x8000) flags += "Requiresecobject | ";
	for (size_t i = 0; i < 2; i++)
		flags.pop_back();
	return flags;
}
 
inline std::string GetKlassParent(Il2CppClass* klass)
{
	if (klass = ApiManager.GetClassParent(klass), klass == nullptr)
		return std::string();
	return ApiManager.GetClassNamespace(klass) + std::string("::") + ApiManager.GetClassName(klass);
}

inline FieldStructure AnalysisField(FieldInfo* field)
{
	FieldStructure fieldStructure;
	fieldStructure.name = ApiManager.GetFieldName(field);
	const Il2CppType* type = ApiManager.GetFieldType(field);
	fieldStructure.type = GetTypeFullName(type);
	fieldStructure.offset = std::to_string(ApiManager.GetFieldOffset(field));
	return fieldStructure;
}

inline MethodStructure AnalysisMethod(const MethodInfo* method)
{
	MethodStructure methodStructure;
	methodStructure.name = ApiManager.GetMethodName(method);
	methodStructure.flags = GetMethodFlag(ApiManager.GetMethodFlags(method, NULL));
	methodStructure.signature = Signature::Method::Create(method, ApiManager);
	const Il2CppType* returnType = ApiManager.GetMethodReturnType(method);
	methodStructure.returnType = GetTypeFullName(returnType);
	int paramCount = ApiManager.GetMethodParamCount(method);
	for (int i = 0; i < paramCount; i++)
	{
		const Il2CppType* param = ApiManager.GetMethodParam(method, i);
		MethodPrameterStructure paramStructure;
		paramStructure.name = ApiManager.GetMethodParamName(method, i);
		paramStructure.type = GetTypeFullName(param);
		methodStructure.parameters.push_back(paramStructure);
	}
	return methodStructure;
}

inline std::string GetKlassAssemble(Il2CppClass* klass)
{
	struct _Il2CppAssembly
	{
		char __pad__[sizeof(void*) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t)];
		const char* name;
	};
	const Il2CppImage* image = ApiManager.GetClassImage(klass);
	if (!image)
		return std::string();
	const Il2CppAssembly* assembly = ApiManager.GetImageAssembly(image);
	if (!assembly)
		return std::string();
	const _Il2CppAssembly* _assembly = (_Il2CppAssembly*)assembly;
	if (!_assembly)
		return std::string();
	return _assembly->name;
}

inline KlassStructure AnalysisKlass(Il2CppClass* klass)
{
	KlassStructure klassStructure;
	if (klass == nullptr)
	{
		logger.LogError("AnalysisKlass: klass is nullptr");
		return klassStructure;
	}
	klassStructure.name = ApiManager.GetClassName(klass);
	klassStructure.klass.name = klassStructure.name;
	klassStructure.klass.nameSpace = ApiManager.GetClassNamespace(klass);
	klassStructure.klass.assemblyName = GetKlassAssemble(klass);
	klassStructure.inheritance = GetKlassInheritance(klass);
	klassStructure.flags = GetKlassFlag(ApiManager.GetClassFlags(klass));
	klassStructure.baseKlass = GetKlassParent(klass);
	
	void* iterator = nullptr;
	FieldInfo* field = nullptr;
	while ((field = ApiManager.GetClassFields(klass, &iterator)) != nullptr)
	{
		if (ApiManager.GetFieldFlags(field) & 0x0010)
			klassStructure.staticFields.push_back(AnalysisField(field));
		else
			klassStructure.fields.push_back(AnalysisField(field));
	}

	iterator = nullptr;
	const MethodInfo* method = nullptr;
	while ((method = ApiManager.GetClassMethods(klass, &iterator)) != nullptr)
	{
		std::string methodName = ApiManager.GetMethodName(method);
		if (methodName.find(".ctor") != std::string::npos)
			continue;
		if (ApiManager.GetMethodFlags(method, NULL) & 0x0010)
			klassStructure.staticMethods.push_back(AnalysisMethod(method));
		else
			klassStructure.methods.push_back(AnalysisMethod(method));
	}
	
	iterator = nullptr;
	Il2CppClass* nestedKlass = nullptr;
	while ((nestedKlass = ApiManager.GetClassNestedTypes(klass, &iterator)) != nullptr)
	{
		std::string nestedKlassName = ApiManager.GetClassName(nestedKlass);
		klassStructure.nestedKlasses.push_back(AnalysisKlass(nestedKlass));
	}
	return klassStructure;
}

inline std::string GenerateKlassStructure(KlassStructure klass)
{
	std::string result = "";
	std::string inheritance = "";
	for (auto& item : klass.inheritance)
		inheritance += item + (item != klass.inheritance.back() ? std::string(" -> ") : "");
	
	result += "// Name: " + klass.name + "\n";
	result += "// Flags: " + klass.flags + "\n";
	result += "// Inheritance: " + inheritance + "\n";
	result += "class " + klass.name;
	if (!klass.baseKlass.empty())
		result += " : public " + klass.baseKlass;
	result += "\n";
	result += "{\n";
	if (!klass.nestedKlasses.empty())
	{
		result += "public:\n";
		for (auto& item : klass.nestedKlasses)
		{
			std::string nestedKlass = GenerateKlassStructure(item);
			result += nestedKlass;
		}
	}
	result += "public:\n";
	result += "\tCLASS(\"" + klass.klass.assemblyName + "\", \"" + klass.klass.nameSpace + "\", \"" + klass.klass.name + "\");\n\n";
	
	if (!klass.staticFields.empty())
	{
		for (auto& item : klass.staticFields)
		{
			result += "\tSTATIC_MEMBER(" + item.type + ", " + item.name + ", " + item.offset + ");\n";
		}
		result += "\n";
	}
	if (!klass.fields.empty())
	{
		result += "\tunion\n\t{\n";
		for (auto& item : klass.fields)
		{
			result += "\t\tMEMBER(" + item.type + ", " + item.name + ", " + item.offset + ");\n";
		}
		result += "\t};\n";
	}
	if (!klass.staticFields.empty() || !klass.fields.empty())
		result += "\n";
	if (!klass.staticMethods.empty())
	{
		for (auto& item : klass.staticMethods)
		{
			result += "\tstatic " + item.returnType + " " + item.name + "(";
			for (auto& param : item.parameters)
			{
				result += param.type + " " + param.name + (param.name != item.parameters.back().name ? ", " : "");
			}
			result += ")\n\t{\n";
			result += "\t\tMETHOD(" + item.returnType + ", (";
			for (auto& param : item.parameters)
			{
				result += param.type + (param.name != item.parameters.back().name ? ", " : "");
			}
			result += ", MethodInfo*), " + item.signature + ");\n";
			result += "\t\treturn _fn(";
			for (auto& param : item.parameters)
			{
				result += param.name + (param.name != item.parameters.back().name ? ", " : "");
			}
			result += ", nullptr);\n";
			result += "\t}\n\n";
		}
		result += "\n";
	}
	if (!klass.methods.empty())
	{
		for (auto& item : klass.methods)
		{
			result += "\t" + item.returnType + " " + item.name + "(";
			for (auto& param : item.parameters)
			{
				result += param.type + " " + param.name + (param.name != item.parameters.back().name ? ", " : "");
			}
			result += ")\n\t{\n";
			result += "\t\tMETHOD(" + item.returnType + ", (" + klass.name + "*" + (item.parameters.empty() ? "" : ", ");
			for (auto& param : item.parameters)
			{
				result += param.type + (param.name != item.parameters.back().name ? ", " : "");
			}
			result += ", MethodInfo*), \"" + item.signature + "\");\n";
			result += "\t\treturn _fn(this" + std::string(item.parameters.empty() ? "" : ", ");
			for (auto& param : item.parameters)
			{
				result += param.name + (param.name != item.parameters.back().name ? ", " : "");
			}
			result += ", nullptr);\n";
			result += "\t}\n";
			if (item.name != klass.methods.back().name)
				result += "\n";
		}
		result += "\n";
	}
	
	result += "};\n\n";
	return result;
}

inline void RunResolver()
{
	char gamePath[MAX_PATH];
	GetModuleFileNameA(NULL, gamePath, MAX_PATH);
	std::filesystem::path path = std::filesystem::path(gamePath).parent_path() / "dumper";
	if (!std::filesystem::exists(path)) std::filesystem::create_directory(path);
	
	logger.LogInfo("NaDUGR is running!");
	logger.LogInfo("PreAnalysisKlassList size: %d", preAnalysisKlassList.size());
	for (size_t i = 0; i < preAnalysisKlassList.size(); i++)
	{
		logger.LogInfo("PreAnalysisKlassList[%d] name: %s", i, preAnalysisKlassList[i].name.c_str());
		logger.LogInfo("PreAnalysisKlassList[%d] nameSpace: %s", i, preAnalysisKlassList[i].nameSpace.c_str());
		logger.LogInfo("PreAnalysisKlassList[%d] assemblyName: %s", i, preAnalysisKlassList[i].assemblyName.c_str());
	}
	logger.LogInfo("Work strat.\n\n");
	std::vector<KlassStructure> klassStructures;
	for (auto& klass : preAnalysisKlassList)
	{
		clock_t startTime = clock();
		logger.LogInfo("AnalysisKlass: %s", klass.name.c_str());
		Il2CppClass* il2CppClass = Il2CppResolver->GetClassEx(klass.assemblyName, klass.nameSpace, klass.name);
		if (il2CppClass == nullptr)
		{
			logger.LogError("AnalysisKlass: %s failed!", klass.name.c_str());
			continue;
		}
		klassStructures.push_back(AnalysisKlass(il2CppClass));
		logger.LogInfo("AnalysisKlass: %s success (%dms)!\n\n", klass.name.c_str(), clock() - startTime);
	}
	for (auto& klass : klassStructures)
	{
		logger.LogInfo("Klass name: %s", klass.name.c_str());
		std::string inheritance = "";
		for (auto& item : klass.inheritance)
			inheritance += item + (item != klass.inheritance.back() ? std::string(" -> ") : "");
		logger.LogInfo("Inheritance: %s", inheritance.c_str());
		logger.LogInfo("Flags: %s", klass.flags.c_str());
		logger.LogInfo("BaseKlass: %s", klass.baseKlass.c_str());
		logger.LogInfo("Fields:");
		for (auto& field : klass.fields)
			logger.LogInfo("    %s %s %s", field.type.c_str(), field.name.c_str(), field.offset.c_str());
		logger.LogInfo("StaticFields:");
		for (auto& field : klass.staticFields)
			logger.LogInfo("    %s %s %s", field.type.c_str(), field.name.c_str(), field.offset.c_str());
		logger.LogInfo("Methods:");
		for (auto& method : klass.methods)
		{
			std::string name = method.name + "(";
			for (auto& item : method.parameters)
			{
				name += item.type + " " + item.name + (item.name != method.parameters.back().name ? std::string(", ") : "");
			}
			name += ")";
			logger.LogInfo("    %s %s    [%s]", method.returnType.c_str(), name.c_str(), method.signature.c_str());
		}
		logger.LogInfo("StaticMethods:");
		for (auto& method : klass.staticMethods)
		{
			std::string name = method.name + "(";
			for (auto& item : method.parameters)
				name += item.type + (item.type != method.parameters.back().type ? std::string(", ") : ")");
			name += ")";
			logger.LogInfo("    %s %s    [%s]", method.returnType.c_str(), name.c_str(), method.signature.c_str());
		}
		logger.LogInfo("NestedKlasses:");
		for (auto& nestedKlass : klass.nestedKlasses)
			logger.LogInfo("    %s", nestedKlass.name.c_str());
		std::string klassStructure = GenerateKlassStructure(klass);
		std::filesystem::path logFilePath = path / (klass.name + std::string(".h"));
		std::ofstream file = std::ofstream(logFilePath, std::ios::out);
		file << klassStructure;
	}
	logger.LogInfo("NaDUGR is done!");
}