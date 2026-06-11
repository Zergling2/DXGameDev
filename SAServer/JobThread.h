#pragma once

class IJobThread
{
public:
	IJobThread() = default;
	virtual ~IJobThread() = default;

	virtual bool Start() = 0;
	virtual void Shutdown() = 0;
};
