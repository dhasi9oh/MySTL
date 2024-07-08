#ifndef MYSTL_TYPE_TRAITS_H
#define MYSTL_TYPE_TRAITS_H

#include <type_traits>

//这个文件就是萃取器类

namespace mystl
{
	// 这个类主要是用来将变量萃取成常量的
	template <class T, T v>
	class m_integral_constant
	{
	public:
		// constexpr 这个关键字是让变量
		// 在编译期间可以被读取成常量
		static constexpr T value = v;

	};

	// 声明一个在编译期间可以被读取的bool常量类
	template <bool b>
	using m_bool_constant = m_integral_constant<bool, b>;

	typedef m_bool_constant<true> m_true_tpye;
	typedef m_bool_constant<false> m_false_tpye;

	// 在这里去声明pair类
	template <class T1, class T2>
	class pair;

	// is_类都是用来确定是不是该类型的
	// 因为我可以从里面调一个基类的 bool 出来看是否为true
	template <class T>
	class is_pair : public m_false_tpye {};

	template <class T1, class T2>
	class is_pair<mystl::pair<T1, T2>> : public m_true_tpye {};
}

#endif