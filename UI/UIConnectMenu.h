#pragma once
#include "UIBase.h"
#include "UIMenuBase.h"
#include "UIButton.h"
#include "UITextBox.h"
#include "UIComboBox.h"
#include <string>

class UIConnectMenu : public UIMenuBase
{
	char mac[51] = "";
	UITextBox macImput = UITextBox(this, 5, 0, 200, 14, mac, 50);
	UIComboBox pairedDevices = UIComboBox(this, 5, 0, 200, 14, 0,
		(GetListLen)&UIConnectMenu::PairedGetLen,
		(GetListElement)&UIConnectMenu::PairedGetElement);

	UIButton connectButton = UIButton(this, 5, 0, 50, 14, "Connect", (ActOfClick)&UIConnectMenu::ClickConnect);

	UIMenuElement uiarray[3] =
	{
		{(UIBase*)&(UIConnectMenu::macImput),		2, 1, 0, 0}, //0
		{(UIBase*)&(UIConnectMenu::pairedDevices),	0, 2, 1, 1}, //1
		{(UIBase*)&(UIConnectMenu::connectButton),	1, 0, 2, 2}, //2
	};

public:
	UIConnectMenu() {
		uiarray_internal = uiarray;
		uiarray_size_internal = sizeof(uiarray) / sizeof(uiarray[0]);
	};

	void Draw(unsigned short* buf) override;

private:
	void RightSoftkeyClick() override;

	void ClickConnect();

	int PairedGetLen();
	const char* PairedGetElement(int id);
};

