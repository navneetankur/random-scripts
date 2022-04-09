import os
import sys
import shutil
from datetime import datetime

if len(sys.argv) > 1:
    dest = os.argv[1]
else:
    dest = str(datetime.now()).replace(":",'-')+".md"
outputList = ["# index of {}\n\n".format(dest)]
for (dirpath, dirnames, filenames) in os.walk("."):
    for filename in filenames:
        outputList.append("#. **{}** : {}\n".format(filename, dirpath))

with open(dest, 'w') as file:
    file.writelines(outputList)
print("Output written to {}".format(dest))