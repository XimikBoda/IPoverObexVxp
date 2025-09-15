#include "UIMenuBase.h"
#include "UIEngine.h"
#include "vmio.h"

extern UIEngine uiengine;

void UIMenuBase::KeyboardEvent(int event, int keycode)
{
	switch (event) {
	case VM_KEY_EVENT_UP:
		switch (keycode) {
		case VM_KEY_UP:
			MoveFocus(uiarray_internal[cur_element].up);
			break;
		case VM_KEY_DOWN:
			MoveFocus(uiarray_internal[cur_element].down);
			break;
		case VM_KEY_LEFT:
			MoveFocus(uiarray_internal[cur_element].left);
			break;
		case VM_KEY_RIGHT:
			MoveFocus(uiarray_internal[cur_element].right);
			break;
		case VM_KEY_LEFT_SOFTKEY:
			LeftSoftkeyClick();
			break;
		case VM_KEY_RIGHT_SOFTKEY:
			RightSoftkeyClick();
			break;

		case VM_KEY_OK:
			if (uiarray_internal[cur_element].el)
				uiarray_internal[cur_element].el->OKClick();
			break;
		}
		break;
	}
}

void UIMenuBase::MoveFocus(int new_id)
{
	if (new_id > uiarray_size_internal || cur_element == new_id)
		return;

	if (uiarray_internal[cur_element].el)
		uiarray_internal[cur_element].el->isFocused = 0;

	cur_element = new_id;

	if (uiarray_internal[cur_element].el)
		uiarray_internal[cur_element].el->isFocused = 1;
}