# FractalsGen

This program generates beautiful fractal images using the following functions:

- Family00:
     ```math
        z = x^2(1+Re(Q)z^2)/(1-Re(Q)z^2) + C
    ```
- Family01:
     ```math
        z = x^2 + C
    ```
- Family02:
     ```math
        z = x^n + C
    ```
- Family03 (Newton method):
     ```math
        z = z - \frac{x^n + C}{nx^{n-1}}
     ```
- Family04 (Newton method):
     ```math
        z = z - \frac{x^n + Qz + C}{nx^{n-1}+Q}
     ```
Actually, some functions are special cases of other functions, for example, `Family01` is a special case of the function `Family00` when ```Re(Q) = 0```, this is so for optimization purposes. You can change the different parameters to get very impressive images.

## How to build de app?
Just use QtSDK-6.4.* and open [qtapp/CMakeLists.txt](qtapp/CMakeLists.txt) with QtCreator tool. It doesn't need any external. 

## plot.py script 

You can save the raw data from the `FractalGen` application and use the [plot.py](./plot.py) script to display the fractal using Matplotlib's palettes.
```
usage: plot.py [-h] [--cmap CMAP] [--log] [--save_img SAVE_IMG] fname

positional arguments:
  fname                Input file

options:
  -h, --help           show this help message and exit
  --cmap CMAP          Colormap
  --log                Use log scale
  --save_img SAVE_IMG  Save image
```


## Gallery
FractalGen:
![](./app.jpg?raw=true "FractalGen") 

Some examples:
![](./sample00.jpg?raw=true) 
![](./sample01.jpg?raw=true) 
![](./sample02.jpg?raw=true) 
![](./sample03.jpg?raw=true) 
![](./sample04.jpg?raw=true) 
![](./sample05.jpg?raw=true) 
![](./sample06.jpg?raw=true) 
## TO DO:
- Add other color maps
- Add new fractals