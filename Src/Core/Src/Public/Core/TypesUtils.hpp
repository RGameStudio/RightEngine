#pragma once

namespace core
{

template<class T, typename = void> struct GetKeyType { using type = void; };
template<class T, typename = void> struct GetValueType { using type = void; };
template<class T, typename = void> struct HasIterator : std::false_type { };
template<class T> struct GetKeyType<T, std::void_t<typename T::key_type>> { using type = typename T::key_type; };
template<class T> struct GetValueType<T, std::void_t<typename T::value_type>> { using type = typename T::value_type; };
template<class T> struct HasIterator<T, std::void_t<typename T::iterator>> : std::true_type { };
template<class T> constexpr bool IsContainerType = HasIterator<T>::value && !std::is_same_v<T, std::string>;

} // core