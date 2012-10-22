from distutils.core import setup, Extension
import os, sys

def local_path(path):
    local_dir = os.path.dirname(__file__)
    return os.path.normpath(os.path.join(local_dir, path))

def parse_version_from_c():
    cfile = open(local_path('../src/bloom.c'))
    result = ''
    for line in cfile:
        parts = line.split()
        if len(parts) == 3 and parts[:2] == ['#define', 'BLOOMIN_VERSION']:
            result = parts[2].strip('"')
            break
    cfile.close()
    return result

def path_from_env(name, default):
    return os.environ.get(name, local_path(default))

module1 = Extension('pybloomin', 
                    include_dirs = [local_path('../src')],
                    extra_objects = [os.path.join(path_from_env('BLDDIR', '../build'), 'libbloomin.a')],
                    sources = [local_path('pybloomin.c')],
                   )

setup (name = 'pybloomin',
    version = parse_version_from_c(),
    description = 'This is a a python extension of a counting bloom filter. Many thanks to Dablooms for inspiration!',
    author = 'Corey Lynch',
    author_email = 'coreylynch9@gmail.com',
    url = 'http://github.com/coreylynch/bloomin.git',
    ext_modules = [module1])
