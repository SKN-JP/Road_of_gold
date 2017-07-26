﻿#include"Planet.h"
#include"Node.h"
#include"Pi.h"
#include"JSON.h"
#include"Urban.h"
/*
Road of Gold専用マップエディタ
*/

Array<Urban> urbans;
Planet planet;
TinyCamera2D tinyCamera2D;
int		selectedBiome = 0;
int		selectedBrush = 0;
int		brushSize = 10;
Urban*	selectedUrban = nullptr;


void Main()
{
	const Rect uiRect(32, 32, 320, 720 - 64);
	const Font font12(12);
	const Font font16(16);
	const Font font24(24);
	const Font textBoxFont(12, Typeface::Bold);

	enum struct ActionMode {
		none,
		set,
		remove,
	} actionMode = ActionMode::set;

	enum struct UIMode {
		setBiome,
		setUrban,
	} uiMode = UIMode::setBiome;

	bool	drawOutlineEnabled = true;
	Node*	nearestNode = &nodes[0];

	Window::SetTitle(L"MapEditor");
	Window::Resize(1280, 720);

	//データの読み込み
	if (!loadJSONData() || !planet.loadVoronoiMap()) return;

	//ファイル名入力欄
	TextBox textBox(textBoxFont, Vec2(160, 72), 120);

	Array<TextBox> resouceTextBox;
	for (auto i : step(rData.size()))
		resouceTextBox.emplace_back(textBoxFont, Vec2(114, 114 + i * 20), none);

	while (System::Update())
	{

		//キー入力
		if (!textBox.isActive())
		{
			if (KeyG.down()) drawOutlineEnabled = !drawOutlineEnabled;
			if (Key1.down()) selectedBiome = 0;
			if (Key2.down()) selectedBiome = 1;
			if (Key3.down()) selectedBiome = 2;
			if (Key4.down()) selectedBiome = 3;
			if (Key5.down()) selectedBiome = 4;
			if (Key6.down()) selectedBiome = 5;
			if (Key7.down()) selectedBiome = 6;
			if (Key8.down()) selectedBiome = 7;
			if (Key9.down()) selectedBiome = 8;
			if (Key0.down()) selectedBiome = 9;
			if (KeyR.down()) selectedBrush = 0;
			if (KeyF.down()) selectedBrush = 1;
			if (KeyV.down()) selectedBrush = 2;
		}

		//ブラシサイズの変更
		if (KeyControl.pressed()) brushSize = Max(2, int(brushSize - Mouse::Wheel()));

		//カメラの更新
		tinyCamera2D.update();

		//マップの描画
		for (int i = 0; i < 2; i++) {
			const auto t1 = tinyCamera2D.createTransformer(i);

			planet.mapTexture.resize(TwoPi, Pi).drawAt(0, 0);
			if (drawOutlineEnabled) planet.outlineTexture.resize(TwoPi, Pi).drawAt(0, 0);
		}

		//都市の描画
		for (int i = 0; i < 2; i++) {
			const auto t1 = tinyCamera2D.createTransformer(i);

			for (auto& u : urbans)
				Circle(u.getPos().mPos, 0.012).draw(Palette::Red).drawFrame(0.002, 0.0, Palette::Black);
		}

		//selectedUrbanの描画
		if (selectedUrban != nullptr && uiMode == UIMode::setUrban)
		{
			for (int i = 0; i < 2; i++) {
				const auto t1 = tinyCamera2D.createTransformer(i);

				Circle(selectedUrban->getPos().mPos, 0.012).draw(Palette::Yellow).drawFrame(0.002, 0.0, Palette::Black);
			}
		}
		else selectedUrban = nullptr;

		if (!uiRect.mouseOver())
		{

			//nearestNodeの設定
			{
				const auto& p = (tinyCamera2D.getCursorPos().mPos / TwoPi).movedBy(0.5, 0.25)*planet.voronoiMap.size().x;
				nearestNode = &nodes[planet.voronoiMap[int(p.y)][int(p.x)]];
			}

			//nearestNodeの描画
			for (int i = 0; i < 2; i++)
			{
				const auto t1 = tinyCamera2D.createTransformer(i);
				Circle(nearestNode->pos.mPos, 0.01).drawFrame(0.003, Palette::Black);
			}

			switch (uiMode)
			{
			case UIMode::setBiome:

				//色の取得
				if (MouseR.down()) selectedBiome = nearestNode->biomeType;

				//ブラシの使用
				switch (selectedBrush)
				{
				case 0:
					if (MouseL.pressed() && nearestNode->biomeType != selectedBiome)
					{
						Array<Node*> list = { nearestNode };
						nearestNode->biomeType = selectedBiome;
						planet.updateImage(list);
					}
					break;

				case 1:
					if (MouseL.pressed())
					{
						Array<Node*> list;
						auto mp = tinyCamera2D.getCursorPos();
						for (auto& n : nodes)
						{
							if ((n.pos.ePos - mp.ePos).length() < 0.01*brushSize)
							{
								if (n.biomeType != selectedBiome)
								{
									n.biomeType = selectedBiome;
									list.emplace_back(&n);
								}
							}
						}
						planet.updateImage(list);
					}
					break;

				case 2:
					if (MouseL.down() && nearestNode->biomeType != selectedBiome)
					{
						Array<Node*> list;
						list.emplace_back(nearestNode);
						for (int i = 0; i < int(list.size()); i++)
						{
							auto& n1 = list[i];
							for (auto& p : n1->paths)
							{
								auto& n2 = p.getChild();
								if (n1->biomeType == n2.biomeType && !list.any([&n2](Node* _n) {return _n == &n2; }))
								{
									list.emplace_back(&n2);
								}
							}
						}
						for (auto& n : list) n->biomeType = selectedBiome;
						planet.updateImage(list);
					}
					break;
				}

				break;

			case UIMode::setUrban:

				if (MouseR.down())
				{
					selectedUrban = nullptr;
					actionMode = ActionMode::none;
				}

				switch (actionMode)
				{
				case ActionMode::none:
					if (MouseL.down() && nearestNode->ownUrbanID != -1)
					{
						selectedUrban = &urbans[nearestNode->ownUrbanID];
						for (auto i : step(rData.size()))
							resouceTextBox[i].setText(Format(selectedUrban->resource[i]));
					}
					break;
				case ActionMode::set:
					if (MouseL.down() && nearestNode->ownUrbanID == -1)
					{
						urbans.emplace_back(nearestNode->id);
						selectedUrban = &urbans.back();
						for (auto i : step(rData.size()))
							resouceTextBox[i].setText(Format(selectedUrban->resource[i]));
					}
					break;
				case ActionMode::remove:
					if (MouseL.down() && nearestNode->ownUrbanID != -1)
					{
						const int targetID = nearestNode->ownUrbanID;
						urbans.remove_if([&nearestNode](Urban& u) {return nearestNode->ownUrbanID == u.id; });
						nearestNode->ownUrbanID = -1;
						selectedUrban = nullptr;
						//IDの整合性を取る
						for (auto& n : nodes)
							if (n.ownUrbanID > targetID) --n.ownUrbanID;
						for (auto& u : urbans)
							if (u.id > targetID) --u.id;
					}
					break;
				}

				break;
			}
		}

		//スライドバーの描画
		tinyCamera2D.draw();

		//UIの描画
		uiRect.draw(Color(Palette::Darkcyan, 192)).drawFrame(1, 0, Palette::Skyblue);

		//UIModeの選択
		{
			const Array<String> ns = { L"🌍",L"🏭" };
			for (auto i : step(int(ns.size())))
			{
				const int width = 320 / int(ns.size());
				const Rect rect(32 + width*i, 32, width, 32);
				if (rect.leftClicked()) uiMode = UIMode(i);
				rect.draw(int(uiMode) == i ? Palette::Red : rect.mouseOver() ? Palette::Orange : Color(0, 0)).drawFrame(1, 0, Palette::Skyblue);
				font24(ns[i]).drawAt(rect.center());
			}
		}
		switch (uiMode)
		{
		case UIMode::setBiome:
		{
			//一覧
			for (auto i : step(20))
			{
				const Rect rect(32, 64 + i * 16, 160, 16);
				const Color color(selectedBiome == i ? Palette::Red : rect.mouseOver() ? Palette::Orange : Color(0, 0));
				if (i < int(bData.size()))
				{
					if (rect.leftClicked()) selectedBiome = i;
					rect.draw(color);
					font12(bData[i].name).draw(rect.pos.movedBy(12, 0));
				}
				else
				{
					if (color != Color(0, 0)) rect.draw(Color(color, 128));
					font12(L"---").draw(rect.pos.movedBy(12, 0));
				}
				rect.drawFrame(1, 0, Palette::Skyblue);
			}
			//詳細
			{
				const Rect rect(192, 64, 160, 40);
				rect.drawFrame(1, 0, Palette::Skyblue);
				font16(L"選択中のバイオーム").draw(rect.pos.movedBy(4, 0));
				font12(bData[selectedBiome].name).draw(rect.pos.movedBy(4, 22));
			}
			//ブラシの選択
			{
				const Rect rect(192, 104, 160, 128);
				rect.drawFrame(1, 0, Palette::Skyblue);
				font16(L"ブラシの選択").draw(rect.pos.movedBy(4, 0));
				const Array<String> brushName = { L"鉛筆:KeyR", L"筆:KeyF", L"バケツ:KeyV" };
				for (auto i : step(int(brushName.size())))
				{
					const Rect s(rect.pos.movedBy(6, 26 + i * 20), 16, 16);
					if (s.leftClicked()) selectedBrush = i;
					s.draw(selectedBrush == i ? Palette::Red : s.mouseOver() ? Palette::Orange : Palette::White).drawFrame(2, 0, Palette::Black);
					font12(brushName[i]).draw(s.pos.movedBy(24, 0));
				}
				font12(L"太さ:", brushSize).draw(rect.pos.movedBy(4, 94));
				font12(L"太さはCtrl+Wheelで変更可").draw(rect.pos.movedBy(4, 110));
			}
			//生成
			{
				const Rect rect(192, 232, 160, 24);
				rect.drawFrame(1, 0, Palette::Skyblue);
				const Rect s(rect.pos.movedBy(136, 4), 16, 16);
				if (s.leftClicked()) planet.generateBiome();
				s.draw(s.mouseOver() ? Palette::Orange : Palette::White).drawFrame(2, 0, Palette::Black);
				font16(L"マップの自動生成").draw(rect.pos.movedBy(4, 0));
			}
			//セーブ
			{
				const Rect rect(192, 256, 160, 48);
				textBox.setPos(rect.pos.movedBy(4, 28));
				rect.drawFrame(1, 0, Palette::Skyblue);
				const Rect s(rect.pos.movedBy(136, 4), 16, 16);
				if (s.leftClicked())
				{
					FilePath filePath = L"Map/" + (textBox.getText().indexOf(L".bin") != String::npos ? textBox.getText() : textBox.getText() + L".bin");
					saveBiomeData(filePath);
				}
				s.draw(s.mouseOver() ? Palette::Orange : Palette::White).drawFrame(2, 0, Palette::Black);
				font16(L"セーブ").draw(rect.pos.movedBy(4, 0));
				textBox.update();
				textBox.draw();
			}

			break;
		}
		case UIMode::setUrban:
		{
			{
				const Rect rect(32, 64, 160, 24);
				rect.drawFrame(1, 0, Palette::Skyblue);
				const Rect s(rect.pos.movedBy(4, 4), 16, 16);
				if (s.leftClicked()) actionMode = actionMode == ActionMode::set ? ActionMode::none : ActionMode::set;
				s.draw(actionMode == ActionMode::set ? Palette::Red : s.mouseOver() ? Palette::Orange : Palette::White).drawFrame(2, 0, Palette::Black);
				font16(L"都市配置モード").draw(rect.pos.movedBy(28, 0));
			}
			{
				const Rect rect(32, 88, 160, 24);
				rect.drawFrame(1, 0, Palette::Skyblue);
				const Rect s(rect.pos.movedBy(4, 4), 16, 16);
				if (s.leftClicked()) actionMode = actionMode == ActionMode::remove ? ActionMode::none : ActionMode::remove;
				s.draw(actionMode == ActionMode::remove ? Palette::Red : s.mouseOver() ? Palette::Orange : Palette::White).drawFrame(2, 0, Palette::Black);
				font16(L"都市削除モード").draw(rect.pos.movedBy(28, 0));
			}
			for (auto& i : step(int(rData.size())))
			{
				const Rect rect(32, 112 + i * 20, 80, 20);
				rect.drawFrame(1, 0, Palette::Skyblue);
				font12(rData[i].name).draw(rect.pos.movedBy(4, 0));
			}
			if (selectedUrban != nullptr)
			{
				for (auto& i : step(int(rData.size())))
				{
					const Rect rect(112, 112 + i * 20, 60, 20);
					rect.drawFrame(1, 0, Palette::Skyblue);

					auto& t = resouceTextBox[i];
					t.setWidth(56);
					t.update();
					selectedUrban->resource[i] = ParseInt<int>(t.getText()) % 10000;
					t.setText(Format(selectedUrban->resource[i]));
					t.draw();
				}
			}
			break;
		}
		}

		//ロード
		auto items = DragDrop::GetDroppedFilePaths();
		for (auto item : items)
		{
			BinaryReader reader(item.path);
			Array<Node*> list;
			for (auto& n : nodes)
			{
				int t;
				reader.read(t);
				if (t != n.biomeType)
				{
					n.biomeType = t;
					list.emplace_back(&n);
				}
			}
			planet.updateImage(list);
		}
	}
}
