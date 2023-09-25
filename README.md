# PxlsLog
Python script for log extraction from pxls.place pixels logs

</br>

## Disclamer :warning:
**The code is not foolproof at all. If your synthaxe isn't as desired the script will crash or behave randomly !**

</br>

## Setting up the log key file

You need to create a *log_key.txt* file where you have to place your log key as detailled below

```
1  Key_log_1
2  key_log_2
    ...
N  key_log_N
```

## Help to get your placemap/placegraph and do timelapses

Normal behavior prompt
```
Warning : You'll need a 'users_keys.txt' file contaning at least one log key (one per line).     
If multiple log keys are written the placemap will contain the pxls of all the log keys.

Enter .sanit.log path without extention: ./path/to/pxls_place_logs
Enter inital canvas path without extention: ./path/to/initial_canvas
Enter key log file path without extention: ./path/to/log_keys
Enter placemap path+filename without extenion: ./path/to/output_placemap
Do you want to show full canvas (take a very long time) (yes|no): no
Timelapse (yes|no): no
Finished hashing logkey #1 !
...
Finished hashing logkey #N !
Finished hashing !
Finished extract !
Drawing pxls : 100.000%
Finished drawing !
Total number of pixels placed: nb_of_pixels
Total number of pixels: nb_of_pixels_and_undos
```

Timelapse behavior prompt
```
Warning : You'll need a 'users_keys.txt' file contaning at least one log key (one per line).     
If multiple log keys are written the placemap will contain the pxls of all the log keys.

Enter .sanit.log path without extention: ./path/to/pxls_place_logs
Enter inital canvas path without extention: ./path/to/initial_canvas
Enter key log file path without extention: ./path/to/log_keys
Enter placemap path+filename without extenion: ./path/to/output_placemap
Do you want to show full canvas (take a very long time) (yes|no): no
Timelapse (yes|no): yes
Enter timelapse crop area (x,y,x+offx,y+offy): (example: 5,200,500,700)
Enter date & time of timelapse start (%Y-%m-%d %H:%M:%S): (example: 2023-09-25 22:00:00)
Enter duration of timelapse (in sec): (example: 3600)
Enter interval between frames (in sec): (example: 30)
Enter gif filename: ./path/to/output_gif
Finished hashing logkey #1 !
...
Finished hashing logkey #N !
Finished hashing !
Finished extract !
Drawing pxls : 100.000%
Finished drawing !
Total number of pixels placed: nb_of_pixels
Total number of pixels: nb_of_pixels_and_undos
```
