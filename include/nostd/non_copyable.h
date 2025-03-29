#ifndef __NOSTD_NON_COPYABLE_H__
#define __NOSTD_NON_COPYABLE_H__

namespace nostd {

	/**
	 * @brief      This class describes a non copyable.
	 */
	class non_copyable
	{
	public:
		non_copyable() = default;
		virtual ~non_copyable() = default;

	private:
		non_copyable(const non_copyable&) = delete;
		non_copyable& operator =(const non_copyable&) = delete;
		non_copyable& operator =(non_copyable&&) = delete;
	};

} // namespace nostd

#endif