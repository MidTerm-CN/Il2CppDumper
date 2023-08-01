#pragma once
#include <regex>

#define ApiManager Il2CppResolver->il2CppManager

inline std::unordered_map<std::string, std::string> csTypeMap
{
	{ "System::Boolean",				"bool" },
	{ "System::Byte",					"uint8_t" },
	{ "System::SByte",					"int8_t" },
	{ "System::Char",					"uint16_t" },
	{ "System::Int16",					"int16_t" },
	{ "System::UInt16",					"uint16_t" },
	{ "System::Int32",					"int32_t" },
	{ "System::UInt32",					"uint32_t" },
	{ "System::Int64",					"int64_t" },
	{ "System::UInt64",					"uint64_t" },
	{ "System::Single",					"float" },
	{ "System::Double",					"double" },
	{ "System::String",					"Il2CppString*" },
	{ "System::Object",					"Il2CppObject*" },
	{ "System::Void",					"void" },
	{ "System::IntPtr",					"intptr_t"},
	{ "System::Type",					"Il2CppType*"},
	{ "System::MulticastDelegate",		"Il2CppMulticastDelegate*" },
	{ "System::Delegate",				"Il2CppDelegate*" },
	{ "System::Func",					"Il2CppMulticastDelegate*" },
	{ "System::Action",					"Il2CppMulticastDelegate*" },
};

const std::unordered_map<std::string, std::string> csOperatorMap
{
	{ "decrement",		"--" },
	{ "increment",		"++" },
};

struct PreAnalysisKlass
{
	std::string assemblyName;
	std::string nameSpace;
	std::string name;
	Il2CppClass* klass = nullptr;

	bool notSupported = false;
};

inline std::vector<PreAnalysisKlass> preAnalysisKlassList =
{

};

inline std::vector<std::string> preAnalysisAssemblyList =
{
	"Assembly-CSharp",
	"UnityEngine.CoreModule",
	"UnityEngine.AssetBundleModule",
	"UnityEngine.AudioModule",
};

struct FieldStructure
{
	std::string name;
	std::string type;
	std::string value;
	std::string offset;

	std::vector<struct FieldStructure> elements;

	Il2CppClass* klass = nullptr;
};

struct MethodPrameterStructure
{
	std::string name;
	std::string type;

	Il2CppClass* klass = nullptr;
};

struct MethodStructure
{
	std::string name;
	std::string flags;
	std::string signature;
	std::vector<MethodPrameterStructure> parameters;
	std::string returnType;

	bool operatorMethod = false;

	MethodInfo* method = nullptr;
	Il2CppClass* returnKlass = nullptr;
};

struct KlassStructure
{
	PreAnalysisKlass klass;
	std::string fullName;
	std::string name;
	std::vector<std::string> inheritance;
	std::string flags;
	std::string baseKlass;
	std::vector<FieldStructure> fields;
	std::vector<FieldStructure> staticFields;
	std::vector<MethodStructure> methods;
	std::vector<MethodStructure> staticMethods;
	std::vector<KlassStructure> nestedKlasses;

	std::vector<std::string> genericParameters;

	bool nested = false;

	bool enumKlass = false;
	bool genericKlass = false;
	bool realGenericKlass = false;
	bool valueKlass = false;
	
	bool illegality = false;

	bool processed = false;
};

inline std::string GetAssemblyName(const Il2CppAssembly* assembly)
{
	struct _Il2CppAssembly
	{
		char __pad__[sizeof(void*) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t)];
		const char* name;
	};
	if (!assembly)
		return std::string();
	const _Il2CppAssembly* _assembly = (_Il2CppAssembly*)assembly;
	if (!_assembly)
		return std::string();
	return _assembly->name;
}

inline std::string GetKlassAssembly(Il2CppClass* klass)
{

	const Il2CppImage* image = ApiManager.GetClassImage(klass);
	if (!image)
		return std::string();
	const Il2CppAssembly* assembly = ApiManager.GetImageAssembly(image);
	if (!assembly)
		return std::string();
	return GetAssemblyName(assembly);
}

