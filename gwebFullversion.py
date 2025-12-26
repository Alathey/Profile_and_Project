from selenium import webdriver
from selenium.webdriver.chrome.service import Service as ChromeService

from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import Select
import time
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait

from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys
import pandas as pd
import os
#-----------------------------------------------------------------------------------------------
class Driver():
    def __init__(self, url):
        self.__url = url
        self.__options = webdriver.ChromeOptions()
        self.__service = ChromeService(executable_path="chromedriver.exe")
        self.__driver = webdriver.Chrome(service=self.__service, options=self.__options)
        self.__action = ActionChains(self.__driver)
        self.__driver.maximize_window()
        self.__driver.get(self.__url)

        self.ele_list = []                 # 存子連結
        self.list_text_temp = []           # 暫存txt
        self.ele_list_text = []            # 存txt
        self.ele_index = []                # 過略過的資料

        print("Constructor...") 

        self.index = 1                     # index = 0搜尋特定目標 1拿到所有table
        self.close = ""                    # 按close的button
        self.check = ""                    # 按close的button
        self.keyword = "keyword"           # 關鍵字查詢
        self.nextpage_str = ""

        

    def __del__(self):
        self.__driver.quit()
        del self.ele_index
        del self.ele_list
        del self.ele_list_text
        del self.__action
        del self.__driver
        del self.__service
        del self.__options
        print("Destructor...")   
    
    def set_keyword(self, keyword):
        self.keyword = keyword
    
    def set_index(self, index):
        self.index = index

    def set_close_button(self, close):
        self.close = close

    def set_check(self, check):
        self.check = check
        
    #寫檔案
    def file_write(self, xlsxName, sheetName):
        if(len(self.ele_list_text)!=0):
            df = pd.DataFrame(self.ele_list_text)
            if os.path.exists(xlsxName): # 'a':append
                writer = pd.ExcelWriter(xlsxName, mode='a',
                                         engine='openpyxl', if_sheet_exists='replace')
                df.to_excel(writer, sheet_name=sheetName, columns=False)
                writer.close()
            else:
                df.to_excel(xlsxName, sheet_name=sheetName, columns=False)
            print("寫入檔案...")
        else:
            print("沒有符合的資料...")


    #找到某元素並點擊
    def find_ele_click(self, str):
        WebDriverWait(self.__driver, 30).until(EC.presence_of_element_located((By.CSS_SELECTOR, str)))
        button = self.__driver.find_element(By.CSS_SELECTOR, str)
        self.__action.click(button).perform()
        time.sleep(3)

    #select and option
    def select_option(self, option_value, str):
        WebDriverWait(self.__driver, 30).until(EC.presence_of_element_located((By.CSS_SELECTOR, str)))
        button = self.__driver.find_element(By.CSS_SELECTOR, str)
        select = Select(button)
        select.select_by_value(option_value)
        for op in select.options:
            print(op.text, ':', op.get_attribute('value'))
    
    #字串處理
    def str_process(self, str):
        for ele in self.ele_index:
            str.append(str[0] + ":nth-child(" + f'{ele}' + ")")
        del str[0]
        return str


    def display(self):
        print("len of ele_list = ",  len(self.ele_list))
        print("ele_list = ",  self.ele_list)
        print("ele_list_text = ",  self.ele_list_text)


    def tab_down_key(self, tab_times):
        self.__action.send_keys(Keys.TAB).perform()
        for i in range(tab_times):
            self.__action.send_keys(Keys.DOWN).perform()
            
    def close_button(self):
        try:
            if(self.close != ""):
                WebDriverWait(self.__driver, 30).until(EC.presence_of_element_located((By.CSS_SELECTOR, self.close)))
                close_button = self.__driver.find_element(By.CSS_SELECTOR, self.close)
                close_button.click()
                time.sleep(3)
        except:
            self.list_text_temp.clear()
            self.list_text_temp.append(["no data..."])
            print('table no data')

  
    #找到table的元素
    def find_table_get_url(self, table, tr, td):
        print("function table:",table)
        time.sleep(5)
        WebDriverWait(self.__driver, 30).until(EC.presence_of_element_located((By.CSS_SELECTOR, table)))
        table_tag = self.__driver.find_element(By.CSS_SELECTOR, table)
        trList = table_tag.find_elements(By.CSS_SELECTOR, tr)
        time.sleep(3)
        for trs in trList:
            dataset = [data.get_attribute('href') for data in trs.find_elements(By.CSS_SELECTOR, td)]
            self.ele_list.append(dataset)    
        self.ele_list = [token for st in self.ele_list for token in st]  #降維
        self.len = len(self.ele_list)
        time.sleep(3)


    def into_sub_url(self, table_list, tr, td):
        for ele_url in self.ele_list:
            self.__driver.get(ele_url)
            time.sleep(10)
            self.collect_table_data(table_list, tr, td)


    def collect_table_data(self, table_list, tr, td):
        for ele in table_list:
            tables = self.__driver.find_elements(By.CSS_SELECTOR, ele) 
            print(len(tables))
            print("table:", ele)
            for tab_ele in tables:
                trList = tab_ele.find_elements(By.CSS_SELECTOR, tr)

                for trs in trList:
                    dataset = [data.text for data in trs.find_elements(By.CSS_SELECTOR, td)]
                    self.list_text_temp.append(dataset)

        self.ele_list_text.append([token for st in self.list_text_temp for token in st])  #降維
        self.list_text_temp.clear()
        
class Session():
    def __init__(url):
        pass

#------------------------------------------------------------------------------------------
url = 'https://gweb.wra.gov.tw/Hydroinfo/?id=Index'

driver = Driver(url)
time.sleep(5)

# 地下水井
select_adress = "select[name='CATEGORY']"
option_value1 = 'GW' #GW地下水 LE河川
driver.select_option(option_value1, select_adress)
print('a')

# 行政區
select_adress = "select[class='sel-st-filter']"
option_value2 = 'COUNTY' #COUNTY行政區
driver.select_option(option_value2, select_adress)
print('b')

# option_value2 = 'BASIN' #地下水分區
# driver.select_option(option_value2, select_adress)
# time.sleep(3)
# print('a')

# 區域
# tab_times = int(input('請輸入案down次數(ex:11, 縣市位置): '))
tab_times = 12
driver.tab_down_key(tab_times)
time.sleep(3)                   

# 清單查詢
ele_adress = "div[id='iconListSearch']"
driver.find_ele_click(ele_adress)
time.sleep(3)

# 找table
option_value1 = 'GW' #GW地下水 LE河川
table = "table[data-category="+ option_value1 +"]"      #找到水利鑽井資料的table標籤
tr = "tbody tr"
td = "td:nth-child(2) a"                                # 基本資料的欄位
driver.find_table_get_url(table, tr, td)
driver.display()

#進入下個連結並退出
table_list = ["table[border='0'][style='width:100%;']",
               "table[style='width: 95%;margin: 0 auto;'][border='0']"]
tr = "tbody tr"
td = "td"
driver.into_sub_url(table_list, tr, td)

#寫檔案
xlsxName = 'texttt1.xlsx'
# sheetName = input('請輸入sheet名稱(ex:嘉義縣地下水水文資料): ')
sheetName = '嘉義地下水水文資料'
driver.file_write(xlsxName, sheetName)

del driver