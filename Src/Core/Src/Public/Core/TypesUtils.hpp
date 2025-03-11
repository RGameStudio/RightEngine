#pragma once

#include <type_traits>

namespace core
{

// Empty structure. Useful as a placeholder in template code.
//--------------------------------------------------------------------------------------------------------------------
struct EmptyStruct
{
	template<typename... Args> EmptyStruct(Args&&...) {}
};

// Uninitialized storage suitable for containing single instance of a type.
//--------------------------------------------------------------------------------------------------------------------
template<typename T>
using UninitStorage = std::aligned_storage_t<sizeof(T), alignof(T)>;

// Template variable that is always false.
// Useful for static_asserts in else branch of constexpr-if, to make it dependent on template argument.
//--------------------------------------------------------------------------------------------------------------------
template<typename T> inline constexpr bool AlwaysFalse = false;

// Check whether given type is a specialization of a given template.
// Example of usage:
//	eastl::vector<float> is specialization of eastl::vector<T, A>.
//	static_assert(IsSpecializationOfV<eastl::vector, eastl::vector<float>>);
//	eastl::vector<float> is not specialization of std::pair<T1, T2>.
//	static_assert(!IsSpecializationOfV<std::pair, eastl::vector<float>>);
//--------------------------------------------------------------------------------------------------------------------
template<template<typename...> typename Template, typename Type> struct IsSpecializationOf : std::false_type {};
template<template<typename...> typename Template, typename... Args> struct IsSpecializationOf<Template, Template<Args...>> : std::true_type {};
template<template<typename...> typename Template, typename Type> constexpr auto IsSpecializationOfV = IsSpecializationOf<Template, Type>::value;

// Check whether one class (Derived) is derived from template class (Base).
// Example of usage:
//	template<typename T>
//	struct BaseClass {};
//
//	struct TestClass : BaseClass<int> {};
//
//	// TestClass is derived from template class BaseClass<T>.
//	static_assert(IsTemplateBaseOfV<BaseClass, TestClass>);
//--------------------------------------------------------------------------------------------------------------------
template <template <typename...> class Base, typename Derived>
struct IsTemplateBaseOf
{
	template<typename... Ts>
	static constexpr std::true_type  test(const Base<Ts...>*);
	static constexpr std::false_type test(...);

	using type = decltype(test(std::declval<Derived*>()));
	static constexpr bool value = type::value;
};

template <template <typename...> class Base, typename Derived>
inline constexpr bool IsTemplateBaseOfV = IsTemplateBaseOf<Base, Derived>::value;

// Functor that extracts specified member from a structure.
// Useful for associative containers (e.g. generalized hashmaps) that embed key into stored structure.
// Inspired by eastl::use_first/use_second; use eastl::use_self for identity mapping (e.g. hash-set-like containers).
//--------------------------------------------------------------------------------------------------------------------
template<auto Field> struct UseField
{
	static_assert(std::is_member_object_pointer_v<decltype(Field)>);
	template<typename T> const auto& operator()(const T& x) const { return x.*Field; }
};

// Shortcut to remove cv-ref qualifications. To be replaced with remove_cvref after we switch to C++20.
//--------------------------------------------------------------------------------------------------------------------
template<typename T> struct RemoveCVRef { using type = std::remove_cv_t<std::remove_reference_t<T>>; };
template<typename T> using RemoveCVRefT = typename RemoveCVRef<T>::type;

// Utility for creating overload set with lambdas. Useful e.g. for std::variant::visit:
// std::variant<int, float, string> var;
// std::visit(Overloaded {
//     [](int i) { /* called if var contains int */ },
//     [](const string& s) { /* called if var contains string */ },
//     [](const auto& v) { /* called in all other cases */ }
// }, var);
template<typename... Ts> struct Overloaded : Ts... {};
template<typename... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

// Simple type list and utilities for manipulation.
//--------------------------------------------------------------------------------------------------------------------
template<typename... Ts> struct TypeList {};

// get size of type list
template<typename TL> struct TypeListSize;
template<typename... Ts> struct TypeListSize<TypeList<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};
template<typename TL> inline constexpr size_t TypeListSizeV = TypeListSize<TL>::value;

// get element by index; returns void if index is out of range
template<typename TL, size_t Index> struct TypeListElement;
template<typename T0, typename... Ts> struct TypeListElement<TypeList<T0, Ts...>, 0> { using type = T0; };
template<typename T0, typename... Ts, size_t Index> struct TypeListElement<TypeList<T0, Ts...>, Index> : TypeListElement<TypeList<Ts...>, Index - 1> {};
template<size_t Index> struct TypeListElement<TypeList<>, Index> { using type = void; };
template<typename TL, size_t Index> using TypeListElementT = typename TypeListElement<TL, Index>::type;