inline std::string GetKlassFullName(Il2CppClass* klass)
{
	std::string nameSpace = ApiManager.GetClassNamespace(klass);
	std::string name = ApiManager.GetClassName(klass);
	if (nameSpace.empty())
		return name;
	while (nameSpace.find(".") != std::string::npos) nameSpace.replace(nameSpace.find("."), 1, "::");
	return nameSpace + "::" + name;
}

inline std::string GetTypeFullName(const Il2CppType* type)
{
	std::string name = ApiManager.GetTypeName(type);
	while (name.find(".") != std::string::npos) name.replace(name.find("."), 1, "::");
	return name;
}

inline bool ClassIsIllegality(const std::string& name)
{
	const std::vector<std::string> ilegalityRegex =
	{
		{ "(<.*>d__\\d+)" },			// 协程类
		{ "<>c__DisplayClass(.*)" },	// 匿名类
		{ "<>c" },						// 匿名类
		{ "(<.*>e__.*)" },						// 匿名类
	};
	for (const auto& item : ilegalityRegex)
	{
		std::regex regex = std::regex(item);
		bool illegality = std::regex_match(name, regex);
		if (illegality)
			return true;
	}
	return false;
}

inline bool IsBackingField(const std::string& name)
{
	std::regex regex("<(.*)>k__BackingField");
	return std::regex_match(name, regex);
}

inline void BackingFieldNameRationalization(std::string& name)
{
	std::regex regex("<(.*)>k__BackingField");
	name = std::regex_replace(name, regex, "backingField__$1");
}

inline bool ClasseIsArray(const std::string& name)
{
	for (size_t i = name.size() - 1; i > 0; i--)
	{
		if (name[i] == ']')
		{
			return true;
		}
	}
	std::regex regex("Array<(.*)>*");
	return std::regex_match(name, regex);
}

inline bool ClassIsGeneric(const std::string& name)
{
	std::regex regex("(.*)<(.*)>");
	return std::regex_match(name, regex);
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
	if (flags.empty()) return flags;
	for (size_t i = 0; i < 3; i++)
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
	if (flags.empty()) return flags;
	for (size_t i = 0; i < 3; i++)
		flags.pop_back();
	return flags;
}

inline std::string GetKlassParent(Il2CppClass* klass)
{
	if (klass = ApiManager.GetClassParent(klass), klass == nullptr)
		return std::string();
	return GetKlassFullName(klass);
}



inline FieldStructure AnalysisField(FieldInfo* field)
{
	FieldStructure fieldStructure;
	fieldStructure.name = ApiManager.GetFieldName(field);
	const Il2CppType* type = ApiManager.GetFieldType(field);
	fieldStructure.type = GetTypeFullName(type);
	fieldStructure.offset = std::to_string(ApiManager.GetFieldOffset(field));
	Il2CppClass* klass = ApiManager.GetClassFromType(type);
	fieldStructure.klass = klass;
	return fieldStructure;
}

inline MethodStructure AnalysisMethod(const MethodInfo* method)
{
	MethodStructure methodStructure;
	methodStructure.method = (MethodInfo*)method;
	methodStructure.name = ApiManager.GetMethodName(method);
	methodStructure.flags = GetMethodFlag(ApiManager.GetMethodFlags(method, NULL));
	methodStructure.signature = Signature::Method::Create(method, ApiManager);
	const Il2CppType* returnType = ApiManager.GetMethodReturnType(method);
	methodStructure.returnType = GetTypeFullName(returnType);
	methodStructure.returnKlass = ApiManager.GetClassFromType(returnType);
	int paramCount = ApiManager.GetMethodParamCount(method);
	for (int i = 0; i < paramCount; i++)
	{
		const Il2CppType* param = ApiManager.GetMethodParam(method, i);
		MethodPrameterStructure paramStructure;
		paramStructure.name = ApiManager.GetMethodParamName(method, i);
		paramStructure.type = GetTypeFullName(param);
		Il2CppClass* klass = ApiManager.GetClassFromType(param);
		paramStructure.klass = klass;
		methodStructure.parameters.push_back(paramStructure);
	}
	return methodStructure;
}

