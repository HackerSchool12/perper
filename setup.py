from distutils.core import setup, Extension
from Cython.Distutils import build_ext

permap = Extension(
    'permap',
    sources = ['hashmap.c', 'object.c', 'pyext.c', 'permap.pyx']) 

setup (name = 'persistenthashmap',
       version = '1.0',
       description = 'This is a persistent hashmap package',
       cmdclass = {'build_ext': build_ext},
       ext_modules = [permap])
