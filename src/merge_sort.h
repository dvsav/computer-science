#include <algorithm>
#include <vector>

namespace sc
{
	template <typename T>
	static std::vector<T> merge(const std::vector<T>& a, const std::vector<T>& b)
	{
		std::vector<T> result;
		result.reserve(a.size() + b.size());

		size_t a_i = 0;
		size_t b_i = 0;

		while (a_i < a.size() && b_i < b.size())
		{
			if (a[a_i] < b[b_i])
				result.push_back(a[a_i++]);
			else
				result.push_back(b[b_i++]);
		}

		while (a_i < a.size())
			result.push_back(a[a_i++]);

		while (b_i < b.size())
			result.push_back(b[b_i++]);

		return result;
	}

	template <typename TIterator>
	static std::vector<typename TIterator::value_type> merge_sort_internal(TIterator begin, TIterator end)
	{
		auto size = end - begin;

		if (size == 0)
			return std::vector<typename TIterator::value_type>();
		else if (size == 1)
			return std::vector<typename TIterator::value_type>(1, *begin);

		auto middle = begin + size / 2;

		auto left_part = merge_sort_internal(begin, middle);
		auto right_part = merge_sort_internal(middle, end);

		return merge(left_part, right_part);
	}

	template <typename TIterator>
	void merge_sort(TIterator begin, TIterator end)
	{
		auto result = merge_sort_internal(begin, end);
		std::copy(result.begin(), result.end(), begin);
	}
}
