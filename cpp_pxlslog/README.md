# PxlsLog
c++ script for log extraction from pxls.place pixels logs

### Timelapse example from [C73 canvas](https://archives.pxls.space/canvas-73) on full canvas

https://github.com/Mephiro/PxlsLog/assets/37735944/a2134c2e-5618-4ce1-996c-5dd560408316

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

Normal behavior prompt
```
user:~$ ./Pxlslog

┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃ ██████╗ ██╗  ██╗██╗     ███████╗██╗      ██████╗  ██████╗  ┃
┃ ██╔══██╗╚██╗██╔╝██║     ██╔════╝██║     ██╔═══██╗██╔════╝  ┃
┃ ██████╔╝ ╚███╔╝ ██║     ███████╗██║     ██║   ██║██║  ███╗ ┃
┃ ██╔═══╝  ██╔██╗ ██║     ╚════██║██║     ██║   ██║██║   ██║ ┃
┃ ██║     ██╔╝ ██╗███████╗███████║███████╗╚██████╔╝╚██████╔╝ ┃
┃ ╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝ ╚═════╝  ╚═════╝  ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛


Enter the userkey path to file (default : userkey.txt): ./path/to/log_keys.txt
Enter the pxls log path to file (default : pixels.log): ./path/to/pxls_place_logs.log
Enter the palette path to file (default : palette.txt): ./path/to/palette.txt
Enter the background image path to file (default: canvas-initial.png): ./path/to/initial_canvas.png
Enter your timezone (default: CEST): UTC+2
Draw all the pixels ? (yes|no*): 
Timelapse ? (yes|no*): no
Overlay with initial template ? (yes|no*): 

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━


Canvas created!
Brush created!
Palette created!
Extracting pxls in progress...
Extracting pxls done!
Pixel total canvas: nb_of_total_pixels | Undo total canvas: nb_of_total_undos
Pixel user(s): nb_of_users_pixels | Undo user(s): nb_of_users_undos
Drawing in progress...
Progress :100%
Drawing finished!


Press Esc to close.
```

Timelapse behavior prompt
```
user:~$ ./Pxlslog

┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃ ██████╗ ██╗  ██╗██╗     ███████╗██╗      ██████╗  ██████╗  ┃
┃ ██╔══██╗╚██╗██╔╝██║     ██╔════╝██║     ██╔═══██╗██╔════╝  ┃
┃ ██████╔╝ ╚███╔╝ ██║     ███████╗██║     ██║   ██║██║  ███╗ ┃
┃ ██╔═══╝  ██╔██╗ ██║     ╚════██║██║     ██║   ██║██║   ██║ ┃
┃ ██║     ██╔╝ ██╗███████╗███████║███████╗╚██████╔╝╚██████╔╝ ┃
┃ ╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝ ╚═════╝  ╚═════╝  ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛


Enter the userkey path to file (default : userkey.txt): ./path/to/log_keys.txt
Enter the pxls log path to file (default : pixels.log): ./path/to/pxls_place_logs.log
Enter the palette path to file (default : palette.txt): ./path/to/palette.txt
Enter the background image path to file (default: canvas-initial.png): ./path/to/initial_canvas.png
Enter your timezone (default: CEST): (ex: UTC+2)
Draw all the pixels ? (yes|no*): 
Timelapse ? (yes|no*): yes
Overlay with initial template ? (yes|no*): 
Enter the crop area for timelapse (example : X0 X0+Xoff Y0 Y0+Yoff): (enter 0 0 0 0 for full canvas)
Heatmap ? (yes|no*): 
Enter timelapse starting date (example : YYYY-MM-DD HH:MM:SS): (ex: 2023-09-24 00:00:00)
Enter timelapse duration in seconds: (ex: 2592000)
Enter frame interval in seconds: (ex: 3600)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━


Canvas created!
Brush created!
Palette created!
Extracting pxls in progress...
Extracting pxls done!
Pixel total canvas: nb_of_total_pixels | Undo total canvas: nb_of_total_undos
Pixel user(s): nb_of_users_pixels | Undo user(s): nb_of_users_undos
Drawing in progress...
Progress :100%
Drawing finished!


Press Esc to close.
```
