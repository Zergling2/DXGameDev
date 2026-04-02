#pragma once

class GameChannel;

class IChannelJob
{
public:
	IChannelJob() = default;
	virtual ~IChannelJob() = default;

	virtual void Execute(GameChannel& channel) = 0;
};
