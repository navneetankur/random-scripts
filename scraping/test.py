# since filenames were created deterministacially we can get the url corres to it by repeating the process used to generate filename. from that url we can construct the full path of images and download them.


import os
from urllib.parse import urljoin
from urllib.parse import urlsplit
from urllib.parse import urlunsplit
import requests
import mylib
from bs4 import BeautifulSoup
import re

baseurl = 'https://codelabs.developers.google.com'
def one(path, filename):
    url = 'https://codelabs.developers.google.com/android-kotlin-fundamentals/'
    url = baseurl + path
    res = requests.get(url)
    res.raise_for_status()
    filename = mylib.make_safe_filename(filename)
    playFile = open('download/'+filename,'wb')
    for chunk in res.iter_content(100000):
        playFile.write(chunk)
    playFile.close()

def two():
    soup = None
    with open("android-kotlin-fundamentals.html") as fp:
        soup = BeautifulSoup(fp, 'lxml')
    a_list = soup.find_all('a', href=re.compile('codelabs'))
    filename_dict = {}
    for item in a_list:
        hr = (item.get('href'))
        filename = (item.find(class_='description').string + '.html')
        # one(hr, filename)
        url = (baseurl+hr)
        print()
        filename_dict[url] = mylib.filename_safe(filename)
        # print(item.find(class_='description').string)
    # print(a_list)
    mylib.download_webpages(filename_dict.keys(), filename_dict, 'scrape')

def four():
    soup = None
    with open("android-kotlin-fundamentals.html") as fp:
        soup = BeautifulSoup(fp, 'html.parser')
    a_list = soup.find_all('a', href=re.compile('codelabs'))
    for item in a_list:
        hr = (item.get('href'))
        filename = (item.find(class_='description').string + '.html')
        print(filename)
        url = (baseurl+hr)
        a = urlsplit(url)
        # a.query = ''
        a = (a._replace(query=''))
        url = urlunsplit(a)
        print(url)
        print()

def three():
    i = 0
    sourcepath = './download'
    destpath = sourcepath+'/a3/'
    soup = None
    for entry in os.listdir(sourcepath):
        filepath = os.path.join(sourcepath, entry)
        if os.path.isfile(filepath):
            print(entry)
            with open(filepath, 'r') as fp:
                soup = BeautifulSoup(fp, 'html.parser')
            img_list = soup.find_all('img')
            for img in img_list:
                src = img.get('src')
                print(src)
                if src[:4] != 'http':
                    print('relative')
                else:
                    print('absolute')
                # img['src'] = 'image{}'.format(i)
            # with open(destpath+entry, 'w') as fp:
            #     fp.write(soup.prettify())



two()
print()
print("done")
