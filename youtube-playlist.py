import argparse
import subprocess
import os

parser = argparse.ArgumentParser()
parser.add_argument("url")
subprocess.run(['youtube-dl', '-o', "%(playlist_index)s-%(title)s.%(ext)s", '-f', 'bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best', '--download-archive', 'archive.txt', parser.parse_args().url])

#ds= youtube-dl -o "%(playlist_index)s-%(title)s.%(ext)s" -f 'bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best' --download-archive archive.txt <playlist_link>
