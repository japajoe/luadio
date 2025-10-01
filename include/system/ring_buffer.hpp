#include <vector>

namespace luadio
{
	template<typename T>
    class ring_buffer
    {
	public:
		ring_buffer() : ring_buffer(100) {}

		ring_buffer(int maxMessages) 
			: maxSize(maxMessages), startIndex(0), endIndex(0), itemCount(0) 
		{
			items.resize(maxSize);
		}

		int count() const 
		{
			return itemCount;
		}

		int size() const 
		{
			return maxSize;
		}

        void add(const T &item)
        {
            items[endIndex] = item;
            endIndex = (endIndex + 1) % maxSize;

            if (itemCount < maxSize)
            {
                itemCount++;
            }
            else
            {
                startIndex = (startIndex + 1) % maxSize;
            }
        }

        T get_at(int index) const
        {
            if (index >= 0 && index < itemCount)
            {
                int idx = (startIndex + index) % maxSize;
                return items[idx];
            }
            throw std::out_of_range("Index out of range");
        }

		void resize(size_t size)
		{
			items.resize(size);
			maxSize = size;
			clear();
		}

        void clear()
        {
            startIndex = 0;
            endIndex = 0;
            itemCount = 0;
        }
	private:
        std::vector<T> items;
        int maxSize;
        int startIndex;
        int endIndex;
        int itemCount;
    };
}