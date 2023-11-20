rm -rf out-AFL-NoSchedule out-AFL-Schedule out-AFL-Schedule2 out-AFLFast-NoSchedule out-AFLFast-Schedule out-AFLFast-Schedule2

gnome-terminal -e "../../../afl-fuzz-org -i ../fuzzer_input -o out-AFL-NoSchedule -d ./installed-Org/bin/md5sum -c @@"

gnome-terminal -e "../../../afl-schedule -i ../fuzzer_input -o out-AFL-Schedule -d ./installed-Org/bin/md5sum -c@@"


gnome-terminal -e "../../../afl-schedule2 -i ../fuzzer_input -o out-AFL-Schedule2 -d ./installed-Org/bin/md5sum -c @@"

gnome-terminal -e "../../../afl-fast-fuzz -i ../fuzzer_input -o out-AFLFast-NoSchedule -d ./installed-Org/bin/md5sum -c @@"


gnome-terminal -e "../../../afl-fast-schedule -i ../fuzzer_input -o out-AFLFast-Schedule -d ./installed-Org/bin/md5sum -c @@"

gnome-terminal -e "../../../afl-fast-schedule2 -i ../fuzzer_input -o out-AFLFast-Schedule2 -d ./installed-Org/bin/md5sum -c @@"

