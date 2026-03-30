from PIL import Image
import os
import math


newWidth = 512

c = os.getcwd()
files = os.listdir(c)
files = [f for f in files if f.split(".")[-1]=="png"]

if not os.path.exists("./scaled"):
    os.makedirs("./scaled")


for f in files:
    img = Image.open(f)
    w = img.width
    h = img.height
    scale = newWidth / w
    print(scale, w*scale, h*scale)
    img = img.resize((math.floor(w*scale), math.floor(h*scale)))
    img.save(f"./scaled/{f}")