// internal utilities to find type in type list
namespace detail
{
template<typename T, size_t Skip, typename... Ts> struct TypeListIndexOfPack;
template<typename T, size_t Skip, typename... Ts> struct TypeListIndexOfPack<T, Skip, T, Ts...> : std::integral_constant<size_t, Skip> {};
template<typename T, size_t Skip, typename T0, typename... Ts> struct TypeListIndexOfPack<T, Skip, T0, Ts...> : TypeListIndexOfPack<T, Skip + 1, Ts...> {};
template<typename T, size_t Skip> struct TypeListIndexOfPack<T, Skip> : std::integral_constant<size_t, Skip> {};

template<typename TL, typename T> struct TypeListIndexOfImpl;
template<typename... Ts, typename T> struct TypeListIndexOfImpl<TypeList<Ts...>, T> : TypeListIndexOfPack<T, 0, Ts...> {};
} // detail

// get index of type
template<typename TL, typename T> struct TypeListIndexOf
{
	static constexpr size_t value = detail::TypeListIndexOfImpl<TL, T>::value;
	static_assert(value < TypeListSizeV<TL>, "Type not found");
};
template<typename TL, typename T> inline constexpr size_t TypeListIndexOfV = TypeListIndexOf<TL, T>::value;

// check whether type is in type list
template<typename TL, typename T> struct TypeListContains : std::bool_constant<detail::TypeListIndexOfImpl<TL, T>::value != TypeListSizeV<TL>> {};
template<typename TL, typename T> inline constexpr bool TypeListContainsV = TypeListContains<TL, T>::value;

// append types to the type list
template<typename TL, typename... Ts> struct TypeListAppend;
template<typename... Old, typename... Ts> struct TypeListAppend<TypeList<Old...>, Ts...> { using type = TypeList<Old..., Ts...>; };
template<typename TL, typename... Ts> using TypeListAppendT = typename TypeListAppend<TL, Ts...>::type;

// merge type lists
template<typename... TLs> struct TypeListMerge;
template<> struct TypeListMerge<> { using type = TypeList<>; };
template<typename... Ts> struct TypeListMerge<TypeList<Ts...>> { using type = TypeList<Ts...>; };
//--------------------------------------------------------------------------------------------------------------------
template<typename... Ts, typename... Us, typename... Rest>
struct TypeListMerge<TypeList<Ts...>, TypeList<Us...>, Rest...>
{
	using type = typename TypeListMerge<TypeList<Ts..., Us...>, Rest...>::type;
};
template<typename... TLs> using TypeListMergeT = typename TypeListMerge<TLs...>::type;

// transform type list by applying metafunction to each element
template<typename TL, template<typename...> typename F> struct TypeListMap;
template<typename... Ts, template<typename...> typename F> struct TypeListMap<TypeList<Ts...>, F> { using type = TypeList<F<Ts>...>; };
template<typename TL, template<typename...> typename F> using TypeListMapT = typename TypeListMap<TL, F>::type;

// pass all elements of type list to variadic template
template<typename TL, template<typename...> typename F> struct TypeListReduce;
template<typename... Ts, template<typename...> typename F> struct TypeListReduce<TypeList<Ts...>, F> { using type = F<Ts...>; };
template<typename TL, template<typename...> typename F> using TypeListReduceT = typename TypeListReduce<TL, F>::type;

// filter type list: return type list of elements for which F<T>::value == true
namespace detail
{
template<typename TL, template<typename> typename F, typename... Ts> struct TypeListFilterImpl;
template<typename TL, template<typename> typename F, typename T0, typename... Ts> struct TypeListFilterImpl<TL, F, T0, Ts...>
{
	using Next = std::conditional_t<F<T0>::value, TypeListAppendT<TL, T0>, TL>;
	using type = typename TypeListFilterImpl<Next, F, Ts...>::type;
};
template<typename TL, template<typename> typename F> struct TypeListFilterImpl<TL, F> { using type = TL; };
} // detail

template<typename TL, template<typename> typename F> struct TypeListFilter;
template<typename... Ts, template<typename> typename F> struct TypeListFilter<TypeList<Ts...>, F> { using type = typename detail::TypeListFilterImpl<TypeList<>, F, Ts...>::type; };
template<typename TL, template<typename> typename F> using TypeListFilterT = typename TypeListFilter<TL, F>::type;

