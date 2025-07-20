#pragma once

#include "framework.h"

// CLogListView view

enum class LOG_TYPE
{
	LT_INFO,
	LT_WARNING,
	LT_ERROR
};

class CLogListView : public CListView
{
	DECLARE_DYNCREATE(CLogListView)
protected:
	CLogListView();           // protected constructor used by dynamic creation
	virtual ~CLogListView();
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	DECLARE_MESSAGE_MAP()
private:
	bool m_initialized;
public:
	void AddLog(const CString& msg, LOG_TYPE type) { this->AddLog(msg.GetString(), type); }
	void AddLog(LPTSTR msg, LOG_TYPE type);

	virtual void OnInitialUpdate();
};