inline void PretreatmentKlass(KlassStructure& klass);
inline KlassStructure AnalysisKlass(Il2CppClass* klass)
{
	KlassStructure klassStructure;
	if (klass == nullptr)
	{
		logger.LogError("AnalysisKlass: klass is nullptr");
		return klassStructure;
	}
	klassStructure.fullName = GetKlassFullName(klass);
	klassStructure.name = ApiManager.GetClassName(klass);
	if (ClassIsIllegality(klassStructure.name))
	{
		klassStructure.illegality = true;
		return klassStructure;
	}
	klassStructure.klass.klass = klass;
	klassStructure.klass.name = klassStructure.name;
	klassStructure.klass.nameSpace = ApiManager.GetClassNamespace(klass);
	klassStructure.klass.assemblyName = GetKlassAssembly(klass);
	klassStructure.inheritance = GetKlassInheritance(klass);
	klassStructure.flags = GetKlassFlag(ApiManager.GetClassFlags(klass));
	klassStructure.baseKlass = GetKlassParent(klass);
	klassStructure.enumKlass = ApiManager.ClassIsEnum(klass);
	klassStructure.valueKlass = ApiManager.ClassIsValueType(klass);
	klassStructure.genericKlass = ApiManager.ClassIsGeneric(klass);

	void* iterator = nullptr;
	FieldInfo* field = nullptr;
	while ((field = ApiManager.GetClassFields(klass, &iterator)) != nullptr)
	{
		if (ApiManager.GetFieldFlags(field) & 0x0010)
		{
			FieldStructure fieldStructure = AnalysisField(field);
			if (klassStructure.enumKlass)
			{
				int value = 0;
				ApiManager.StaticFieldGetValue(field, &value);
				fieldStructure.value = std::to_string(value);
			}
			klassStructure.staticFields.push_back(fieldStructure);
		}
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
		{
			MethodStructure methodStructure = AnalysisMethod(method);
			if (methodName[0] == 'o' && methodName[1] == 'p' && methodName[2] == '_')
			{
				methodStructure.name = methodStructure.name.substr(3);
				methodStructure.operatorMethod = true;
			}
			klassStructure.staticMethods.push_back(methodStructure);
		}
		else
			klassStructure.methods.push_back(AnalysisMethod(method));
	}

	iterator = nullptr;
	Il2CppClass* nestedKlass = nullptr;
	while ((nestedKlass = ApiManager.GetClassNestedTypes(klass, &iterator)) != nullptr)
	{
		std::string nestedKlassName = ApiManager.GetClassName(nestedKlass);
		KlassStructure nestedKlassStructure = AnalysisKlass(nestedKlass);
		nestedKlassStructure.nested = true;
		klassStructure.nestedKlasses.push_back(nestedKlassStructure);
	}
	PretreatmentKlass(klassStructure);
	return klassStructure;
}

inline void TypeDispose(std::string& type, Il2CppClass* klass);

inline void ArrayTypeDispose(std::string& type, Il2CppClass* klass)
{
	int arrayRank = 0;
	Il2CppClass* arrayBaseClass = klass;
	for (int i = (int)type.find_first_of('['); i < type.size(); i++)
	{
		if (type[i] == '[')
		{
			arrayBaseClass = ApiManager.GetClassElementClass(arrayBaseClass);
			arrayRank++;
		}
	}

	std::string arrayBaseClassName = GetKlassFullName(arrayBaseClass);
	TypeDispose(arrayBaseClassName, arrayBaseClass);
	type = arrayBaseClassName;
	for (size_t i = 0; i < arrayRank; i++)
	{
		type = "Array<" + type + ">*";
	}
}

inline void TypeDispose(std::string& type, Il2CppClass* klass)
{
	if (csTypeMap.find(type) != csTypeMap.end())
	{
		type = csTypeMap.at(type);
		return;
	}

	std::string originType = type;
	bool isEnum = ApiManager.ClassIsEnum(klass);
	bool isValueType = ApiManager.ClassIsValueType(klass);
	bool isGeneric = ClassIsGeneric(type);
	bool isArray = ClasseIsArray(type);
	if (isValueType || isEnum)
	{
		goto save;
	}
	if (isArray)
	{
		ArrayTypeDispose(type, klass);
		goto save;
	}
	if (isGeneric)
	{
		return;
	}
	type += "*";
save:
	if (originType != type)
		csTypeMap.insert(std::make_pair(originType, type));
}

