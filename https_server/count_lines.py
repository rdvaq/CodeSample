import os 
from collections import Counter

def count_lines(fp):
    with open(fp, 'r', encoding='utf-8') as f:
        lines = 0
        for line in f:
            line = line.strip()
            if len(line) > 0 and not line.startswith('//'):
                lines += 1
        return lines

def cfile(fp):
    return fp.endswith('.c') or fp.endswith('.h')

def pyfile(fp):
    return fp.endswith('.py')

def bashfile(fp):
    if (fp.find('.') != -1 and not fp.endswith('.sh')) or fp == 'proxy_testing/r2_file':
        return False

    with open(fp, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip().replace(" ", "").lower()
            if len(line) > 0:
                return line == "#!/bin/bash"
    return False # empty file

def makefile(fp):
    return fp.lower().endswith('makefile')

def count_code(dp):
    lines = Counter()
    for entry in os.scandir(dp):
        if entry.is_file():
            if cfile(entry.path):
                lines['c'] += count_lines(entry.path)
            elif pyfile(entry.path) and entry.path != './count_lines.py':
                lines['python'] += count_lines(entry.path)
            elif bashfile(entry.path):
                lines['bash'] += count_lines(entry.path)
            elif makefile(entry.path):
                lines['make'] += count_lines(entry.path)
    return lines
        
def count_proxy():
    return count_code('.')

def count_server():
    return count_code('server')

def count_testing():
    lines = count_code('unit_testing')
    lines.update(count_code('proxy_testing'))
    return lines

def count_experiments():
    return count_code('experiments')

def nprint(name, lines):
    print(name)
    print('\n'.join(f'{lines[k]}\t lines of {k}' for k in sorted(lines.keys())))

def main():
    nprint('proxy implementation:', count_proxy())
    nprint('server implementation:', count_server())
    nprint('testing:', count_testing())
    nprint('experiments:', count_experiments())

if __name__ == '__main__':
    main()