import os
import requests
import bs4
import re
from urllib.parse import urljoin
from datetime import datetime

def filename_safe(filename):
    keepcharacters = (' ','.','_')
    filename = "".join(c for c in filename if c.isalnum() or c in keepcharacters).rstrip()
    return filename

def filename_from_request(r, url='', noblank=False):
    filename = ''
    if "Content-Disposition" in r.headers.keys():
        filename = re.findall("filename=(.+)", r.headers["Content-Disposition"])[0]
    elif url != '':
        filename = url.split("/")[-1]
    if noblank and filename == '':
        filename = str(datetime.now())
    filename = filename_safe(filename)
    return filename

def relative_to_absolute_url(baseurl, url):
    return_url = urljoin(baseurl, url)
    return return_url

def download_webpages(urls, name_dict={}, output_dir='./'):
    if output_dir[-1] != '/':
        output_dir += '/'
    image_dict = {}
    js_dict = {}
    css_dict = {}
    os.makedirs(output_dir+'link', exist_ok=True)
    os.makedirs(output_dir+'script', exist_ok=True)
    os.makedirs(output_dir+'img', exist_ok=True)
    for url in urls:
        req = requests.get(url)
        req.raise_for_status()
        soup = bs4.BeautifulSoup(req.text, 'html.parser')
        for img in soup.find_all('img'):
            href = img.get('src')
            if not href.startswith('http'):
                href = relative_to_absolute_url(url, href)
            if href not in image_dict:
                #download image
                res_req = requests.get(href)
                res_req.raise_for_status()
                filename = filename_from_request(res_req, href, True)
                filename = filename.replace(' ', '_')
                playFile = open(output_dir+'img/'+filename,'wb')
                for chunk in res_req.iter_content(100000):
                    playFile.write(chunk)
                playFile.close()
                image_dict[href] = filename
            img['src'] = 'img/'+image_dict[href]

        for link in soup.find_all('link', rel="stylesheet"):
            href = link.get('href')
            if not href.startswith('http'):
                href = relative_to_absolute_url(url, href)
            if href not in css_dict:
                #download
                res_req = requests.get(href)
                res_req.raise_for_status()
                filename = filename_from_request(res_req, href, True)
                filename = filename.replace(' ', '_')
                playFile = open(output_dir+'link/'+filename,'wb')
                for chunk in res_req.iter_content(100000):
                    playFile.write(chunk)
                playFile.close()
                css_dict[href] = filename
            link['href'] = 'link/'+css_dict[href]
        for script in soup.find_all('script', src=True):
            href = script.get('src')
            if not href.startswith('http'):
                href = relative_to_absolute_url(url, href)
            if href not in js_dict:
                #download 
                res_req = requests.get(href)
                res_req.raise_for_status()
                filename = filename_from_request(res_req, href, True)
                filename = filename.replace(' ', '_')
                playFile = open(output_dir+'script/'+filename,'wb')
                for chunk in res_req.iter_content(100000):
                    playFile.write(chunk)
                playFile.close()
                js_dict[href] = filename
            script['src'] = 'script/'+js_dict[href]
        if url in name_dict:
            filename = name_dict[url]
        else:
            filename = filename_from_request(req, url, True)
        playFile = open(output_dir+filename,'wb')
        playFile.write(soup.prettify('utf8'))
        playFile.close()

