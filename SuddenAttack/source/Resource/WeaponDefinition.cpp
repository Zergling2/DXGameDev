#include "WeaponDefinition.h"
#include <algorithm>

void WeaponEventTable::AddEvent(float time, WeaponEvent event)
{
	m_events.push_back(std::make_pair(time, event));

	std::sort(
		m_events.begin(),
		m_events.end(),
		[](const std::pair<float, WeaponEvent>& a, const std::pair<float, WeaponEvent>& b)
		{
			return a.first < b.first;
		}
	);
}
