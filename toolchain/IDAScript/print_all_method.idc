#include <idc.idc>

static main() {
  auto filename = AskFile(1, "*.txt", "save symbol raw data to");
  if (filename != 0) {
    auto file = fopen(filename, "w");
    if (file != 0) {
      auto addr = 0;
      for (addr = NextFunction(addr); addr != BADADDR; addr = NextFunction(addr)) {
          auto name = GetFunctionName(addr);
          auto end = GetFunctionAttr(addr, FUNCATTR_END);
          Message("0x%x\t0x%x\t%s\n", addr, end, name);
          fprintf(file, "0x%x\t0x%x\t%s\n", addr, end, name);
      }
    }
    fclose(file);
  }
}
