import os

def main():
    with open('full.html', 'w') as fp:
        fp.write('<html>\n')
        for root, dirs, files in os.walk('.'):
            for file in files:
                name = (root.replace('\\','/')+'/'+file)
                fp.write('<a href="{}">{}</a><br />\n'.format(name, name))
                # print(root.replace('\\','/')+'/'+file)
        fp.write('</html>')


def generate_index(root='.', types=None):
    prev_root = (os.getcwd())
    os.chdir(root)
    fp = open('index.html', 'w')
    fp.write('<html>\n')
    fp.write('<a href="../">&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt&lt</a><br />\n')
    with os.scandir('.') as it:
        for entry in it:
            if entry.is_file() and ((entry.name[entry.name.rfind('.')+1:] in types) or (types == None)):
                fp.write('<a href="{}">{}</a><br />\n'.format(entry.name, entry.name))
                print(entry.path)
            elif entry.is_dir():
                fp.write('<a href="{}">{}</a><br />\n'.format(entry.name, entry.name))
                print(entry.path)
    fp.write('</html>')
    fp.close()
    os.chdir(prev_root)

def generate_index_recurse(root='.', types=None):
    prev_root = (os.getcwd())
    dirs = []
    with os.scandir(root) as it:
        for entry in it:
            if entry.is_dir():
                dirs.append(entry.path)
    generate_index(root, types)
    for folder in dirs:
        generate_index_recurse(folder, types)


generate_index_recurse('.', ['html', 'htm', 'mp3'])
main()
print('done')