inline void PretreatmentFieldStructure(FieldStructure& field)
{
	if (IsBackingField(field.name))
		BackingFieldNameRationalization(field.name);
	TypeDispose(field.type, field.klass);
}

inline void PretreatmentBaseKlass(std::string& name)
{
	if (csTypeMap.find(name) != csTypeMap.end())
	{
		name = csTypeMap.at(name);
		if (name.back() == '*')
			name.pop_back();
		return;
	}
}

inline void PretreatmentMethod(MethodStructure& method)
{
	TypeDispose(method.returnType, method.returnKlass);
	do
	{
		if (method.name.compare(".cctor") == 0)
		{
			method.name = "StaticConstructor";
			break;
		}
	} while (false);

	for (auto& item : method.parameters)
	{
		TypeDispose(item.type, item.klass);
	}
}

inline void PretreatmentKlass(KlassStructure& klass)
{
	if (klass.processed)
		return;
	klass.processed = true;
	PretreatmentBaseKlass(klass.baseKlass);
	for (auto& item : klass.fields)
	{
		PretreatmentFieldStructure(item);
		if (klass.genericKlass && item.type[0] == 'T')
		{
			item.type.pop_back();
			klass.genericParameters.push_back(item.type);
		}
	}
	for (auto& item : klass.staticFields)
	{
		PretreatmentFieldStructure(item);
		if (klass.genericKlass && item.type[0] == 'T')
		{
			item.type.pop_back();
			klass.genericParameters.push_back(item.type);
		}
	}
	for (auto& item : klass.methods)
	{
		PretreatmentMethod(item);
	}
	for (auto& item : klass.staticMethods)
	{
		PretreatmentMethod(item);
	}
	for (auto& item : klass.nestedKlasses)
	{
		PretreatmentKlass(item);
		if (klass.genericKlass)
		{
			klass.genericParameters.insert(klass.genericParameters.end(), item.genericParameters.begin(), item.genericParameters.end());
		}
	}
	if (klass.genericKlass && klass.name[klass.name.size() - 2] == '`')
	{
		klass.realGenericKlass = true;
		std::sort(klass.genericParameters.begin(), klass.genericParameters.end());
		klass.genericParameters.erase(std::unique(klass.genericParameters.begin(), klass.genericParameters.end()), klass.genericParameters.end());
		int parameterIndex = (int)klass.name.find_last_of('`');
		for (size_t i = 0; i < klass.name.size() - parameterIndex; i++)
		{
			klass.name.pop_back();
		}
		klass.name.pop_back();
	}
}

inline std::vector<std::string> GetGenericTypeParameterCount(const std::string& name)
{
	std::vector<std::string> result;
	std::string waitForSplit = "";
	waitForSplit = name.substr(name.find_first_of('<') + 1, name.find_last_of('>') - name.find_first_of('<') - 1);
	std::string::size_type pos = waitForSplit.find(',');
	while (pos != std::string::npos)
	{
		std::string temp = waitForSplit.substr(0, pos);
		result.push_back(temp);
		waitForSplit = waitForSplit.substr(pos + 1);
		pos = waitForSplit.find(',');
	}
	result.push_back(waitForSplit);
	return result;
}

inline void GenericTypeDispose(std::string& type)
{
	if (type.find("Action") != std::string::npos || type.find("Func") != std::string::npos)
	{
		if (csTypeMap.find(type) != csTypeMap.end())
		{
			type = csTypeMap.at(type);
			return;
		}
		type = "Il2CppMulticastDelegate*";
		return;
	}
	std::vector<std::string> genericParameters = GetGenericTypeParameterCount(type);
	for (auto& item : genericParameters)
	{
		if (csTypeMap.find(item) != csTypeMap.end())
		{
			item = csTypeMap.at(item);
		}
	}
	std::string result = "";
	result += type.substr(0, type.find_first_of('<'));
	result += "<";
	for (auto& item : genericParameters)
	{
		result += item;
		result += ", ";
	}
	result.pop_back();
	result.pop_back();
	result += ">";
	type = result;
}

