#ifndef MYSTL_TYPE_TRAITS_H
#define MYSTL_TYPE_TRAITS_H

#include <type_traits>

//����ļ�������ȡ����

namespace mystl
{
	// �������Ҫ��������������ȡ�ɳ�����
	template <class T, T v>
	class m_integral_constant
	{
	public:
		// constexpr ����ؼ������ñ���
		// �ڱ����ڼ���Ա���ȡ�ɳ���
		static constexpr T value = v;

	};

	// ����һ���ڱ����ڼ���Ա���ȡ��bool������
	template <bool b>
	using m_bool_constant = m_integral_constant<bool, b>;

	typedef m_bool_constant<true> m_true_tpye;
	typedef m_bool_constant<false> m_false_tpye;

	// ������ȥ����pair��
	template <class T1, class T2>
	class pair;

	// is_�඼������ȷ���ǲ��Ǹ����͵�
	// ��Ϊ�ҿ��Դ������һ������� bool �������Ƿ�Ϊtrue
	template <class T>
	class is_pair : public m_false_tpye {};

	template <class T1, class T2>
	class is_pair<mystl::pair<T1, T2>> : public m_true_tpye {};
}

#endif