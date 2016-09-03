# DSYMCreator
Symbol restore tool for iOS apps whose debug info are stripped. 

With IDA Pro's help, it can create a DSYM/DWARF file. You can import the symbol file to `lldb` by yourself. 

---

## Usage

0. **Prepare Part 1.** The tool needs IDA Pro. So You should install it first. 

   **Demo version is enough!** It's free, you can download from [here](https://www.hex-rays.com/products/ida/support/download_demo.shtml).
   
1. **Prepare Part 2.** If the executable binary is fetched from `App Store`, it likely is encrypted, you should decrypt the `armv7` part first by [dumpdecrypted](https://github.com/stefanesser/dumpdecrypted).

2. **Action.** 

	```shell
	$ ./main.py /path/to/binary/xxx
	```
	Well done! Now your task in only wait. Then a symbol file `/path/to/binary/xxx.symbol` will be create.
	
	Then you can import it by yourself. Have Fun!
	
## Doc

1. [For Better iOS Debug : recreate debug symbol table](https://github.com/imoldman/DSYMCreator/blob/master/doc/For%20Better%20iOS%20Debug%201.md) [Chinese].