inline void PretreatmentGenericType(KlassStructure& klass)
{
	for (auto& item : klass.fields)
	{
		bool isGeneric = ClassIsGeneric(item.type);
		bool isArray = ClasseIsArray(item.type);
		if (isGeneric && !isArray)
			GenericTypeDispose(item.type);
	}
	for (auto& item : klass.staticFields)
	{
		bool isGeneric = ClassIsGeneric(item.type);
		bool isArray = ClasseIsArray(item.type);
		if (isGeneric && !isArray)
			GenericTypeDispose(item.type);
	}
	for (auto& item : klass.methods)
	{
		bool isGeneric = ClassIsGeneric(item.returnType);
		bool isArray = ClasseIsArray(item.returnType);
		if (isGeneric && !isArray)
			GenericTypeDispose(item.returnType);
		for (auto& item2 : item.parameters)
		{
			isGeneric = ClassIsGeneric(item2.type);
			isArray = ClasseIsArray(item2.type);
			if (isGeneric && !isArray)
				GenericTypeDispose(item2.type);
		}
	}
	for (auto& item : klass.staticMethods)
	{
		bool isGeneric = ClassIsGeneric(item.returnType);
		bool isArray = ClasseIsArray(item.returnType);
		if (isGeneric && !isArray)
			GenericTypeDispose(item.returnType);
		for (auto& item2 : item.parameters)
		{
			isGeneric = ClassIsGeneric(item2.type);
			isArray = ClasseIsArray(item2.type);
			if (isGeneric && !isArray)
				GenericTypeDispose(item2.type);
		}
	}
	for (auto& item : klass.nestedKlasses)
	{
		PretreatmentGenericType(item);
	}
}

inline std::string GenerateKlassStructure(KlassStructure klass);

inline std::vector<std::string> GetNamespaceList(const std::string& nameSpace)
{
	std::vector<std::string> result;
	std::string waitForSplit = nameSpace;
	std::string::size_type pos = waitForSplit.find('.');
	while (pos != std::string::npos)
	{
		std::string temp = waitForSplit.substr(0, pos);
		result.push_back(temp);
		waitForSplit = waitForSplit.substr(pos + 1);
		pos = waitForSplit.find('.');
	}
	result.push_back(waitForSplit);
	return result;
}

inline std::string Format(std::string input, ...)
{
	va_list args;
	va_start(args, input);
	char buffer[1024];
	vsprintf_s(buffer, input.c_str(), args);
	va_end(args);
	return std::string(buffer);
}

