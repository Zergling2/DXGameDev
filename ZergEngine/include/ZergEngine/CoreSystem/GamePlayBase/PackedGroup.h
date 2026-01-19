#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\GroupSlot.h>
#include <vector>
#include <cassert>

namespace ze
{
    template <typename T, typename SlotPolicy>
    class PackedGroup
    {
    public:
        static constexpr INVALID_INDEX = (std::numeric_limits<gsi_type>::max)();
    public:
        void Add(T* pItem)
        {
            const gsi_type index = SlotPolicy::GetIndex(pItem);
            assert(index == INVALID_INDEX);

            m_items.push_back(pItem);
            SlotPolicy::SetIndex(pItem, static_cast<gsi_type>(m_items.size() - 1));
        }

        void Remove(T* pItem)
        {
            const gsi_type index = SlotPolicy::GetIndex(pItem);
            assert(index != INVALID_INDEX);
            assert(m_items[index] == pItem);

            gsi_type endIndex = static_cast<gsi_type>(m_items.size() - 1);

            if (index != endIndex)
            {
                T* pEndItem = m_items[endIndex];
                const gsi_type endItemIndex = SlotPolicy::GetIndex(pEndItem);

                std::swap(m_items[index], m_items[endIndex]);
                SlotPolicy::SetIndex(pEndItem, index);
            }

            SlotPolicy::SetIndex(pItem, INVALID_INDEX);
            m_items.pop_back();
        }

        std::vector<T*>& GetItems() { return m_items; }
        const std::vector<T*>& GetItems() const { return m_items; }
    private:
        std::vector<T*> m_items;
    };
}
