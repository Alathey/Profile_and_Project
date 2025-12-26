"# Portfolio" 
gwebFullversion.py
專案背景: 論文的前處理，利用公開網站水利署水文資訊網針對地下水、河川中撈取雲林、嘉義、台南的監測站水井、水文特徵像是：水位、流量、滲透係數、單位洩降、透水係數等等結構化後存入CSV檔
網站連結：https://gweb.wra.gov.tw/Hydroinfo/?id=Index
技術細節：seleium模擬滑鼠選取雲林、嘉義、台南的監測站，進入各個測站對資料進行擷取
環境配置：ChromeDriver 110.0.5481.77、 Visual Studio Code、 Anaconda
程式語言：Python，seleium
其他語言：需了解基本HTML結構

專案結構：
1. class：Driver class
2. function：
    * __init__ 物件初始化
    * __del__ 物件銷毀
    * str_process 字串處理
    * display 顯示一些抓到的資訊：可以拿來debug
    * select_option 資料種類選擇，下拉式的選單
    * find_ele_click 找到ICON並點擊
    * tab_down_key 模擬鍵盤DOWN鍵
    * close_button 關按鈕
    * find_table_get_url 找到table元素中的子連結，存起來
    * into_sub_url 進入子連結
    * collect_table_data 收集表格資料
    * file_write 寫檔

設計考量：
1. 泛用化
2. 像是存成csv檔、抓table的function在其他地方(像是我的論文結果水位、流量數據是.sfr檔案直接轉換成csv)，txt檔轉成csv檔就可以直接用

*設計Driver class
當時設計考量基於論文可能不只這網站需要撈資料，若有其他網址需要抓取，步驟可能大同小異
因此設計函數：點擊、進入子連結、找到表格的位置、收集表格資料、存成CSV檔案。

-------------------------------------------------------------------------------------------------------------------------------------------------

"# Portfolio" 
BPTree3.cpp
專案背景：資料庫系統課程的課堂實作，老師要求實作底層資料庫未限制程式語言，這裡採用C++去實作互動式資料庫儲存結構，包含B+ Tree中新增、查詢、刪除、走訪、雙向鍵結
技術細節：重點主要在於刪除的部分以及雙向鍵結的部分是網路上的資源較少，這部分依照資料結構原理進行創作
環境配置：MinGW-w64、 Visual Studio Code
程式語言：C++、Tree

專案架構：
1. class：樹的節點、BPTree建樹
2. class BPTree中函數：
  a. containsValue() 檢查值是否重複
  b. insertValue() 插入值
  c. deleteValue() 刪除值
  d. displayDoubleLinkList() 雙向鍵結的展示
  e. display() 展示樹的新增/刪除過程
  f. remove() 在destructor中使用，必免memory leak

設計考量：
1. deleteValue中，在建樹的過程遇到父子關係的傳遞，把要移動的Node先存在原本被刪掉的位置，等向上傳遞完後再刪掉
2. 互動式BPTree設計：資料庫在創作時需要先確認要的儲存體的數量(order)，把它設計成可以更改數量的結構

處理問題：
1. 回傳值參雜一些遞迴結構 很容易overstack，指標在每個head、parent、child、是否是null都需要檢查

參考資料：
1. https://www.geeksforgeeks.org/dsa/insertion-in-a-b-tree/
2. https://www.geeksforgeeks.org/dsa/deletion-in-b-tree/
3. https://www.cnblogs.com/JayL-zxl/p/14304178.html

--------------------------------------------------------------------------------------------------------------------------------------------

"# 學術專案/作業經驗" 
LSTM.py
作業背景: 深度學習課程中的作業，這是一個二氧化碳的排放量預測，老師邊教學，我們邊實作並附上備註
環境配置：Visual Studio Code、 Anaconda、Python版本3.8.20
程式語言：Python，tensorflow，LSTM

專案架構：
1. 讀取檔案
2. 結構化、朔型
3. 標準化
4. 建立時間序列
5. 切分訓練集、測試集
6. 建立Model
  * Sequential：線性堆疊
  * 第一層LSTM：理解基本特徵：對每個時間點特徵提取提供完整時間序列給下一層
  * 第二層LSTM：深度學特徵：細微的趨勢、波動等等
  * 中間層：學習非線性關係，加強預測能力
  * 輸出層：輸出最終預測
  * compile：用哪些方法衡量錯誤、更新權重、監控指標
7. 訓練模型
8. 預測模型
9. 畫圖

學習過程：這個專案是我練習LSTM預測時間序列的作業，我理解資料預測處理、模型建立、評估

成果：
Train RMSE: 0.3952
Test  RMSE: 0.5775

-------------------------------------------------------------------------------------------------------------------------------------------------

"# 學術專案/作業經驗" 
ResNet.py
作業背景: 深度學習課程中的作業，這是一個預測MNIST資料集的作業，學習怎麼建立殘差結構，老師邊教學，我們邊實作並附上備註
環境配置：Visual Studio Code、 Anaconda、Python版本3.8.20
程式語言：Python，tensorflow，ResNet

專案架構：
1. 定義殘差結構：resNetBlock function
  * 左支路：三個卷積層(1x1,3x3,1,1)
  * 右支路：如果第一個Block是shortcut要改變channel數量，其他直接輸出
  * 殘差結構：左支路+右支路
    
3. 建立Block區塊並呼叫function：幾個ResNet、Block
4. 特徵圖尺寸放大
5. 卷積層：一個通道分類用
6. 平坦化後放入全連接層：機率輸出
7. 建立模型
8. 載入MNIST資料並訓練
9. 編譯模型

學習過程：這個專案是我練習ResNet的殘差結構的結構，讓我理解梯度更容易傳遞減少梯度消失的問題。

成果：
Train 準確率：0.9650
Test   準確率：0.9691
