# DSYMCreator
Symbol restore tool for iOS apps whose debug info are stripped. 

With IDA Pro's help, it can create a DSYM/DWARF file. You can import the symbol file to `lldb` by yourself. 

**Please Note.** If the executable binary is fetched from `App Store`, it likely is encrypted, you should decrypt the `armv7` part first by [dumpdecrypted](https://github.com/stefanesser/dumpdecrypted).

## Usage (default mode)

1. **Install IDA Pro.** The tool needs IDA Pro. So You should install it first. If you don't want to use IDA Pro, see **Usage (only-objc mode)**

   **Demo version is enough!** It's free, you can download from [here](https://www.hex-rays.com/products/ida/support/download_demo.shtml).
   

2. **Create Symbol File.** 

  ```shell
  $ ./main.py --input /path/to/binary/xxx
  ```

Well done! Now your task in only wait (and wait, and wait...). Then a symbol file `/path/to/binary/xxx.symbol` will be created.

Then you can import it to `lldb` by yourself. Have fun!

## Usage (only-objc mode)

If you don't want to use IDA Pro, you can use this mode. In this mode, you don't install any software, just create symbol file like this.

```shell
$ ./main.py --only-objc --input /path/to/binary/xxx
```

Also, a symbol file `/path/to/binary/xxx.symbol` will be created.

Please note, since it only dumps objective-c functions, the function count in symbole file is less than the function count in symbol file created in default mode (i.e. IDA Pro mode).

  
---
注:

Technical Doc : [高效逆向 - 为任意iOS App生成符号表](https://g.hz.netease.com/lixianliang/DSYMCreator/blob/master/doc/all.md)


