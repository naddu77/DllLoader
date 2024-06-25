#pragma once

#include "NamedArgument.h"
#include "Mpl.h"

#include <type_traits>
#include <filesystem>
#include <cassert>

#include <wil/resource.h>

template <typename T>
concept FuntionPointerConcept = std::is_pointer_v<T> and std::is_function_v<std::remove_pointer_t<T>>;

template <typename T>
concept NamedArgumentConcept = requires (T t) {
	T::name;
	typename T::ValueType;
	t.value;
} and FuntionPointerConcept<typename T::ValueType>;

template <typename... NamedArguments>
requires (NamedArgumentConcept<NamedArguments> and ...)
class DllLoader
	: public NamedArguments...
{
public:
	DllLoader(std::filesystem::path const& dll_path)
		: module_handle{ LoadLibraryW(dll_path.c_str()) }
	{

	}

	~DllLoader() = default;

	bool Valid() const
	{
		return module_handle.is_valid();
	}

	template <StringLiteral function_name, typename... Ts>
	decltype(auto) Invoke(Ts&&... ts)
	{
		return std::invoke(Lookup<function_name>(), std::forward<Ts>(ts)...);
	}

	template <StringLiteral function_name>
	auto Lookup() 
	{
		using Arg = MapLookup<DllLoader, function_name, void, NamedArgument>;

		static_assert(not std::is_same_v<Arg, void>, "Could not find the function.");

		auto& fn{ static_cast<Arg&>(*this).value };

		if (fn == nullptr)
		{
			fn = reinterpret_cast<typename Arg::ValueType>(GetProcAddress(module_handle.get(), function_name.buf));
		}

		assert(fn);

		return fn;
	}

private:
	wil::unique_hmodule module_handle;
};
