#pragma once
#include"united.h"

struct CitizenData
{
	Font	textBoxFont = Font(16);
	TextBox	nameTextBox;
	TextBox	wageTextBox;
	TextBox	needEnergyTextBox;
	TextBox productNameTextBox;
	TextBox numProductTextBox;

	CitizenData(JSONValue _j)
		: nameTextBox(textBoxFont, Vec2(288, 24), 120, unspecified, _j[U"Name"].getString())
		, wageTextBox(textBoxFont, Vec2(288, 24 * 3), 120, unspecified, Format(_j[U"Wage"].getOr<int>(0)))
		, needEnergyTextBox(textBoxFont, Vec2(288, 24 * 5), 120, unspecified, _j[U"NeedEnergy"].getString())
		, productNameTextBox(textBoxFont, Vec2(288, 24 * 7), 120, unspecified, _j[U"Product"][U"ItemName"].getOr<String>(U""))
		, numProductTextBox(textBoxFont, Vec2(288, 24 * 9), 120, unspecified, Format(_j[U"Product"][U"NumItem"].getOr<int>(0)))
	{}
	CitizenData()
		: nameTextBox(textBoxFont, Vec2(288, 24), 120, unspecified, U"newCitizen")
		, wageTextBox(textBoxFont, Vec2(288, 24 * 3), 120, unspecified, U"0")
		, needEnergyTextBox(textBoxFont, Vec2(288, 24 * 5), 120, unspecified, U"")
		, productNameTextBox(textBoxFont, Vec2(288, 24 * 7), 120, unspecified, U"")
		, numProductTextBox(textBoxFont, Vec2(288, 24 * 9), 120, unspecified, U"0")
	{}
	String	name() const { return nameTextBox.getText(); }
};

Array<CitizenData>	citizenData;

struct SetCitizenData : MyApp::Scene
{
	Font	font16 = Font(16);


	SetCitizenData(const InitData& init)
		: IScene(init)
	{}

	void update() override
	{
		//Άj[
		for (int i = 0; i < int(itemList.size()); i++)
		{
			Rect rect(0, i * 24, 24 * 6, 24);
			if (rect.mouseOver()) rect.draw(Palette::Orange);
			if (i == 1) rect.draw(Palette::Red);
			if (rect.leftClicked())
			{
				getData().selectedSceneID = i;
				changeScene(itemList[i], 0ms, false);
			}
			rect.drawFrame(2);
			font16(itemList[i]).draw(rect.movedBy(4, 0));
		}

		//Άj[L[μ
		if (false)
		{
			if (KeyW.down() && getData().selectedSceneID > 0)
			{
				getData().selectedSceneID--;
				changeScene(itemList[getData().selectedSceneID], 0ms, false);
			}
			if (KeyS.down() && getData().selectedSceneID < int(itemList.size() - 1))
			{
				getData().selectedSceneID++;
				changeScene(itemList[getData().selectedSceneID], 0ms, false);
			}
		}

		//ωΆΜs―
		for (int i = 0; i < int(citizenData.size()); i++)
		{
			Rect rect(24 * 6, i * 24, 24 * 5, 24);

			if (rect.mouseOver()) rect.draw(Palette::Orange);
			if (rect.leftClicked()) getData().selectedCitizenType = i;
			if (i == getData().selectedCitizenType) rect.draw(Palette::Red);
			rect.drawFrame(2);

			font16(citizenData[i].name()).draw(rect.movedBy(4, 0));
		}

		//Vs―ΗΑ
		{
			Rect rect(144, int(citizenData.size()) * 24, 24 * 5, 24);

			rect.draw(Palette::Gray);
			if (rect.mouseOver()) rect.draw(Palette::Orange);
			if (rect.leftClicked())
			{
				getData().selectedCitizenType = int(citizenData.size());
				citizenData.emplace_back();
			}
			rect.drawFrame(2);

			font16(U"Vs―ΗΑ").draw(rect.movedBy(4, 0));
		}

		if (citizenData.isEmpty()) return;

		//γΊL[Ξ
		{
			if (KeyUp.down() && getData().selectedCitizenType > 0) --getData().selectedCitizenType;
			if (KeyDown.down() && getData().selectedCitizenType < int(citizenData.size() - 1)) ++getData().selectedCitizenType;
		}

		//TabL[Ξ
		if (KeyTab.down())
		{
			auto& c = citizenData[getData().selectedCitizenType];
			if (c.nameTextBox.isActive())
			{
				c.nameTextBox.setActive(false);
				c.wageTextBox.setActive(true);
			}
			else if (c.wageTextBox.isActive())
			{
				c.wageTextBox.setActive(false);
				c.needEnergyTextBox.setActive(true);
			}
			else if (c.needEnergyTextBox.isActive())
			{
				c.needEnergyTextBox.setActive(false);
				c.productNameTextBox.setActive(true);
			}
			else if (c.productNameTextBox.isActive())
			{
				c.productNameTextBox.setActive(false);
				c.numProductTextBox.setActive(true);
			}
			else if (c.numProductTextBox.isActive())
			{
				c.numProductTextBox.setActive(false);
				c.nameTextBox.setActive(true);
			}
			else c.nameTextBox.setActive(true);
		}

		//Nbv{[h
		if (KeyControl.pressed() && KeyV.down())
		{
			auto& c = citizenData[getData().selectedCitizenType];
			String string;

			Clipboard::GetText(string);
			if (string)
			{
				if (c.nameTextBox.isActive()) c.nameTextBox.setText(string);
				if (c.wageTextBox.isActive()) c.wageTextBox.setText(string);
				if (c.needEnergyTextBox.isActive()) c.needEnergyTextBox.setText(string);
				if (c.productNameTextBox.isActive()) c.productNameTextBox.setText(string);
				if (c.numProductTextBox.isActive()) c.numProductTextBox.setText(string);
			}
		}

		//s―ΜXV`ζ
		{
			auto& c = citizenData[getData().selectedCitizenType];

			font16(U"s―Ό").drawAt(348, 12);
			c.nameTextBox.update();
			c.nameTextBox.draw();

			font16(U"ΐΰ").drawAt(348, 60);
			c.wageTextBox.update();
			c.wageTextBox.draw();
			c.wageTextBox.setText(Format(Max(0, ParseInt<int32>(c.wageTextBox.getText(), Arg::radix = 10))));

			font16(U"KvΉ").drawAt(348, 108);
			c.needEnergyTextBox.update();
			{
				TextBox::Style style;

				style.boxColor = energyIsExist(c.needEnergyTextBox.getText()) ? Palette::White : Palette::Red;
				c.needEnergyTextBox.draw(style);
			}

			font16(U"ΆYACeΌ").drawAt(348, 156);
			c.productNameTextBox.update();
			{
				TextBox::Style style;

				style.boxColor = itemIsExist(c.productNameTextBox.getText()) ? Palette::White : Palette::Red;
				c.productNameTextBox.draw();
			}

			font16(U"ACeΆY").drawAt(348, 204);
			c.numProductTextBox.update();
			c.numProductTextBox.draw();

		}

		{
			Rect rect(720, 72, 120, 24);
			if (rect.mouseOver()) rect.draw(Palette::Orange);
			if (rect.leftClicked())
			{
				citizenData.erase(citizenData.begin() + getData().selectedCitizenType);

				if (getData().selectedCitizenType == int(citizenData.size()) && getData().selectedCitizenType > 0) getData().selectedCitizenType--;
			}
			rect.drawFrame(2);
			font16(U"ΪΜν").drawAt(rect.center());
		}
	}
};