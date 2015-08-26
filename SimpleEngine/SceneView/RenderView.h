#pragma once

class RenderView
{
public:
	RenderView();
	virtual ~RenderView();


	virtual void Update( float dt );

protected:
	// draw objects list
	//vector<DrawElement*>	m_vDrawElements;
};

