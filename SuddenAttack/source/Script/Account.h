#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Constants.h"

class Account : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	Account(ze::GameObject& owner);
	virtual ~Account() = default;

	uint16_t GetLevel() const { return m_level; }
	void SetLevel(uint16_t level) { m_level = level; }
	uint32_t GetExp() const { return m_exp; }
	void SetExp(uint32_t exp) { m_exp = exp; }
	uint32_t GetPoint() const { return m_point; }
	void SetPoint(uint32_t point) { m_point = point; }
	const wchar_t* GetNickname() const { return m_nickname; }
	void SetNickname(const wchar_t* nickname) { wcscpy_s(m_nickname, nickname); }
private:
	uint16_t m_level;	// 레벨
	uint32_t m_exp;		// 경험치
	uint32_t m_point;	// 포인트 소유량
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];	// null termination 상태로 저장
};