inline std::string GenerateClassKlassStructure(KlassStructure klass)
{
	std::string result = "";
	std::vector<std::string> namespaceList = GetNamespaceList(klass.klass.nameSpace);

	if (!klass.klass.nameSpace.empty())
	{
		for (auto& item : namespaceList)
		{
			result += "namespace ";
			result += item;
			result += "\n{\n";
		}
	}
	
	std::string inheritance = "";
	for (auto& item : klass.inheritance)
		inheritance += item + (item != klass.inheritance.back() ? std::string(" -> ") : "");

	result += "// Name: " + klass.fullName + "\n";
	result += "// ClassType:";

	std::string klassType = "";
	if (klass.valueKlass)
		klassType += " ValueType |";
	if (klass.enumKlass)
		klassType += " Enum |";
	if (klass.genericKlass)
		klassType += " Generic |";
	klassType += " Normal";
	result += klassType + "\n";

	result += "// Flags: " + klass.flags + "\n";
	result += "// Inheritance: " + inheritance + "\n";
	if (klass.genericKlass && klass.realGenericKlass)
	{
		result += "// GenericParameters: ";
		for (auto& item : klass.genericParameters)
			result += item + (item != klass.genericParameters.back() ? std::string(", ") : "");
		result += "\n";

		result += "template <";
		for (auto& item : klass.genericParameters)
			result += "typename " + item + (item != klass.genericParameters.back() ? std::string(", ") : "");
		result += ">\n";
	}
	result += "class " + klass.name;
	if (!klass.baseKlass.empty())
	{
		std::string baseKlassString = " : public " + klass.baseKlass;
		result += baseKlassString;
		if (klass.valueKlass)
		{
			if (baseKlassString.find("System::ValueType") != std::string::npos)
			{
				for (size_t i = 0; i < baseKlassString.size(); i++)
				{
					result.pop_back();
				}
			}
		}
	}
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
			result += "\t// Flags: " + item.flags + "\n";
			result += "\t// Addresss: " + Format("%p", (uintptr_t)(*(void**)item.method)) + "\n";
			if (!item.operatorMethod)
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
					result += param.type + ", ";
				}
				result += "MethodInfo*), \"" + item.signature + "\");\n";
				result += "\t\treturn function(";
				for (auto& param : item.parameters)
				{
					result += param.name + ", ";
				}
				result += "nullptr);\n";
				result += "\t}\n\n";
			}
			else
			{
				std::transform(item.name.begin(), item.name.end(), item.name.begin(), ::tolower);
				if (item.name == "implicit")
				{
					if (item.returnType != klass.fullName)
					{
						result += "\toperator " + item.returnType + "()\n\t{\n";
						result += "\t\tMETHOD(" + item.returnType + ", (";
						for (auto& param : item.parameters)
						{
							result += param.type + "*, ";
						}
						result += "MethodInfo*), \"" + item.signature + "\");\n";
						result += "\t\treturn function(this, nullptr);\n";
						result += "\t}\n\n";
					}
					else
					{
						result += "\t" + klass.name + "(" + item.parameters[0].type + " " + item.parameters[0].name + ")\n\t{\n";
						result += "\t\tMETHOD(" + klass.fullName + ", (";
						for (auto& param : item.parameters)
						{
							result += param.type + ", ";
						}
						result += "MethodInfo*), \"" + item.signature + "\");\n";
						result += "\t\tfunction(";
						for (auto& param : item.parameters)
						{
							result += param.name + ", ";
						}
						result += "nullptr);\n";
						result += "\t}\n\n";
					}
				}
				else if (csOperatorMap.find(item.name) != csOperatorMap.end())
				{
					result += "\t" + item.returnType + " operator " + csOperatorMap.at(item.name) + "()\n\t{\n";
					result += "\t\tMETHOD(" + klass.fullName + ", (";
					for (auto& param : item.parameters)
					{
						result += param.type + ", ";
					}
					result += "MethodInfo*), \"" + item.signature + "\");\n";
					result += "\t\function(this, nullptr);\n";
					result += "\t}\n\n";
				}
				else
				{
					result += "\tstatic " + item.returnType + " op_" + item.name + "(";
					for (auto& param : item.parameters)
					{
						result += param.type + " " + param.name + (param.name != item.parameters.back().name ? ", " : "");
					}
					result += ")\n\t{\n";
					result += "\t\tMETHOD(" + item.returnType + ", (";
					for (auto& param : item.parameters)
					{
						result += param.type + ", ";
					}
					result += "MethodInfo*), \"" + item.signature + "\");\n";
					result += "\t\treturn function(";
					for (auto& param : item.parameters)
					{
						result += param.name + ", ";
					}
					result += "nullptr);\n";
					result += "\t}\n\n";
				}
			}
		}
	}
	if (!klass.methods.empty())
	{
		for (auto& item : klass.methods)
		{
			if (item.name == "BeginInvoke" || item.name == "EndInvoke")
				continue;
			result += "\t// Flags: " + item.flags + "\n";
			result += "\t// Addresss: " + Format("%p", (uintptr_t)(*(void**)item.method)) + "\n";
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
			result += "\t\treturn function(this" + std::string(item.parameters.empty() ? "" : ", ");
			for (auto& param : item.parameters)
			{
				result += param.name + (param.name != item.parameters.back().name ? ", " : "");
			}
			result += ", nullptr);\n";
			result += "\t}\n\n";
		}
	}
	if (result.back() == '\n')
	{
		result.pop_back();
	}
	result += "};\n\n";
	
	if (!klass.klass.nameSpace.empty())
	{
		for (size_t i = 0; i < namespaceList.size(); i++)
		{
			result += "}\n";
		}
	}
	return result;
}

