import argparse
import subprocess
import os
import shutil
import sys
from datetime import datetime

log_list = []
def log(message):
  message = "{}: {}\n".format(datetime.now(), message)
  print(message)
  log_list.append(message)

def convert(input, outputpath, outputfile):
  #os.makedirs(outputpath,exist_ok=True)
  output = outputpath+"\\"+outputfile+".mp4"
  if (input[-3:].lower() == "mkv"):
    log("starting "+input)
    x = subprocess.run(['ffmpeg','-i',input,'-codec','copy',output])
  elif (input[-3:].lower() == "mp4"):
    if input != output:
      log("copying: {}\nto {}".format(input,output))
      shutil.copy2(input, output)
      log("copied: {} \nto {}".format(input,output))
      log("deleting: {}".format(input))
      os.remove(input)
      log("deleted: {}".format(input))
    return
  else :
    log("starting "+input)
    x = subprocess.run(['ffmpeg','-i',input,output])
  log("input: {}".format(input))
  log("output: {}".format(output))
  log("return code: {}".format(x.returncode))
  if x.returncode == 0:
    os.remove(input)
    log("deleted: {}".format(input))

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("-b","-p","--basepath")
  parser.add_argument("-w","--walk",action="store_true")
  parser.add_argument("-t","--type")
  args = parser.parse_args()
  if(args.basepath):
    path = args.basepath
  else:
    path = os.getcwd()
  log(str(sys.argv))
  log(path)
  if(not args.walk):
    with os.scandir(path) as entries:
      for entry in entries:
        try:
          if entry.is_file():
            if(args.type):
              if(args.type.lower() != entry.name[entry.name.rindex(".")+1:].lower()):
                log("Skipping {}".format(entry.name))
                continue
            convert(path+"\\"+entry.name, path, entry.name[:entry.name.rindex(".")])
        except KeyboardInterrupt:
          raise
        except:
          log(sys.exc_info()[0])
  else:
    for(dirpath,dirnames,filenames) in os.walk(path):
      for filename in filenames:
        try:
          if(args.type):
            if(args.type.lower() != filename[filename.rindex(".")+1:].lower()):
              log("Skipping {}".format(filename))
              continue
          convert(dirpath+"\\"+filename, dirpath, filename[:filename.rindex(".")])
        except KeyboardInterrupt:
          raise
        except:
          log(sys.exc_info()[0])
  log("done")
  
try:
  main()
except:
  log(sys.exc_info()[0])
logfile = "log_{}.txt".format(datetime.now())
logfile = logfile.replace(':','-')
with open(logfile, 'w') as out_file:
  out_file.writelines(log_list)
    
