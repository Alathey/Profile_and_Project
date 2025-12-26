from tensorflow.keras.models import Model
from tensorflow.keras.datasets import mnist
from tensorflow.keras.layers import (
    Input, Dense, Flatten, add,
    Conv2D, Activation, UpSampling2D,
    BatchNormalization
)
from tensorflow.keras import backend as K
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras.utils import to_categorical, plot_model
# -------------------------------------------

# 定義 ResNet block 結構
def resNetBlock(numFilter, nBlock, x):
    # 判斷是否為 block 中的第一層（需下採樣）
    numStride = 1 if nBlock != 0 else 2  # 非第一個block時保持原尺寸，# 第一個block時下採樣，尺寸變為一半

    # 左支路（主路徑）：3個卷積層
    y = Conv2D(numFilter, (1, 1), strides=numStride)(x)
    y = BatchNormalization(axis=-1)(y)   # 對每個 channel 做標準化
    y = Activation('relu')(y)

    y = Conv2D(numFilter, (3, 3), padding='same')(y) # y 是(batch_size, height, width, channels) = (輸入的照片數，3,3,numFilter)，這表示輸出numFilter種不同3x3的眼睛、毛髮、紋理等等特徵圖
    y = BatchNormalization(axis=-1)(y)
    y = Activation('relu')(y)

    y = Conv2D(numFilter*4, (1, 1))(y)
    y = BatchNormalization(axis=-1)(y)

    # 右支路（shortcut）：只有在第一層才需要額外卷積以對齊維度，其他block直接跳接
    if nBlock == 0:
        shortcut = Conv2D(numFilter*4, (1, 1), strides=numStride)(x)
        shortcut = BatchNormalization(axis=-1)(shortcut)
    else:
        shortcut = x

    # 殘差結構：加總主支路與shortcut支路的輸出，再通過ReLU
    y = add([y, shortcut])
    y = Activation('relu')(y)
    return y
# -------------------------------------------

# 建立模型架構
inpLyr = Input((28, 28, 1))  # 輸入層：28x28 灰階影像

# 初始卷積層，擷取特徵，filter數量為64，kernel大小為11x11，stride為移動5單位掃描
x = Conv2D(64, (11, 11), strides=5, padding='same')(inpLyr)
x = BatchNormalization(axis=-1)(x)
x = Activation('relu')(x)

# -----------------------------------------
# 建立ResNet區塊
numFilter = 32         # 基礎filter數量
numResNet = 2          # ResNet block數量（幾個殘差模組）
numBlock = 3           # 每個ResNet block內有幾個子block

# 疊加ResNet blocks，通常i會設定讓通道數會加倍ex:numFilter = baseFilter * (2 ** i)，控制「這個區段裡有幾個殘差單元（residual blocks）」
for i in range(numResNet):
    for b in range(numBlock):
        x = resNetBlock(numFilter, b, x)

# -----------------------------------------
# 特徵圖放大（因為前面有下採樣），恢復部分尺寸
x = UpSampling2D((2, 2))(x)

# 將特徵圖壓縮回1個通道（灰階），用於分類
x = Conv2D(1, (3, 3), padding='same', activation='relu')(x)

# 將多維特徵展平為1維向量
x = Flatten()(x)

# 全連接層，輸出層10個分類的機率（MNIST：0~9）
x = Dense(10, activation='softmax')(x)

# 建立整體模型
model = Model(inputs=inpLyr, outputs=x)

# 儲存網路結構圖
plot_model(model, to_file='22_resNet.png')

# ---------------------------------------------------
# 載入MNIST手寫數字資料集
numClass = 10
(trnX, trnY), (tstX, tstY) = mnist.load_data()

# 取得訓練與測試資料的維度資訊
trnSpl, row, col = trnX.shape  # trnSpl：訓練樣本數
tstSpl, _, _ = tstX.shape      # _, _：忽略通道數，因為後續會根據後端格式調整資料形狀

# 根據後端格式調整資料shape（TensorFlow為channels_last）
if K.image_data_format() == 'channels_first':   # (樣本數, 通道數, 高度, 寬度)
    trnX = trnX.reshape(trnSpl, 1, row, col)
    tstX = tstX.reshape(tstSpl, 1, row, col)
    inputShape = (1, row, col)
else:                                            # (樣本數, 高度, 寬度, 通道數)
    trnX = trnX.reshape(trnSpl, row, col, 1)
    tstX = tstX.reshape(tstSpl, row, col, 1)
    inputShape = (row, col, 1)

# 正規化：將像素值縮放到 [0, 1]
trnX = trnX.astype('float32') / 255.0
tstX = tstX.astype('float32') / 255.0

# 將標籤轉換為one-hot格式
trnY = to_categorical(trnY, numClass)
tstY = to_categorical(tstY, numClass)

# 編譯模型
model.compile(
    loss='categorical_crossentropy',  # 多類別分類損失函數
    optimizer='rmsprop',              # 優化器
    metrics=['accuracy']              # 評估指標
)

# 訓練模型
NUM_EPOCHS = 2
history = model.fit(
    trnX, trnY,
    validation_data=(tstX, tstY),  # 使用測試資料驗證
    epochs=NUM_EPOCHS              # 訓練輪數
)