inline std::string GenerateEnumKlassStructure(KlassStructure klass)
{
	std::string result = "";

	std::vector<std::string> namespaceList = GetNamespaceList(klass.klass.nameSpace);

	if (!klass.klass.nameSpace.empty())
	{
		for (auto& item : namespaceList)
		{
			result += "namespace ";
			result += item;
			result += "\n{\n";
		}
	}
	result += "// Name: " + klass.fullName + "\n";
	result += "// Flags: " + klass.flags + "\n";
	result += "enum class " + klass.name + "\n";
	result += "{\n";
	for (auto& item : klass.staticFields)
	{
		result += "\t" + item.name + " = " + item.value + ",\n";
	}
	result += "};\n";
	if (!klass.klass.nameSpace.empty())
	{
		for (size_t i = 0; i < namespaceList.size(); i++)
		{
			result += "}\n";
		}
	}
	return result;
}

inline std::string GenerateKlassStructure(KlassStructure klass)
{
	if (klass.illegality)
		return std::string();
	else if (klass.enumKlass)
		return GenerateEnumKlassStructure(klass);
	return GenerateClassKlassStructure(klass);
}

inline void RunResolver()
{
	char gamePath[MAX_PATH];
	GetModuleFileNameA(NULL, gamePath, MAX_PATH);
	std::filesystem::path path = std::filesystem::path(gamePath).parent_path() / "dumper";
	if (!std::filesystem::exists(path)) std::filesystem::create_directory(path);

	logger.LogInfo("NaDUGR is running!");
	logger.level = NaLoggerLevel_Warning;
	clock_t allStartTime = clock();
	try
	{
		logger.LogInfo("PreAnalysisAssemblyList size: %d", preAnalysisAssemblyList.size());
		{
			size_t assembliesSize = 0;
			const Il2CppAssembly** assemblies = ApiManager.GetAssemblies(Il2CppResolver->domain, &assembliesSize);
			if (assembliesSize <= 0 || assemblies == nullptr)
			{
				throw std::exception("Failed to get assemblies!");
			}
			std::vector<const Il2CppAssembly*> preAnalysisAssemblies;
			for (int i = 0; i < assembliesSize; i++)
			{
				if (preAnalysisAssemblies.size() == preAnalysisAssemblyList.size())
				{
					break;
				}
				const Il2CppAssembly* assembly = assemblies[i];
				if (assembly == nullptr)
					continue;
				std::string assemblyName = GetAssemblyName(assembly);
				for (auto& item : preAnalysisAssemblyList)
				{
					if (item == assemblyName)
					{
						preAnalysisAssemblies.push_back(assembly);
						preAnalysisAssemblyList.erase(std::remove(preAnalysisAssemblyList.begin(), preAnalysisAssemblyList.end(), item));
						break;
					}
				}
			}
			if (!preAnalysisAssemblyList.empty())
			{
				logger.LogWarning("Failed to find assemblies: ");
				for (auto& item : preAnalysisAssemblyList)
				{
					logger.LogWarning("\t%s", item.c_str());
				}
			}
			for (auto& assembly : preAnalysisAssemblies)
			{
				std::string assemblyName = GetAssemblyName(assembly);
				const Il2CppImage* image = ApiManager.GetAssemblyImage(assembly);
				if (image == nullptr)
				{
					throw std::exception(("Failed to get image of assembly: " + assemblyName).c_str());
				}
				size_t klassSize = ApiManager.GetImageClassCount(image);
				if (klassSize <= 0)
				{
					throw std::exception(("Failed to get class count of image: " + assemblyName).c_str());
				}
				for (size_t i = 0; i < klassSize; i++)
				{
					Il2CppClass* klass = (Il2CppClass*)ApiManager.GetImageClass(image, i);
					if (klass == nullptr)
					{
						throw std::exception(("Failed to get class of image: " + assemblyName).c_str());
					}
					std::string name = ApiManager.GetClassName(klass);
					if (name == "<Module>")
						continue;
					std::string nameSpace = ApiManager.GetClassNamespace(klass);
					preAnalysisKlassList.push_back({ assemblyName, nameSpace, name, klass });
				}
			}
		}
		logger.LogInfo("PreAnalysisKlassList size: %d", preAnalysisKlassList.size());
		for (size_t i = 0; i < preAnalysisKlassList.size(); i++)
		{
			PreAnalysisKlass& klass = preAnalysisKlassList[i];
			if (klass.name.find("`") != std::string::npos)
			{
				klass.notSupported = true;
				logger.LogWarning("Sorry, we don't support this type class now: %s", klass.name.c_str());
				continue;
			}
			if (klass.klass == nullptr)
			{
				Il2CppClass* insurance = Il2CppResolver->GetClassEx(klass.assemblyName.c_str(), klass.nameSpace.c_str(), klass.name.c_str());
				if (insurance == nullptr)
				{
					logger.LogWarning("Failed to get class: %s.%s.%s", klass.assemblyName.c_str(), klass.nameSpace.c_str(), klass.name.c_str());
					continue;
				}
				klass.klass = insurance;
			}
			logger.LogInfo("PreAnalysisKlassList : %d", i);
			logger.LogInfo("	name: %s", klass.name.c_str());
			logger.LogInfo("	nameSpace: %s", klass.nameSpace.c_str());
			logger.LogInfo("	assemblyName: %s", klass.assemblyName.c_str());
			logger.LogInfo("	klass: %p", klass.klass);
		}
		logger.LogInfo("Work strat.\n\n");
		std::vector<KlassStructure> klassStructures;
		std::vector<std::string> assemblyNames;
		for (auto& klass : preAnalysisKlassList)
		{
			if (klass.notSupported)
			{
				continue;
			}
			clock_t startTime = clock();
			logger.LogInfo("AnalysisKlass: %s", klass.name.c_str());
			Il2CppClass* il2CppClass = klass.klass;
			if (il2CppClass == nullptr)
			{
				logger.LogWarning("AnalysisKlass: %s failed! \n\n", klass.name.c_str());
				continue;
			}
			klassStructures.push_back(AnalysisKlass(il2CppClass));
			logger.LogInfo("AnalysisKlass: %s success (%dms)!\n\n", klass.name.c_str(), clock() - startTime);
		}
		for (auto& klass : klassStructures)
		{
			if (klass.klass.notSupported)
			{
				continue;
			}
			PretreatmentGenericType(klass);
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

			if (!klass.klass.name.empty())
			{
				std::filesystem::path assemblyPath = path / klass.klass.assemblyName;
				if (!std::filesystem::exists(assemblyPath)) std::filesystem::create_directory(assemblyPath);
				if (klass.klass.nameSpace.empty()) klass.klass.nameSpace = "__NO_NAMESPACE__";
				std::filesystem::path namespacePath = assemblyPath / klass.klass.nameSpace;
				if (!std::filesystem::exists(namespacePath)) std::filesystem::create_directory(namespacePath);
				std::filesystem::path klassPath = namespacePath / (klass.klass.name + ".h");
				if (std::filesystem::exists(klassPath)) std::filesystem::remove(klassPath);
				std::ofstream file = std::ofstream(klassPath, std::ios::out);
				file << klassStructure;
				file.close();
				if (klass.klass.nameSpace.compare("__NO_NAMESPACE__") == 0)
				{
					klass.klass.nameSpace = "";
				}
				if (std::find(assemblyNames.begin(), assemblyNames.end(), klass.klass.assemblyName) == assemblyNames.end())
					assemblyNames.push_back(klass.klass.assemblyName);
			}
		}
	}
	catch (const std::exception& e)
	{
		logger.level = NaLoggerLevel_All;
		logger.LogFatal("Exception in work : %s", e.what());
	}
	logger.level = NaLoggerLevel_All;
	logger.LogInfo("Work end (%dms)!", clock() - allStartTime);
	logger.LogInfo("NaDUGR is done!");
}
