# 載入所需的函式庫
import pandas as pd  # 資料處理
import numpy as np  # 數值運算
from sklearn.preprocessing import MinMaxScaler  # 資料標準化
from tensorflow.keras.models import Sequential  # 建立序列模型
from tensorflow.keras.layers import Dense, LSTM  # 加入 LSTM 和 Dense 層
import matplotlib.pyplot as plt  # 畫圖用
import os

# 關閉 TensorFlow 訊息等級 (減少警告訊息輸出)
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

# 讀取 Excel 資料
fileName = 'HW12CO2.xlsx'  # 檔案名稱
sheetName = 'Data'  # 工作表名稱
df = pd.read_excel(fileName, sheet_name=sheetName, engine='openpyxl')  # 讀取 Excel 資料為 DataFrame

# 取得其中的 'value' 欄位並 reshape 成一列一列的格式 (n, 1)
Data = df['value'].values.reshape(-1, 1)

# 將資料標準化至 0~1 區間 (有助於 LSTM 訓練)
scaler = MinMaxScaler(feature_range=(0, 1))
sData = scaler.fit_transform(Data).ravel()  # 壓平成一維陣列

# 建立時間序列的輸入輸出資料
xList = []  # 儲存每段時間的輸入 (30筆)
yList = []  # 對應的預測目標
timeStep = 30  # 每筆輸入資料包含的時間點數 (時間視窗)

# 從 timeStep 開始，建立時間序列對
for i in range(timeStep, len(sData)):
    xList.append(sData[i - timeStep:i])  # 前30筆作為輸入
    yList.append(sData[i])  # 第31筆作為對應輸出

# 轉成 numpy array 格式
xData = np.array(xList)
yData = np.array(yList)

# 切分訓練與測試資料（80%訓練，20%測試）
trnCount = int(xData.shape[0] * 0.8)
trnX = xData[:trnCount].reshape(-1, timeStep, 1)  # 訓練輸入轉為3D陣列 (樣本數, timestep, 特徵數)
trnY = yData[:trnCount]  # 訓練輸出
tstX = xData[trnCount:].reshape(-1, timeStep, 1)  # 測試輸入
tstY = yData[trnCount:]  # 測試輸出

def build_model():
    # 建立 LSTM 模型，用兩層 LSTM 可以學習「時間內的特徵」+「整體趨勢」。
    model = Sequential()
    # 第一層 LSTM，有 50 個 units，並回傳每一時間點的輸出 (return_sequences=True)，input_shape=(time_steps, features) == 輸入形狀=(每筆資料有幾個時間點, 每個時間點有幾個特徵)
    model.add(LSTM(units=50, return_sequences=True, input_shape=(timeStep, 1)))
    # 第二層 LSTM，有 50 個 units，只回傳最後一個輸出 (return_sequences=False)，壓縮成一個50維度的特徵向量(趨勢波動等等)，為 Dense 做輸入準備
    model.add(LSTM(units=50, return_sequences=False))
    # 全連接層（Dense），包含 25 個神經元，中間層學習非線性關係，增強預測能力
    model.add(Dense(units=25))
    # 最後一層輸出層，輸出單一預測值
    model.add(Dense(units=1))

    # 編譯模型，使用 Adam 最佳化器和均方誤差作為損失函數
    model.compile(optimizer='adam', loss='mean_squared_error', metrics=['RootMeanSquaredError'])
    return model

model = build_model()
# 顯示模型架構摘要
print(model.summary())

# 訓練模型（執行500次迭代）
model.fit(trnX, trnY, epochs=500)

# 預測訓練與測試資料
trnPred = model.predict(trnX)  # 預測訓練集
tstPred = model.predict(tstX)  # 預測測試集

# 畫出實際與預測值圖形（將標準化後的資料轉回原始尺度）
plt.plot(scaler.inverse_transform(trnY.reshape(-1, 1)), label='CO₂ train y')  # 訓練資料實際值
plt.plot(scaler.inverse_transform(trnPred.reshape(-1, 1)), label='CO₂ train prediction')  # 訓練資料預測值
plt.legend()
plt.show()

# 繪製測試資料實際值與預測值（將標準化資料還原）
plt.figure(figsize=(12, 5))
plt.plot(scaler.inverse_transform(tstY.reshape(-1, 1)), label='CO₂ test y')  # 測試資料實際值
plt.plot(scaler.inverse_transform(tstPred.reshape(-1, 1)), label='CO₂ test prediction')  # 測試資料預測值
plt.title('Test Data CO₂ vs. Predictions CO₂')
plt.xlabel('Time')
plt.ylabel('CO₂ Value')
plt.legend()
plt.grid(True)
plt.show()

# 計算 RMSE誤差（轉回原始尺度後）
from sklearn.metrics import mean_squared_error
import math
rmse_trn = math.sqrt(mean_squared_error(
    scaler.inverse_transform(trnY.reshape(-1, 1)),
    scaler.inverse_transform(trnPred.reshape(-1, 1))
))
rmse_tst = math.sqrt(mean_squared_error(
    scaler.inverse_transform(tstY.reshape(-1, 1)),
    scaler.inverse_transform(tstPred.reshape(-1, 1))
))

print(f"Train RMSE: {rmse_trn:.4f}")
print(f"Test  RMSE: {rmse_tst:.4f}")

# # 存模型、載入模型 下次就不用在訓練一次
# model.save('lstm_co2_model.h5')  # 儲存整個模型到 HDF5 檔案
# print("模型已儲存。")

# from tensorflow.keras.models import load_model

# model = load_model('lstm_co2_model.h5')  # 讀取儲存的模型
# print("模型已載入。")