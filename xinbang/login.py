from selenium import webdriver
from requests import Session
from time import sleep
from lxml import etree


chromePath = r'C:\Users\Administrator\AppData\Local\Google\Chrome\Application\chromedriver.exe'
chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--headless')

wd = webdriver.Chrome(executable_path=chromePath,chrome_options=chrome_options) 

xinbangLogInUrl = 'https://www.newrank.cn/public/info/search.html?value=if_fashion888&isBind=false'
wd.get(xinbangLogInUrl)

wd.find_element_by_xpath('/html/body/div[10]/div[1]/div[2]').click()
wd.find_element_by_xpath('//*[@id="account_input"]').send_keys('nr_953c4lm34') 
wd.find_element_by_xpath('//*[@id="password_input"]').send_keys('xinbang123456')
sleep(2)

wd.find_element_by_xpath('//*[@id="pwd_confirm"]').click()
sleep(2)#等待Cookies加载

text = wd.find_element_by_xpath('//*[@id="show_0AE34266BC626F9D134FC618776AE30A"]/span[1]').text
print(text)
wd.close()



# cookies = wd.get_cookies()

# req = Session()
# for cookie in cookies:
#     req.cookies.set(cookie['name'],cookie['value'])

# wd.close()

# url = 'https://www.newrank.cn/public/info/search.html?if_fashion888&isBind=false'

# req = req.get(url)
# req.encoding = 'utf8'

# print('status_code = {0}'.format(req.status_code))

# print(req.text)

# root = etree.HTML(req.content)

# datas = root.xpath('//*[@id="show_0AE34266BC626F9D134FC618776AE30A"]/span')

# for data in datas:
#     print(data.text)

