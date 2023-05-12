import subprocess
import os
import time
import sys

START_PROXY_TEMPLATE = '''#!/bin/bash
        
rm a.out
make a.out
./a.out 9086 {0}
'''

START_PROXY_PAUSE_TIME = 8


def debug(str):
    print(str, file=sys.stderr, flush=True)


def load_http_urls(urls):
    with open('http_urls', 'r', encoding='utf-8') as f:
        urls.extend(line.strip() for line in f)


def load_https_urls(urls):
    with open('https_urls', 'r', encoding='utf-8') as f:
        urls.extend(line.strip() for line in f)


def convert_to_sec(unix_time):
    unix_time = unix_time.strip()
    min_sec_split = unix_time.index(':')
    sec_ms_split = unix_time.index('.')
    min = int(unix_time[:min_sec_split])
    sec = int(unix_time[min_sec_split + 1:sec_ms_split])
    ms = float(unix_time[sec_ms_split:])
    return min * 60 + sec + ms


def time_command(command):
    return convert_to_sec(subprocess.run(['/usr/bin/time', '-f', '%E'] + command.split(' '), capture_output=True, encoding='utf-8').stderr)


def curl_proxy_command(resource):
    return f'curl -s -x localhost:9086 {resource} -o out_files/proxy_response'


def curl_server_command(resource):
    return f'curl -s {resource} -o out_files/server_response'


def curl_proxy(resource):
    subprocess.run(curl_proxy_command(resource).split(' '))


def curl_server(resource):
    subprocess.run(curl_server_command(resource).split(' '))


def time_proxy(resource):
    return time_command(curl_proxy_command(resource))


def time_server(resource):
    return time_command(curl_server_command(resource))


def save_list(ls, filename, title=None, format='sep_lines'):
    with open(os.path.join('results', filename), 'w+', encoding='utf-8') as f:
        if format == 'sep_lines':
            f.write(f'{title}\n' + '\n'.join(str(e) for e in ls))
        elif format == 'single_line':
            f.write(', '.join(str(e) for e in ls) + '\n')
            


def setup_proxy(non_port_settings):
    non_port_settings = non_port_settings.strip()
    with open(os.path.join('..', 'startproxy'), 'w+', encoding='utf-8') as f:
        f.write(START_PROXY_TEMPLATE.format(non_port_settings))
    debug(
        f'set non-port proxy settings: {non_port_settings}\nsleeping {START_PROXY_PAUSE_TIME}s to startproxy...')
    time.sleep(START_PROXY_PAUSE_TIME)


def prompt_proxy_restart():
    debug(f'sleeping {START_PROXY_PAUSE_TIME}s to restart proxy...')
    time.sleep(START_PROXY_PAUSE_TIME)
