from selenium import webdriver
from requests import Session
from time import sleep

req = Session()
req.headers.clear() 
chromePath = r'C:\Program Files (x86)\Google\Chrome\Application\chromedriver.exe'
wd = webdriver.Chrome(executable_path= chromePath) 

xinbangLogInUrl = 'https://www.newrank.cn/public/login/login.html?back=https%3A//www.newrank.cn/'
wd.get(xinbangLogInUrl)

wd.find_element_by_xpath('/html/body/div[2]/div/div[1]/div[1]/div[2]').click()
wd.find_element_by_xpath('//*[@id="account_input"]').send_keys('nr_953c4lm34') 
wd.find_element_by_xpath('//*[@id="password_input"]').send_keys('xinbang123456')
sleep(10)

wd.find_element_by_xpath('//*[@id="pwd_confirm"]').click()
sleep(1000)#等待Cookies加载

# cookies = wd.get_cookies()
# for cookie in cookies:
#     req.cookies.set(cookie['name'],cookie['value'])