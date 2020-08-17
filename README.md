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
Just use QtSDK-5.12.* and open CMakeList.txt with QtCreator tool. It doesn't need any external. For visualize the fractal I used QCustomPlot [[here]](https://www.qcustomplot.com/). 


## Tricks:
 - Use mouse button and scroll wheel to navigate
 - In most cases the fractal is generated almost in real time. To increase the speed you can select the option `Fast Render` or `Very Fast Render` 
 - When you get a good image you can save the parameters you used and generate an image at the resolution you want
 
## Gallery
FractalGen:
![](./app.png?raw=true "FractalGen") 

Some examples:
![](./gallery.png?raw=true "Some examples") 

## TO DO:
- Code style refactoring, out there is a mess :) 
- Add other color maps
- General revision, correction of some bugs
- Add new fractals