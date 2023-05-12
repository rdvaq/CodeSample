import experiment_utils as eu
import os
import subprocess
import time

http_urls = list()
https_urls = list()


def experiment1_client(proxy_time_filename, save_urls_and_server, title):
    eu.curl_proxy(http_urls[2])
    num_resources = 10
    urls = [url for url in http_urls[3: 3 + num_resources]]
    if save_urls_and_server:
        eu.save_list(urls, os.path.join('experiment1', 'urls.txt'), 'resource')
        eu.curl_server(http_urls[2])
        eu.save_list([eu.time_server(url) for url in urls],
                     os.path.join('experiment1', 'server.txt'), 'server time')
    eu.save_list([eu.time_proxy(url) for url in urls],
                 os.path.join('experiment1', proxy_time_filename), title)


def experiment1():
    """
    To replicate experiment 1: 
    1. Uncomment experiment1() in main()
    2. Comment 3rd and 4th lines and follow prompt
    3. Run this file
    4. Comment 1st and 2nd lines and follow prompt
    5. Run this file
    """

    # eu.setup_proxy('10 banned_hosts.txt 1 0 0 0')
    # experiment1_client('unused_0.txt', True, 'css time')
    eu.setup_proxy('10 banned_hosts.txt 1 0 0 30')
    experiment1_client('unused_30.txt', False, 'suss time')


def experiment2():
    """
    To replicate experiment 2: 
    Uncomment experiment2() in main() and run this file (pay attention to prompts)
    """

    eu.save_list([eu.time_command('./experiment2_curls')],
                 os.path.join('experiment2', 'server.txt'), 'server time')
    eu.setup_proxy('120 banned_hosts.txt 1 0 0 0')
    subprocess.run(['rm', 'experiment2.out'])
    subprocess.run(['make', 'experiment2.out'])
    eu.save_list([eu.time_command('./experiment2.out')],
                 os.path.join('experiment2', 'proxy.txt'), 'proxy time')


def experiment3():
    """
    To replicate experiment 3:
    Uncomment experiment3() in main() and run this file (pay close attention to prompts - will have to restart
    server multiple times)
    """

    eu.setup_proxy('100 banned_hosts.txt 1 0 1 0')
    sizes = [10, 20, 40]
    trials = 5
    trial_sizes = list()
    proxy_times = list()
    server_times = list()
    for size in sizes:
        for t in range(trials):
            eu.debug(f'trial {t} for size {size}')
            trial_sizes.append(size)
            server_times.append(eu.time_server(http_urls[2]))
            for i in range(3, 3 + size):
                eu.curl_proxy(http_urls[i])
            proxy_times.append(eu.time_proxy(http_urls[2]))
            eu.prompt_proxy_restart()
    eu.save_list(trial_sizes, os.path.join(
        'experiment3', 'modifications.txt'), 'modifications')
    eu.save_list(server_times, os.path.join(
        'experiment3', 'server.txt'), 'server time')
    eu.save_list(proxy_times, os.path.join(
        'experiment3', 'proxy.txt'), 'proxy time')


def experiment4():
    """
    To replicate experiment 4: 
    Uncomment experiment4() in main() and run this file (pay attention to prompts)
    """

    eu.setup_proxy('10 banned_hosts.txt 1 0 0 0')
    server_times = list()
    proxy_times = list()
    urls = list()
    trials = 5
    for url in https_urls[:10]:
        for _ in range(trials):
            urls.append(url)
            server_times.append(eu.time_server(url))
            proxy_times.append(eu.time_proxy(url))
    eu.save_list(urls, os.path.join(
        'experiment4', 'urls.txt'), 'url')
    eu.save_list(server_times, os.path.join(
        'experiment4', 'server.txt'), 'server time')
    eu.save_list(proxy_times, os.path.join(
        'experiment4', 'proxy.txt'), 'proxy time')


def experiment5():
    """
    To replicate experiment 5:
    Uncomment experiment5() in main() and run this file (pay close attention to prompts - will have to restart
    server multiple times)
    """

    sizes = [10, 20, 40, 80]
    all_sizes = list()
    hit_times = list()
    miss_times = list()
    server_hit_times = list()
    server_miss_times = list()
    for size in sizes:
        eu.debug(f'size {size}')
        eu.setup_proxy(f'{size} banned_hosts.txt 1 0 0 0')
        for url in http_urls[:size]:
            eu.curl_proxy(url)
        for url in http_urls[:5]:
            hit_times.append(eu.time_proxy(url))
            server_hit_times.append(eu.time_server(url))
            all_sizes.append(size)
        for url in http_urls[size:size + 5]:
            miss_times.append(eu.time_proxy(url))
            server_miss_times.append(eu.time_server(url))
    out_lists = [sizes, all_sizes, hit_times,
                 miss_times, server_hit_times, server_miss_times]
    out_files = ['sizes.txt', 'all_sizes.txt', 'hit_times.txt',
                 'miss_times.txt', 'server_hit_times.txt', 'server_miss_times.txt']
    out_titles = ['size', 'size', 'hit time', 'miss time',
                  'server time (hit)', 'server time (miss)']
    for ol, of, ot in zip(out_lists, out_files, out_titles):
        eu.save_list(ol, os.path.join('experiment5', of), ot)


