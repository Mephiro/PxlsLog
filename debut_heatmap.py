#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 28 01:25:14 2023

@author: mephir0
"""

from PIL import Image, ImageDraw
from random import randint


def brightness(img,coef):
    source = img.split()
    Red = source[0].point(lambda i: i*coef)
    Green = source[1].point(lambda i: i*coef)
    Blue = source[2].point(lambda i: i*coef)
    Alpha = source[3]
    return Image.merge(img.mode,(Red,Green,Blue,Alpha))


im = Image.open("canvas_initial.png")


im.show()

im = brightness(im, 0.1)

im2 = im.copy()

draw = ImageDraw.Draw(im)
for i in range(1000):
    draw.point((randint(0,1000),randint(0,1000)),'#FF0000')

im.show()

im.paste(im2)
im.show()
