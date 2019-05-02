# Grep from Ed
In this project I try to create the grep command from ed source code just like how Ken Thompson did in one evening
## Installing

First update apt
```
sudo apt-get update
```

Make sure you have `gcc` installed.
```
sudo apt-get install gcc
```
Make user you have 'make` installed
```
sudo apt-get install make
```
Then simply grab the files and place them all in the same directory then type
```
make
```
and you should have an exectuable .elf file.

## How to use it
The regular expression handling was taken from ed along with printing and file editing. To use it just type
```
ed_from_grep search file
```

# Source Code Explanation
The challenge with this was figuring out what everything did because the names for variables and functions was not descriptive at all. 
Learning how to use GDB really helped with seeing how everything works sequentially. In the original source code there is an
`unsigned int*` called `zero` and this is the key to understanding the whole program. A file is first read from
`void filename(int comm)` and saved into `file` and then opened. When `append` is called it calls `putline` which writes
a line from the file to a buffer and returns `tline` which is equal to this: `tline += (((lp-linebuf)+03)>>1)&077776;`.
This is basically taking the length of a line from the file adding 3 then dividing by 2 then making sure it is even
and not past a max. This number is stored in `zero` and the same thing is done for each line. In my sample file
`csgo_commands` the values I got from `zero` were "0, 2, 8, 26, 38, 50, 50, 58, 70, 82, 92, 104, 114". These values 
are later decoded from a call to `print` which calls `_getline` which calls `getblock` and is get block the line 
with the line `(atl<<1) & (BLKSIZE-1) & ~03` is present which divides by 2, subtracts 3, and does not allow the value
past a max. With this value it can refer to a line that is stored in `zero`. Another thing to know is `dol` is `$` as
in the end and `dot` is `.` which is what you are currently on. Both of these variables point to positions in `zero`
where `dot` <= `dol`. Which mean they point to certain lines in the file.
