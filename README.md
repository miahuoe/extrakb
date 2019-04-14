# extrakb
Now you can use your extra keyboard to run macros

# Usage
Make sure your keyboard is connected.

Determine which device it is by running fselect:
`sudo fselect /dev/input/event*`
It will output a path like that: /dev/input/eventN
(N is a decimal number)

Disable it (so that xorg does not read its input) by:
`devswitch.sh /dev/input/eventN disable`

Now you can listen to keystrokes by running:
`extrakb /dev/input/eventN | ./handler.sh`
Handler must read the output of extrakb (see handler.sh) and do something with it.

To enable keyboard type:
`devswitch.sh /dev/input/eventN enable`

# More features
For some use cases it's enough, but more features are on the may.

# TODO
- Key support
- Testing

# License
MIT
