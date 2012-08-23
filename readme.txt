epgdump改良版

https://github.com/Piro77/epgdump

使い方
epgdump /CS cs.ts cs.xml
epgdump /BS bs.ts bs.xml
epgdump GR32 gr32.ts gr32.xml

引数１・CS/BS/GR種別
        出力のチャンネルIDに利用します。後ろに_サービスIDが付加されます。
        BS/CSについては引数とは無関係にTSIDより識別します。
        例:CS_101
           BS_237
           GR32_23122
引数２・TSファイル名
引数３・出力XMLファイル名（−で標準出力）

TSを読み込んでxmlを出力します。xmlの形式は独自なので他のepgdump
の置き換えには使えません。

epgdump csv gr32.ts gr32.csv

引数１・csv(固定)
引数２・TSファイル名
引数３・出力csvファイル名（−で標準出力）

XMLの代わりにCSV形式で出力します。

epgdump check /dev/ptx0.t0 21234 4384 2012-07-15T11:30:00

引数１・check(固定)
引数２・デバイス名
引数３・EITサービスID
引数４・EITイベントID
引数５・EITイベント開始時刻

デバイスからTSを読み出し、引数で渡されたサービスIDのイベントIDが
EITの次のイベントにあるかチェックします。
次のイベントにある場合０で終了します。
次のイベントにない場合１で終了します。
EIT情報が取得できない場合等含めて最長約１０秒で終了します。
第５引数とイベント開始時刻差を秒数で標準出力に出力します。

epgdump wait /dev/ptx0.t0 21234 4384 3600

引数１・wait(固定)
引数２・デバイス名
引数３・EITサービスID
引数４・EITイベントID
引数５・最大待ち秒数

デバイスからTSを読み出し、引数で渡されたサービスIDのイベントIDの開始を待機します。
待機が成功した場合０で終了します。(該当イベントの開始、または約３０秒で開始）
待機が失敗した場合１で終了します


変更点
・2chのスレのパッチ取り込み
・読み込み時にseekしないようにした
・取得EIT情報の増加
・BS/CSのテーブル不要化
・スケジュール対応コード（イベント待ち）
・xml出力変更

TODO
・NITからリモコンIDを取得(GR+リモコンID)・第一引数のダミー化
・番組表が埋まったらEXITする(ストリーム読み込み)
・ジャンルコードの取り込み
・TOTによる時刻合わせ


以下オリジナルreadme
------
xmltv-epg

MPEG-TSに含まれるepgをxmlで出力するプログラムです。
◆N/E9PqspSk氏がrecfriio Solaris版(http://2sen.dip.jp/cgi-bin/friioup/source/up0737.zip)に含まれるepgdumpを
Linux版を改造したものをベースにxmltv用のxmlファイルを作成します。

また、タイトル内に含まれる
    "　第"や、"　「"、"　＃"、"（"、"「"、をサブタイトルとして扱う様に修正しています。
例：

タイトルが、
明日のよいち！「あの時君はワルかった」
の場合
明日のよいち！がタイトルに、「あの時君はワルかった」がサブタイトルに追加されます。

サブタイトルとして扱うものを増やす場合は、eit.cにあるsubtitle_cnv_strに追加してください。

使用方法は以下の通りです。

Usage : ./epgdump /BS <tsFile> <outfile>
Usage : ./epgdump <ontvcode> <tsFile> <outfile>
ontvcode チャンネル識別子。****.ontvjapan.com など
/BS      BSモード。一つのTSからBS全局のデータを読み込みます。
/CS      CSモード。一つのTSから複数局のデータを読み込みます。

makeするとepgdumpがビルドされます。

epgdumpライセンス(Solaris版より引用):
>epgdumpに関しては、BonTest Ver.1.40からそのままソースを持ってきている部分も
>あるため、そのライセンスに従いします。
>BonTestのReadme.txtより
>>
>>３．ライセンスについて
>>　　・本パッケージに含まれる全てのソースコード、バイナリについて著作権は一切主張しません。
>>　　・オリジナルのまま又は改変し、各自のソフトウェアに自由に添付、組み込むことができます。
>>　　・但しGPLに従うことを要求しますのでこれらを行う場合はソースコードの開示が必須となります。
>>　　・このとき本ソフトウェアの著作権表示を行うかどうかは任意です。
>>　　・本ソフトウェアはFAAD2のライブラリ版バイナリを使用しています。
>>
>>　　　"Code from FAAD2 is copyright (c) Nero AG, www.nero.com"
>>
>>　　・ビルドに必要な環境
>>　　　- Microsoft Visual Studio 2005 以上　※MFCが必要
>>　　　- Microsoft Windows SDK v6.0 以上　　※DirectShow基底クラスのコンパイル済みライブラリが必要
>>　　　- Microsoft DirectX 9.0 SDK 以上

Special Thanks:
・Solaris版開発者の方
・拡張ツール中の人
・◆N/E9PqspSk氏
・ARIB(資料の無料ダウンロードに対して)

動作確認環境:
  Debian GNU/Linux sid
  Linux 2.6.27.19 SMP PREEMPT x86_64

tomy ◆CfWlfzSGyg