// Inspect types of a function signature.
//--------------------------------------------------------------------------------------------------------------------
template<typename T, typename = void> struct FunctionTraits {};
template<typename T> struct FunctionTraits<T, std::void_t<decltype(&T::operator())>> : FunctionTraits<decltype(&T::operator())> {};
template<typename R, typename... Args> struct FunctionTraits<R(Args...)>
{
	using ReturnType = R;
	using ClassType = void;
	using ArgTypes = TypeList<Args...>;

	static constexpr bool Const = false;

	static constexpr size_t ArgCount = sizeof...(Args);
	template<size_t Idx> using ArgType = TypeListElementT<ArgTypes, Idx>;
};
template<typename R, typename... Args> struct FunctionTraits<R(*)(Args...)> : FunctionTraits<R(Args...)> {};
template<typename R, typename... Args> struct FunctionTraits<R(&)(Args...)> : FunctionTraits<R(Args...)> {};
template<typename R, typename C, typename... Args> struct FunctionTraits<R(C::*)(Args...)> : FunctionTraits<R(Args...)> { using ClassType = C; };
template<typename R, typename C, typename... Args> struct FunctionTraits<R(C::*)(Args...) const> : FunctionTraits<R(Args...)> { using ClassType = C; static constexpr bool Const = true; };
template<typename R, typename C, typename... Args> struct FunctionTraits<R(C::*)(Args...) volatile> : FunctionTraits<R(Args...)> { using ClassType = C; };
template<typename R, typename C, typename... Args> struct FunctionTraits<R(C::*)(Args...) const volatile> : FunctionTraits<R(Args...)> { using ClassType = C; static constexpr bool Const = true; };
template<typename R, typename... Args> struct FunctionTraits<R(*)(Args...) noexcept> : FunctionTraits<R(Args...)> {};
template<typename R, typename... Args> struct FunctionTraits<R(&)(Args...) noexcept> : FunctionTraits<R(Args...)> {};
template<typename R, typename C, typename... Args> struct FunctionTraits<R(C::*)(Args...) noexcept> : FunctionTraits<R(Args...)> { using ClassType = C; };
template<typename R, typename C, typename... Args> struct FunctionTraits<R(C::*)(Args...) const noexcept> : FunctionTraits<R(Args...)> { using ClassType = C; static constexpr bool Const = true; };
template<typename R, typename C, typename... Args> struct FunctionTraits<R(C::*)(Args...) volatile noexcept> : FunctionTraits<R(Args...)> { using ClassType = C; };
template<typename R, typename C, typename... Args> struct FunctionTraits<R(C::*)(Args...) const volatile noexcept> : FunctionTraits<R(Args...)> { using ClassType = C; static constexpr bool Const = true; };
template<typename T> struct FunctionTraits<std::function<T>> : FunctionTraits<T> {};

//--------------------------------------------------------------------------------------------------------------------
template<typename T, typename = void> struct IsFunction : std::false_type {};
template<typename T> struct IsFunction<T, std::void_t<typename FunctionTraits<T>::ReturnType>> : std::true_type {};
template<typename T> inline constexpr bool IsFunctionV = IsFunction<T>::value;

// Select version of overloaded function
//--------------------------------------------------------------------------------------------------------------------
template<bool> struct ConstT {};
static constexpr auto Const = ConstT<true>{};
template<typename... Args> struct Overload {
	template<typename R>
	constexpr auto operator()(R(*ptr)(Args...)) const noexcept -> decltype(ptr) { return ptr; }

	template<typename R, typename C>
	constexpr auto operator()(R(C::*ptr)(Args...), ConstT<false> = {}) const noexcept -> decltype(ptr) { return ptr; }

	template<typename R, typename C>
	constexpr auto operator()(R(C::*ptr)(Args...) const, ConstT<true>) const noexcept -> decltype(ptr) { return ptr; }
};
template<typename... Args> inline constexpr auto OverloadCast = Overload<Args...>{};

// Determine if string is built in constant
//--------------------------------------------------------------------------------------------------------------------
template<typename T> struct IsBuiltinString { static constexpr bool test(const T&) { return false; } };
template<size_t len> struct IsBuiltinString<const char(&)[len]> { static constexpr bool test(const char(&)[len]) { return true; } };
template<size_t len> struct IsBuiltinString<const wchar_t(&)[len]> { static constexpr bool test(const wchar_t(&)[len]) { return true; } };

