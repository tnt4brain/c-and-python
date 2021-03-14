#!/usr/bin/python3
#
# Copyright 2021 Sergey Pechenko
#
# Python code that gets called from within C program
#


import pprint
import math


def main():
  print("reOpenLDAP Python hook file")
  print("For in-app import only")
  exit(1)

def add_hook(*args, **kwargs):
  with open("/tmp/res.txt",'a',newline='\n') as f: 
    f.write("ADD HOOK\n")
    f.write(pprint.pformat(args));
    f.close()
  return pprint.pformat(args)

def bind_hook(*args, **kwargs):
  with open("/tmp/res.txt",'a',newline='\n') as f:
    f.write("BIND HOOK\n")
    f.write(*args)
  return None

def unbind_hook(*args, **kwargs):
  return set(['rock','paper','scissors'])

def compare_hook(*args, **kwargs):
  return ['Compare_hook', 'зачем-то вернула', 'обычный список']

def delete_hook(*args, **kwargs):
  return True

def modify_hook(*args, **kwargs):
  return ('strange','from modify_hook')

def modrdn_hook(*args, **kwargs):
  return pprint

def search_hook(*args, **kwargs):
  return 'வணக்கம், தளம் "ஹப்ராஹப்ர்".'

def abandon_hook(*args, **kwargs):
  return {'"тук-тук, кто там?!"': '"tnt4brain на Хабре"'}

def extended_hook(*args, **kwargs):
  return math.pi

def response_hook(*args, **kwargs):
  return 100500

if __name__ == "__main__":
  main()
