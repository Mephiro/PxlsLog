# PxlsLog
Python and c++ scripts for log extraction from pxls.place pixels logs

### Timelapse example from [C72 canvas](https://archives.pxls.space/canvas-72) on osu! faction

![osuExample](python_pxlslog/osuExample.gif)

</br>

## Disclamer :warning:
**The code is not foolproof at all. If your synthaxe isn't as desired the script will crash or behave randomly !**

</br>

## Setting up the log key file

You need to create a *log_key.txt* file where you have to place your log key as detailled below :
You can place multiple log keys if you want to merge placemap of different users

```
1  Log_key_1	(One log key per line)
2  Log_key_2
3  #00FF00	(Allow to set the color of the following log_key to #00FF00 (green) on the heatmap,
		without #color the default #FF0000 is used)
4  Log_key_3
    ...
N  Log_key_N
```

## Help to get your placemap/placegraph and do timelapses

See in ![pythonPxlslog](python_pxlslog/) or ![cppPxlslog](cpp_pxlslog/) !
