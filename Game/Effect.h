#pragma once

#include "GameObj.h"

class Effect :public GameObj
{
	int endframe;

private:
	void init();

public:
	Effect( const std::string texture, const D3DXVECTOR3 pos, const float frame_rate, const int start_frame = 0, const int end_frame = -1);
	virtual int Update() override;

};