def experiment6_make_lists():
    sizes = [10, 20, 40, 80]
    for i, size in enumerate(sizes):
        with open(os.path.join('banlists', f'list{i+1}.txt'), 'w+', encoding='utf-8') as f:
            for s in range(size):
                f.write(f'www.badwebsite{s+1}.com\n')


def experiment6():
    """
    To replicate experiment 6:
    Uncomment experiment6() in main() and run this file (pay close attention to prompts - will have to restart
    server multiple times)
    """

    # experiment6_make_lists()
    trials = 5
    sizes = list()
    times = list()
    urls = list()
    for fn, size in zip(['list1.txt', 'list2.txt', 'list3.txt', 'list4.txt'], [10, 20, 40, 80]):
        for t in range(trials):
            eu.debug(f'list {fn} trial {t}')
            eu.setup_proxy(
                f'10 {os.path.join("experiments", "banlists", fn)} 1 0 0 0')
            for url in http_urls[:5]:
                sizes.append(size)
                times.append(eu.time_proxy(url))
                urls.append(url)
    eu.save_list(sizes, os.path.join('experiment6', 'sizes.txt'), 'size')
    eu.save_list(times, os.path.join('experiment6', 'times.txt'), 'time')
    eu.save_list(urls, os.path.join('experiment6', 'urls.txt'), 'resource')


def experiment7():
    """
    To replicate experiment 7:
    Uncomment experiment7() in main() and run this file (pay close attention to prompts - will have to restart
    server multiple times)
    """

    trials = 5
    urls = list()
    no_limit_times = list()
    eu.setup_proxy(f'10 banned_hosts.txt 1 0 0 0')
    for url in http_urls[:3]:
        for t in range(trials):
            eu.debug(f'url {url} trial {t}')
            urls.append(url)
            no_limit_times.append(eu.time_proxy(url))
            eu.prompt_proxy_restart()
    limit_times = list()
    eu.setup_proxy(f'10 banned_hosts.txt 20000 1 0 0')
    for url in http_urls[:3]:
        for t in range(trials):
            eu.debug(f'url {url} trial {t}')
            limit_times.append(eu.time_proxy(url))
            eu.prompt_proxy_restart()
    eu.save_list(no_limit_times, os.path.join(
        'experiment7', 'no_limit_times.txt'), 'time')
    eu.save_list(limit_times, os.path.join(
        'experiment7', 'limit_times.txt'), 'time')
    eu.save_list(urls, os.path.join('experiment7', 'urls.txt'), 'resource')

def stresstest1(num_sockets):
    subprocess.run(['rm', 'connect_ddos.out'])
    subprocess.run(['make', 'connect_ddos.out'])
    trials = 5
    times = [eu.time_command(f'./connect_ddos.out {num_sockets}') for _ in range(trials)]
    eu.save_list(times, os.path.join('stresstest1', 'times.txt'), format='single_line')

def stresstest2():
    # Use limited proxy!!!
    trials = 5
    # urls = list()
    # no_limit_times = list()
    # for t in range(trials):
    #     for url in http_urls[:3]:    
    #         eu.debug(f'url {url} trial {t}')
    #         urls.append(url)
    #         no_limit_times.append(eu.time_proxy(url))
    #     eu.prompt_proxy_restart()
    # eu.debug(f'open new terminal to start ./spam_dos.out, sleeping {eu.START_PROXY_PAUSE_TIME}s')
    # time.sleep(eu.START_PROXY_PAUSE_TIME)
    limit_times = list()
    for t in range(trials):
        for url in http_urls[:3]:
            eu.debug(f'url {url} trial {t}')
            limit_times.append(eu.time_proxy(url))
        eu.debug(f'restart proxy and ./spam_dos.out, sleeping {eu.START_PROXY_PAUSE_TIME}s')
        time.sleep(eu.START_PROXY_PAUSE_TIME)
    # eu.save_list(no_limit_times, os.path.join(
    #     'stresstest2', 'no_limit_times.txt'), 'time')
    eu.save_list(limit_times, os.path.join(
        'stresstest2', 'limit_times.txt'), 'time')
    # eu.save_list(urls, os.path.join('stresstest2', 'urls.txt'), 'resource')

def main():
    # Always leave this uncommented
    eu.load_http_urls(http_urls)
    eu.load_https_urls(https_urls)
    # experiment1()
    # experiment2()
    # experiment3()
    # experiment4()
    # experiment5()
    # experiment6()
    # experiment7()
    # stresstest1(700)
    stresstest2()


if __name__ == '__main__':
    main()
