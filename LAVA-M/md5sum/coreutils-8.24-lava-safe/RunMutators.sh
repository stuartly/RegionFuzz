rm -rf out-AFL-bitflip out-AFL-interesting out-AFL-arith out-AFL-havoc out-AFL-splice

gnome-terminal -e "../../../afl-fuzz-bitflip -i ../fuzzer_input -o out-AFL-bitflip -d ./installed-Org/bin/md5sum -c @@"

gnome-terminal -e "../../../afl-fuzz-interesting -i ../fuzzer_input -o out-AFL-interesting -d ./installed-Org/bin/md5sum -c @@"

gnome-terminal -e "../../../afl-fuzz-arith -i ../fuzzer_input -o out-AFL-arith -d ./installed-Org/bin/md5sum -c @@"


gnome-terminal -e "../../../afl-fuzz-havoc -i ../fuzzer_input -o out-AFL-havoc -d ./installed-Org/bin/md5sum -c @@"

gnome-terminal -e "../../../afl-fuzz-splice -i ../fuzzer_input -o out-AFL-splice -d ./installed-Org/bin/md5sum -c @@"
