echo "running fuzzers...\n"
rm -rf out-AFL-Org out-AFL-Fast out-Fair-Fuzz out-TAFL-MemDensity out-TAFL-InstNum out-TAFL-Depth out-TAFL-EntryDegree
gnome-terminal -e "../../../afl-fuzz-org -i ../fuzzer_input -o out-AFL-Org -d ./installed-Org/bin/md5sum  -c @@"

gnome-terminal -e "../../../afl-fast-fuzz -i ../fuzzer_input -o out-AFL-Fast -d ./installed-Org/bin/md5sum -c @@"

gnome-terminal -e "../../../fairfuzz -i ../fuzzer_input -o out-Fair-Fuzz -d ./installed-Org/bin/md5sum -c @@"

gnome-terminal -e "../../../tafl-fuzz -i ../fuzzer_input -o out-TAFL-MemDensity -d -G BBMemDensity ./installed-MemDensity/bin/md5sum -c @@"

gnome-terminal -e "../../../tafl-fuzz -i ../fuzzer_input -o out-TAFL-InstNum -d -G BBInstNum ./installed-InstNum/bin/md5sum -c @@"

gnome-terminal -e "../../../tafl-fuzz -i ../fuzzer_input -o out-TAFL-Depth -d -G BBDepth ./installed-Depth/bin/md5sum -c @@"

gnome-terminal -e "../../../tafl-fuzz -i ../fuzzer_input -o out-TAFL-EntryDegree -d -G BBEntryDegree ./installed-EntryDegree/bin/md5sum -c @@"

