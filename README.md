### [ダウンロードはこちらからどうぞ](https://github.com/sknjpn/Road-of-Gold/releases)
# Road of Goldとは
オープンソースで開発中の本格的な惑星規模の経済シミュレーションゲームです。<br>
プレイヤーは商会の一つとして経済に参加し、活動することができます。<br>
<br>
ここでは困難な開発を少しでも促進するために内部システムの公開と、改善案を募集しています。<br>
<br>
<br>
## 開発環境について
C++のライブラリであるOpenSiv3Dを使って開発しています。<br>
PlanetViewerは例外としてSiv3Dで開発しています。<br>
基本的に最新のバージョンを使いますが、Main.cppにOpenSiv3Dのバージョンを表記します。<br>
<br>
## 惑星
円柱ではなく、球体の惑星を採用しています。<br>
時差があるため、ある時点における時刻は都市ごとに異なります。<br>
<br>
## Node
Nodeは惑星上に無数に存在する座標です。<br>
惑星には数万近いNodeが存在し、座標データと近隣のNodeとの距離データなどを持ちます。<br>
都市はNode上に設置され、交易ユニットはNodeとNode間を移動することができます。<br>
<br>
## 都市
都市の位置はNodeによって指定されます。1つのNodeに複数の都市は存在出来ません。<br>
都市は内部に市民と、市場などを持ちます。<br>
市場は唯一都市のみが持ちます。<br>
都市の役割は商品の生産と取引です。<br>
<br>
## 市場
経済システムの根幹をなす部分です。<br>
市場は商会や都市の販売先、商会の仕入れ先、市民の購入先としての役割を持ちます。<br>
市場は「Basket」と「Ring」によって商品管理を行います。<br>
「Basket」は同一商品の「Ring」の集合体です。<br>
「Ring」には「個数」「値段」「出品者」などが登録されていて<br>
市民は購入条件に一致するRingを見つけると購入処理を行います。<br>
基本的な設定として、販売価格は前日の朝の出品価格の1.05倍から2.00倍で、<br>
1日のうちに売り切れなければ価格が0.95倍されます<br>
<br>
## 交易ユニット
交易ユニットは商品を積載し、都市間を移動することが出来ます。<br>
これにより都市で生産できない商品を輸入することが出来ます。<br>
交易ユニットの速度はVehicleDataとBiomeDataのMovingCostに依存します。<br>
<br>
## 商会
商会は資源の流通を行う組織で、乗り物を交易路に設定し定期的な購入と販売を行います。<br>
資本金が決められていて、その予算内で様々な行動を行うことが出来ます。<br>