// Utility for defining "forwarding constructors" (constructors that accept Args&&...) in such a way that they don't get selected as candidates for copy & move.
// It's actually more generic (checks whether pack is something other than a single type with arbitrary cv-ref qualifications), but I can't think of a good name.
//--------------------------------------------------------------------------------------------------------------------
template<typename T, typename... Types> struct IsForwardConstructorPack : std::true_type {}; // default implementation is selected for 0 or 2+ arg packs
template<typename T, typename U> struct IsForwardConstructorPack<T, U> : std::negation<std::is_same<T, RemoveCVRefT<U>>> {};
template<typename T, typename... Types> inline constexpr bool IsForwardConstructorPackV = IsForwardConstructorPack<T, Types...>::value;

// Utitily to check whether or not type has nested types of container elements or keys
//--------------------------------------------------------------------------------------------------------------------
template<class T, typename = void> struct GetKeyType { using type = void; };
template<class T, typename = void> struct GetValueType { using type = void; };
template<class T, typename = void> struct HasIterator : std::false_type { };
template<class T> struct GetKeyType<T, std::void_t<typename T::key_type>> { using type = typename T::key_type; };
template<class T> struct GetValueType<T, std::void_t<typename T::value_type>> { using type = typename T::value_type; };
template<class T> struct HasIterator<T, std::void_t<typename T::iterator>> : std::true_type { };
template<class T> constexpr bool IsContainerType = HasIterator<T>::value && !std::is_same_v<T, std::string>;

// Helper that returns signed version of unsigned type or the same type if the type is already signed type
// The helper is needed as std::make_signed<T> does not work for floating types
//--------------------------------------------------------------------------------------------------------------------
namespace detail
{
template <size_t> struct MakeSigned {};
template <> struct MakeSigned<1> { using Type = int8_t; };
template <> struct MakeSigned<2> { using Type = int16_t; };
template <> struct MakeSigned<4> { using Type = int32_t; };
template <> struct MakeSigned<8> { using Type = int64_t; };
} // detail
template<typename T> struct MakeSigned
{
	static_assert(!std::is_same_v<T, bool> && !std::is_enum_v<T>, "MakeSigned<T> requires that T shall be an integral type or floating point type but not a bool type or enumeration" );
	using Type = typename detail::MakeSigned<sizeof(T)>::Type;
};
template<> struct MakeSigned<float> { using Type = float; };
template<> struct MakeSigned<double> { using Type = double; };
template<typename T> using MakeSignedT = typename MakeSigned<T>::Type;

// Determine if given type T is pointer or pointer-like object
//--------------------------------------------------------------------------------------------------------------------
namespace detail
{
template<typename T> struct HasOperatorStarImpl
{
	template<typename U> static auto test(int) -> decltype(*std::declval<U>(), std::true_type{});
	template<typename> static auto test(...) -> std::false_type;
	using Type = decltype(test<T>(0));
};
template<typename T> struct HasOperatorStar : HasOperatorStarImpl<T>::Type {};

template<typename T> struct HasOperatorArrowImpl
{
	template<typename U> static auto test(int) -> decltype(std::declval<U>().operator->(), std::true_type{});
	template<typename> static auto test(...) -> std::false_type;
	using Type = decltype(test<T>(0));
};
template<typename T> struct HasOperatorArrow : HasOperatorArrowImpl<T>::Type {};


template<typename From, typename To>
struct IsExplicitlyConvertibleImpl
{
	template<typename F, typename T> static auto test(int) -> decltype(static_cast<T>(std::declval<F>()), std::true_type{});
	template<typename, typename> static auto test(...) -> std::false_type;
	using Type = decltype(test<From, To>(0));
};
template<typename From, typename To> struct IsExplicitlyConvertible : IsExplicitlyConvertibleImpl<From, To>::Type {};
} // detail

template<typename T> struct IsPointerLikeObject : std::integral_constant<bool, (detail::HasOperatorStar<T>::value && detail::HasOperatorArrow<T>::value && detail::IsExplicitlyConvertible<T, bool>::value) || IsTemplateBaseOfV<std::weak_ptr, T>> {};
template<typename T> struct IsPointerLike : std::integral_constant<bool, !std::is_member_pointer_v<T> && (std::is_pointer_v<T> || IsPointerLikeObject<T>::value)> {};
template<typename T> inline constexpr bool IsPointerLikeV = IsPointerLike<T>::value;

} // core
