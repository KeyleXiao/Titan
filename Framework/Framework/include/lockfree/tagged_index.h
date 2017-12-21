#pragma once

namespace speed
{
	typedef unsigned int index_t;

	// 加tag的指针或者数组下标(为了避免cas时 ABA问题，tag + index组装成同一个对象作为cas输入)
	class tagged_index
	{
	public:
		tagged_index() : index(0), tag(0)
		{

		}

		tagged_index(index_t index_, index_t tag_) : index(index_), tag(tag_)
		{
		}

		void set_index(index_t i)
		{
			index = i;
		}

		index_t get_index(void) const
		{
			return index;
		}

		void set_tag(index_t t)
		{
			tag = t;
		}

		index_t get_tag(void) const
		{
			return tag;
		}

		bool operator==(const tagged_index& rhs)
		{
			return index == rhs.index && tag == rhs.tag;
		}

		bool operator != (const tagged_index& rhs)
		{
			return index != rhs.index || tag != rhs.tag;
		}

	private:
		index_t tag;
		index_t index;
	};
}