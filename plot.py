import argparse
import matplotlib.pyplot as plt
import numpy as np
from math import sqrt
from matplotlib.colors import LightSource, Normalize
import struct
from matplotlib import colors

def show_data(ax, data, cmap, useLog : bool =False, save_img="", title=""):

    if useLog: data = np.log(data)

    cmap = plt.cm.get_cmap(cmap)
    ls = LightSource(315, 45)
    rgb = ls.shade(data, cmap)

    im = ax.imshow(rgb, interpolation='bilinear')
    if save_img!="":        
        plt.imsave(save_img, rgb)

    # Use a proxy artist for the colorbar...
    im = ax.imshow(data, cmap=cmap)
    im.remove()

    plt.colorbar(im, ax=ax)
    ax.set_title(title, size='x-large')

def load_data(fname):

    with open(fname, "rb") as ifile:
        W = struct.unpack('i', ifile.read(4))[0]
        H = struct.unpack('i', ifile.read(4))[0]
        data = np.frombuffer(buffer=ifile.read(), dtype=np.float64)
        return data.reshape(H, W)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('fname', type=str, help="Input file")
    parser.add_argument('--cmap', type=str, default="jet", help="Colormap")
    parser.add_argument('--log', help="Use log scale", action='store_true')
    parser.add_argument('--save_img', help="Save image", default="")
    FLAGS, unparsed_args = parser.parse_known_args()
    if len(unparsed_args):
        print("Warning: unknow arguments {}".format(unparsed_args))

    data = load_data(fname=FLAGS.fname) 

    fig = plt.figure()
    fig.set_size_inches((14, 6))
    show_data(ax=plt.gca(), data=data, cmap=FLAGS.cmap, title="Fractal", useLog=FLAGS.log, save_img=FLAGS.save_img)
    fig.tight_layout()
    plt.show()
