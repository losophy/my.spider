from selenium import webdriver
from requests import Session
from time import sleep

chromePath = r'C:\Program Files (x86)\Google\Chrome\Application\chromedriver.exe'
chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--headless')

driver = webdriver.Chrome(executable_path=chromePath,chrome_options=chrome_options) 

loginUrl = 'https://www.newrank.cn/public/login/login.html?back=https%3A//www.newrank.cn/'
driver.get(loginUrl)

driver.find_element_by_xpath('/html/body/div[2]/div/div[1]/div[1]/div[2]').click()
driver.find_element_by_xpath('//*[@id="account_input"]').send_keys('nr_953c4lm34') 
driver.find_element_by_xpath('//*[@id="password_input"]').send_keys('xinbang123456')
sleep(1)

driver.find_element_by_xpath('//*[@id="pwd_confirm"]').click()
sleep(1)

loginUrl = 'https://www.newrank.cn/public/info/search.html?value=if_fashion888&isBind=false'
driver.get(loginUrl)

text = driver.find_element_by_xpath('//*[@id="show_0AE34266BC626F9D134FC618776AE30A"]/span[1]').text
print(text)

driver.close()





