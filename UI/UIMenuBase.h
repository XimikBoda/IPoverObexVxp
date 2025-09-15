#pragma once
#include "UIBase.h"
#include <cstdint>
#include <cstddef>

struct UIMenuElement {
	UIBase* el;
	uint8_t up, down, left, right;
};

class UIMenuBase : public UIBase
{
public:
	virtual void KeyboardEvent(int event, int keycode) override;
protected:
	int cur_element = 0;
	const UIMenuElement* uiarray_internal = 0;
	size_t uiarray_size_internal = 0;

	void MoveFocus(int new_id);

	virtual void LeftSoftkeyClick() {};
	virtual void RightSoftkeyClick() {};
};