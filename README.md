# extrakb
Now you can use your extra keyboard to run macros
# Usage
## fselect
`sudo fselect /dev/input/event*`
Now fselect monitors for any input from any of these devices.
On input it will its path.
## extrakb
`extrakb /dev/input/eventN | ./handler.sh`
Now extrakb will output keystrokes that the chosen device outputs.
Handler must read them (see handler.sh) and do something with it.
# More features
For some use cases it's enough, but more features are on the may.
# License
MIT
